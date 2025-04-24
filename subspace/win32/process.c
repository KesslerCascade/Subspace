#include "process.h"
#include <cx/container.h>
#include <cx/fs.h>
#include <cx/string.h>
#include <cx/thread.h>
#include <cx/utils.h>
#include <cx/log.h>

#include <cx/platform/win.h>
#include <cx/platform/win/win_fs.h>

typedef BOOL(WINAPI* LPFN_QFPINW)(HANDLE, DWORD, LPWSTR, PDWORD);
static LPFN_QFPINW fnQueryFullProcessImageNameW;

typedef DWORD(WINAPI* LPFN_GPIFNW)(HANDLE, LPWSTR, DWORD);
static LPFN_GPIFNW fnGetProcessImageFileNameW;

typedef DWORD(WINAPI* LPFN_GMFNEW)(HANDLE, HMODULE, LPWSTR, DWORD);
static LPFN_GMFNEW fnGetModuleFileNameExW;

static LazyInitState psapiinitstate;

static void psapiinit(void* unused)
{
    HMODULE kernel32 = GetModuleHandleW(L"kernel32");
    HMODULE psapi    = LoadLibraryW(L"psapi.dll");

    if (psapi) {
        fnGetProcessImageFileNameW = (LPFN_GPIFNW)GetProcAddress(psapi, "GetProcessImageFileNameW");
        fnGetModuleFileNameExW     = (LPFN_GMFNEW)GetProcAddress(psapi, "GetModuleFileNameExW");
    }

    if (kernel32) {
        fnQueryFullProcessImageNameW = (LPFN_QFPINW)GetProcAddress(kernel32,
                                                                   "QueryFullProcessImageNameW");
    }
}

bool procCloseHandle(ProcessHandle* h)
{
    CloseHandle(*h);
    *h = 0;
    return true;
}

bool procGetName(ProcessHandle* h, string* out)
{
    lazyInit(&psapiinitstate, psapiinit, NULL);

    strClear(out);

    WCHAR buf[1024];
    DWORD len = 1024;

    // try most reliable methods first, but not all of these work on every OS for all combinations
    // of 64-bit and 32-bit processes

    // exact path returned doesn't matter so long as it's consistent between invocations

    if (fnQueryFullProcessImageNameW && fnQueryFullProcessImageNameW(*h, 0, buf, &len)) {
        strFromUTF16(out, buf, len);
        return true;
    }

    if (fnGetModuleFileNameExW) {
        len = fnGetModuleFileNameExW(*h, NULL, buf, len);
        if (len > 0) {
            strFromUTF16(out, buf, len);
            return true;
        }
    }

    if (fnGetProcessImageFileNameW) {
        len = fnGetProcessImageFileNameW(*h, buf, len);
        if (len > 0) {
            strFromUTF16(out, buf, len);
            return true;
        }
    }

    return false;
}

bool procOpenByID(ProcessHandle* out, uint32 id)
{
    HANDLE ret;

    // QueryFullProcessImageNameW wants PROCESS_VM_READ, so try to get that if we can
    ret = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE, FALSE, id);

    // Tests show it doesn't always need it though, and the other methods also don't
    // require it, so fall back to PROCESS_QUERY_INFORMATION if we can't get VM read
    if (!ret)
        ret = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, id);

    if (ret) {
        *out = ret;
        return true;
    }

    return false;
}

bool procGetID(ProcessHandle* h, uint32* id)
{
    *id = GetProcessId(*h);
    return true;
}

bool procRunning(ProcessHandle* h)
{
    DWORD code = 0;

    if (!GetExitCodeProcess(*h, &code))
        return false;

    if (code == STILL_ACTIVE)
        return true;
    return false;
}

typedef struct ProcWatchEntry {
    HANDLE proc;
    uint32 pid;
    procExitCb closecb;
    void* userdata;
} ProcWatchEntry;
saDeclare(ProcWatchEntry);

typedef struct ProcWatchState {
    HANDLE notify;
    Thread* thr;

    sa_ProcWatchEntry pents;
    Mutex lock;
} ProcWatchState;

static int procWatchThread(Thread* self)
{
    ProcWatchState* pws = stvlNextPtr(&self->args);

    HANDLE* hlist = xaAlloc(sizeof(HANDLE));
    while (!atomicLoad(bool, &self->requestExit, Relaxed)) {
        int n = 0;

        withMutex (&pws->lock) {
            n = saSize(pws->pents) + 1;
            xaResize(&hlist, n * sizeof(HANDLE));
            for (int i = 0; i < n - 1; i++) {
                hlist[i] = pws->pents.a[i].proc;
            }

            hlist[n - 1] = pws->notify;
        }

        DWORD ret = WaitForMultipleObjects(n, hlist, false, 10000);
        if (ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + n - 1) {
            // one of the processes exited
            ProcWatchEntry temp;
            withMutex (&pws->lock) {
                // safe to do this without checking; only the watch thread is
                // allowed to remove things from the list
                int i = ret - WAIT_OBJECT_0;

                // callback might take a while, so copy the data and drop the mutex
                temp = pws->pents.a[i];
                saRemove(&pws->pents, i);
            }

            logFmt(Verbose, _S"Process ${uint} exited", stvar(uint32, temp.pid));
            temp.closecb(&temp.proc, temp.pid, temp.userdata);
        }
    }
    return 0;
}

ProcWatchState* procStartWatchThread()
{
    ProcWatchState* pws = xaAlloc(sizeof(ProcWatchState), XA_Zero);
    saInit(&pws->pents, opaque(ProcWatchEntry), 1);
    mutexInit(&pws->lock);
    pws->notify = CreateEventW(NULL, FALSE, FALSE, NULL);

    pws->thr = thrCreate(procWatchThread, _S"Process Watcher", stvar(ptr, pws));
    return pws;
}

bool procWatchAdd(ProcWatchState* pws, ProcessHandle* h, procExitCb closecb, void* userdata)
{
    withMutex (&pws->lock) {
        ProcWatchEntry ent = { 0 };
        ent.proc = *h;
        procGetID(h, &ent.pid);
        ent.closecb  = closecb;
        ent.userdata = userdata;
        saPush(&pws->pents, opaque, ent);
        SetEvent(pws->notify);
    }

    return true;
}

bool procStopWatchThread(ProcWatchState* pws)
{
    withMutex (&pws->lock) {
        thrRequestExit(pws->thr);
        SetEvent(pws->notify);
    }

    thrShutdown(pws->thr);
    thrRelease(&pws->thr);
    saDestroy(&pws->pents);
    CloseHandle(pws->notify);
    mutexDestroy(&pws->lock);
    xaFree(pws);
    return true;
}

ProcessHandle* procRun(string exe, sa_string args)
{
    string cmdline = 0;

    pathFilename(&cmdline, exe);
    for (int i = 0; i < saSize(args); i++) {
        if (strFind(args.a[i], 0, _S" ") != -1) {
            strNConcat(&cmdline, cmdline, _S" \"", args.a[i], _S"\"");
        } else {
            strNConcat(&cmdline, cmdline, _S" ", args.a[i]);
        }
    }

    string tempexe = 0;
    pathToPlatform(&tempexe, exe);
    logFmt(Info, _S"Launching process ${string}", stvar(string, tempexe));
    logFmt(Info, _S"Command line: ${string}", stvar(string, cmdline));
    strDestroy(&tempexe);

    STARTUPINFOW sinfo        = { 0 };
    PROCESS_INFORMATION pinfo = { 0 };
    sinfo.cb                  = sizeof(sinfo);
    BOOL ret                  = CreateProcessW(fsPathToNT(exe),
                              strToUTF16S(cmdline),
                              NULL,
                              NULL,
                              FALSE,
                              0,
                              NULL,
                              NULL,
                              &sinfo,
                              &pinfo);

    if (!ret) {
        WCHAR winmsgbuf[1024];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, winmsgbuf, 1024, NULL);
        string winmsg = 0;
        strFromUTF16(&winmsg, winmsgbuf, cstrLenw(winmsgbuf));
        logFmt(Error, _S"Failed to launch process: ${string}", stvar(string, winmsg));
        strDestroy(&winmsg);
    }

    strDestroy(&cmdline);
    if (ret) {
        // don't need thread handle
        CloseHandle(pinfo.hThread);
        logFmt(Verbose, _S"Process ${uint} started", stvar(uint32, pinfo.dwProcessId));
        return pinfo.hProcess;
    }
    return NULL;
}

#include "cx/debug/crash_private.h"
#include "cx/debug/blackbox.h"
#include "cx/suid/suid.h"
#include "cx/debug/stacktrace.h"
#include "cx/fs/path.h"
#include "cx/container/sarray.h"
#include "cx/xalloc/cstrutil.h"
#include "cx/platform/os.h"
#include "cx/platform/win.h"
#include "win_fs.h"

#include <stdio.h>

// Override the crash handler defaults for specific branches
// #define PUBLIC_URL_OVERRIDE ""
// #define INTERNAL_URL_OVERRIDE ""
// #define PRIVACY_URL_OVERRIDE ""

static volatile uint32 debugSignal = 0;
static const wchar_t debugWaitClass[] = L"DebugWait";

// lots of static stuff; we cannot safely allocate in an exception handler,
// and want to keep stack usage to an absolute minimum
static wchar_t *crashdir;
static wchar_t *reportfile;
static wchar_t *crashhandler;
static wchar_t *crashhandlercmdline;
static char *processname;
static wchar_t *processnamew;
static char crashid[27];

#define ST_MAX_FRAMES 128
static int stframes;
static uintptr_t stacktrace[ST_MAX_FRAMES];

static uint32 _ef_mode;
static bool _ef_canwrite;
static HANDLE _ef_file;
static HANDLE _ef_module;
static DWORD _ef_temp;
static DWORD _ef_temp2;
static char _ef_numbuf[30];
static int _ef_i, _ef_j;
static char _ef_char;
static STARTUPINFOW _ef_startup;
static PROCESS_INFORMATION _ef_processinfo;
static HWND _ef_debugwin;
static RECT _ef_rect;
static MSG _ef_msg;
#if defined(_ARCH_X86)
#pragma warning (disable:4731)
static DWORD _ef_tempebp;
static DWORD _ef_savedebp;
#endif

static const char *_ef_prefix = "  ";

// ugly and stupid but simple enough to run from an exception handler
#define WriteBuf(buf, len) WriteFile(_ef_file, buf, len, &_ef_temp, NULL)
#define WriteStatic(str) WriteBuf(str, sizeof(str) - 1)
#define WriteStr(str) WriteBuf(str, (DWORD)cstrLen(str))
#define WriteNum(num) _itoa_s(num, _ef_numbuf, sizeof(_ef_numbuf), 10); WriteBuf(_ef_numbuf, (DWORD)cstrLen(_ef_numbuf))
#define WriteUNum(num) _ultoa_s(num, _ef_numbuf, sizeof(_ef_numbuf), 10); WriteBuf(_ef_numbuf, (DWORD)cstrLen(_ef_numbuf))
#define WriteHex(num) _ef_numbuf[0] = '0'; _ef_numbuf[1] = 'x'; _ui64toa_s(num, _ef_numbuf+2, sizeof(_ef_numbuf)-2, 16); WriteBuf(_ef_numbuf, (DWORD)cstrLen(_ef_numbuf))
#define WriteVal(id, macro, val, comma) \
    WriteStr(_ef_prefix); \
    WriteStatic("\"" id "\": \""); \
    macro(val); \
    WriteStatic("\"" comma "\r\n")
#define WriteValNQ(id, macro, val, comma) \
    WriteStr(_ef_prefix); \
    WriteStatic("\"" id "\": "); \
    macro(val); \
    WriteStatic(comma "\r\n")
#define WriteValSz(id, val, sz, comma) \
    WriteStr(_ef_prefix); \
    WriteStatic("\"" id "\": \""); \
    WriteBuf(val, sz); \
    WriteStatic("\"" comma "\r\n")
#define WriteCustom(id, str) \
    WriteStr(_ef_prefix); \
    WriteStatic("\"" id "\": " str)

_no_inline static LONG WINAPI dbgExceptionFilter(LPEXCEPTION_POINTERS info)
{
    lazyInit(&_dbgCrashInitState, _dbgCrashInit, 0);
    mutexAcquire(&_dbgCrashMutex);

    _ef_mode = atomicLoad(uint32, &_dbgCrashMode, SeqCst);
    _ef_canwrite = true;

    if (!_dbgCrashTriggerCallbacks(false) || _ef_mode == 0) {
        mutexRelease(&_dbgCrashMutex);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if (_ef_mode & DBG_CrashBreakpoint)
        __debugbreak();

    // try to create crash directory if needed
    _ef_temp = GetFileAttributesW(crashdir);
    if (_ef_temp == INVALID_FILE_ATTRIBUTES || !(_ef_temp & FILE_ATTRIBUTE_DIRECTORY)) {
        if (!CreateDirectoryW(crashdir, NULL))
            _ef_canwrite = false;
    }

    // this is not an efficient way of writing the file AT ALL, but keeps the
    // use of the stack to an absolute minimum
    if (_ef_canwrite) {
        _ef_file = CreateFileW(reportfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, NULL);
        WriteStatic("{\r\n");
        WriteValSz("id", crashid, 26, ",");
        _ef_temp2 = GetCurrentProcessId();
        WriteValNQ("pid", WriteUNum, _ef_temp2, ",");
        WriteVal("name", WriteStr, processname, ",");

        WriteStatic("  \"build\": {\r\n");
        _ef_prefix = "    ";
        // write out version metadata
        for (_ef_i = 0; _ef_i < saSize(_dbgCrashExtraMeta); _ef_i++) {
            if (!_dbgCrashExtraMeta.a[_ef_i].version)
                continue;
            WriteStr(_ef_prefix);
            WriteStatic("\"");
            WriteStr(_dbgCrashExtraMeta.a[_ef_i].name);
            WriteStatic("\": ");
            if (_dbgCrashExtraMeta.a[_ef_i].str) {
                WriteStatic("\"");
                WriteStr(_dbgCrashExtraMeta.a[_ef_i].str);
                if (_ef_i < saSize(_dbgCrashExtraMeta) - 1)
                    WriteStatic("\",\r\n");
                else
                    WriteStatic("\"\r\n");
            } else {
                WriteNum(_dbgCrashExtraMeta.a[_ef_i].val);
                if (_ef_i < saSize(_dbgCrashExtraMeta) - 1)
                    WriteStatic(",\r\n");
                else
                    WriteStatic("\r\n");
            }
        }
        WriteStatic("  },\r\n");
        _ef_prefix = "  ";

        WriteStatic("  \"crash\": {\r\n");
        _ef_prefix = "    ";
        WriteValNQ("crashmode", WriteUNum, _ef_mode, ",");

        // we may have a trace already if this came from an assert or forced 'crash'
        if (stframes == 0) {
            // but if not grab one
#if defined(_ARCH_X64)
            stframes = dbgStackTrace(1, ST_MAX_FRAMES, stacktrace);
#elif defined(_ARCH_X86)
            // x86 is ugly because RtlCaptureStackBackTrace can't walk the stack back
            // through the exception itself. so we have to temporarily swap in ebp
            // from the exception context (good thing we don't use any locals in
            // this function!)
            if (info) {
                _ef_tempebp = info->ContextRecord->Ebp;
                __asm {
                    mov _ef_savedebp, ebp
                    mov ebp, _ef_tempebp
                }
            }
            stframes = dbgStackTrace(0, ST_MAX_FRAMES, stacktrace);
            if (info) {
                __asm {
                    mov ebp, _ef_savedebp
                }
                // now replace the top frame (which is the above call) with the exception address
                stacktrace[0] = (uintptr_t)info->ExceptionRecord->ExceptionAddress;
            }
#endif
        }
        if (stframes > 0) {
            WriteCustom("stacktrace", "[\r\n");
            _ef_prefix = "      ";
            for (_ef_i = 0; _ef_i < stframes; _ef_i++) {
                WriteStr(_ef_prefix);
                WriteStatic("\"");
                WriteHex(stacktrace[_ef_i]);
                if (_ef_i < stframes - 1)
                    WriteStatic("\",\r\n");
                else
                    WriteStatic("\"\r\n");
            }
            _ef_prefix = "    ";
            WriteStr(_ef_prefix);
            WriteStatic("],\r\n");
        }

        if (info) {
            WriteVal("exceptionaddr", WriteHex, (uintptr_t)info->ExceptionRecord->ExceptionAddress, ",");
        }

        // write out custom metadata (mostly from assert failures)
        for (_ef_i = 0; _ef_i < saSize(_dbgCrashExtraMeta); _ef_i++) {
            if (_dbgCrashExtraMeta.a[_ef_i].version)
                continue;
            WriteStr(_ef_prefix);
            WriteStatic("\"");
            WriteStr(_dbgCrashExtraMeta.a[_ef_i].name);
            WriteStatic("\": ");
            if (_dbgCrashExtraMeta.a[_ef_i].str) {
                WriteStatic("\"");
                WriteStr(_dbgCrashExtraMeta.a[_ef_i].str);
                WriteStatic("\",\r\n");
            } else {
                WriteNum(_dbgCrashExtraMeta.a[_ef_i].val);
                WriteStatic(",\r\n");
            }
        }

        _ef_module = GetModuleHandleW(NULL);
        WriteVal("base", WriteHex, (intptr_t)_ef_module, "");
        WriteStatic("  },\r\n");
        _ef_prefix = "  ";

        // internal stuff for crashhandler's use that shouldn't be saved
        WriteStatic("  \"internal\": {\r\n");
        _ef_prefix = "    ";
        if (info) {
            WriteValNQ("exceptionthread", WriteUNum, GetCurrentThreadId(), ",");
            WriteVal("exceptioninfo", WriteHex, (uintptr_t)info, ",");
        }
        if (saSize(_dbgCrashDumpMem) > 0) {
            WriteCustom("dumpmem", "[\r\n");
            for (_ef_i = 0; _ef_i < saSize(_dbgCrashDumpMem); _ef_i++) {
                WriteStatic("      { \"start\": \"");
                WriteHex(_dbgCrashDumpMem.a[_ef_i].start);
                WriteStatic("\", \"end\": \"");
                WriteHex(_dbgCrashDumpMem.a[_ef_i].end);
                if (_ef_i < saSize(_dbgCrashDumpMem) - 1)
                    WriteStatic("\" },\r\n");
                else
                    WriteStatic("\" }\r\n");
            }
            WriteStatic("    ],\r\n");
        }
        // if we're in development mode, write out a memory address for CrashHandler to notify us
        // the user wants to debug
        if (_ef_mode & DBG_CrashDevMode) {
            WriteVal("debugsignal", WriteHex, (uintptr)&debugSignal, ",");
        }
        // Optional override URLs
#ifdef PUBLIC_URL_OVERRIDE
        WriteVal("publicurl", WriteStatic, PUBLIC_URL_OVERRIDE, ",");
#endif
#ifdef INTERNAL_URL_OVERRIDE
        WriteVal("internalurl", WriteStatic, INTERNAL_URL_OVERRIDE, ",");
#endif
#ifdef PRIVACY_URL_OVERRIDE
        WriteVal("privacyurl", WriteStatic, PRIVACY_URL_OVERRIDE, ",");
#endif
        WriteVal("blackbox", WriteHex, (uintptr_t)dbgBlackBox, "");
        WriteStatic("  }\r\n");
        _ef_prefix = "  ";


        WriteStatic("}\r\n");
        CloseHandle(_ef_file);

        // call the crash handler to process the report file
        if (GetFileAttributesW(crashhandler) != INVALID_FILE_ATTRIBUTES) {
            memset(&_ef_startup, 0, sizeof(_ef_startup));
            _ef_startup.cb = sizeof(STARTUPINFOA);
            memset(&_ef_processinfo, 0, sizeof(_ef_processinfo));
            if (CreateProcessW(crashhandler, crashhandlercmdline, NULL, NULL, FALSE,
                               0, NULL, NULL, &_ef_startup, &_ef_processinfo)) {
                // we need to wait for the crash handler to dump the process or at least get module info & blackbox
                WaitForSingleObject(_ef_processinfo.hProcess, INFINITE);
                CloseHandle(_ef_processinfo.hThread);
                CloseHandle(_ef_processinfo.hProcess);

                // check for debug signal
                if ((_ef_mode & DBG_CrashDevMode) && debugSignal) {
                    _ef_mode &= ~DBG_CrashExit;     // do not exit if we're debugging
                    _ef_rect.top = _ef_rect.left = 0;
                    _ef_rect.right = 260;
                    _ef_rect.bottom = 75;
                    AdjustWindowRectEx(&_ef_rect, WS_CAPTION | WS_OVERLAPPED, FALSE, WS_EX_CLIENTEDGE);
                    _ef_debugwin = CreateWindowExW(WS_EX_CLIENTEDGE, debugWaitClass, L"", WS_CAPTION | WS_OVERLAPPED,
                                                   CW_USEDEFAULT, CW_USEDEFAULT, _ef_rect.right - _ef_rect.left, _ef_rect.bottom - _ef_rect.top,
                                                   NULL, NULL, GetModuleHandle(NULL), NULL);
                    SetWindowTextW(_ef_debugwin, processnamew);
                    ShowWindow(_ef_debugwin, SW_SHOW);
                    while (debugSignal && !IsDebuggerPresent()) {
                        _ef_temp = PeekMessageW(&_ef_msg, NULL, 0, 0, PM_REMOVE);
                        if (_ef_temp > 0) {
                            TranslateMessage(&_ef_msg);
                            DispatchMessageW(&_ef_msg);
                        }
                        Sleep(1);
                    }
                    DestroyWindow(_ef_debugwin);
                    if (IsDebuggerPresent())
                        __debugbreak();
                }
            }
        }
    }

    if (_ef_mode & DBG_CrashExit)
        exit(1);

    _dbgCrashTriggerCallbacks(true);

    mutexRelease(&_dbgCrashMutex);
    return EXCEPTION_CONTINUE_SEARCH;
}

static LRESULT CALLBACK DebugWaitProc(HWND hWnd, UINT msg,
                                      WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        CreateWindowExW(0, L"STATIC", L"Waiting for debugger to attach...",
                        WS_VISIBLE | WS_CHILD | SS_CENTER, 10, 10, 250, 30, hWnd,
                        NULL, GetModuleHandle(NULL), NULL);
        CreateWindowExW(0, L"BUTTON", L"&Cancel",
                        WS_VISIBLE | WS_CHILD, 90, 40, 80, 25, hWnd,
                        (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
        return 0;
    case WM_COMMAND:
    case WM_CLOSE:
        _ef_mode |= DBG_CrashExit;
        ShowWindow(hWnd, SW_HIDE);
        debugSignal = 0;
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void dbgInvalidParameterHandler(const wchar_t *w_exp,
                                       const wchar_t *w_func,
                                       const wchar_t *w_file,
                                       unsigned int line,
                                       uintptr_t reserved)
{
    string exp = 0, func = 0, file = 0;

    lazyInit(&_dbgCrashInitState, _dbgCrashInit, 0);

    if (w_exp) {
        strFromUTF16(&exp, w_exp, cstrLenw(w_exp));
        dbgCrashAddMetaStr("assertexpr", strC(exp));
        strDestroy(&exp);
    }
    if (w_func) {
        strFromUTF16(&func, w_func, cstrLenw(w_func));
        dbgCrashAddMetaStr("assertfunc", strC(func));
        strDestroy(&func);
    }
    if (w_file) {
        strFromUTF16(&file, w_file, cstrLenw(w_file));
        dbgCrashAddMetaStr("assertfile", strC(file));
        strDestroy(&file);
    }
    if (line > 0) {
        dbgCrashAddMetaInt("assertline", (int)line);
    }

    stframes = dbgStackTrace(1, ST_MAX_FRAMES, stacktrace);
    __try {
        *(char*)(0) = 0;
    }
    __except (dbgExceptionFilter(GetExceptionInformation()))
    {
    }
}

bool _dbgCrashPlatformInit()
{
    // debug wait window
    WNDCLASSEXW wc = { 0 };

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DebugWaitProc;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = debugWaitClass;
    RegisterClassExW(&wc);

    SetUnhandledExceptionFilter(dbgExceptionFilter);
    _set_invalid_parameter_handler(dbgInvalidParameterHandler);
    return true;
}

_Use_decl_annotations_
bool dbgCrashSetPath(strref path)
{
    SUID crashsuid;

    lazyInit(&_dbgCrashInitState, _dbgCrashInit, 0);

    // clear out old stuff if you're calling this again to change the path
    xaRelease(&crashdir);
    xaRelease(&processname);
    xaRelease(&processnamew);
    xaRelease(&reportfile);
    xaRelease(&crashhandler);
    xaRelease(&crashhandlercmdline);

    // generate an SUID to use as a unique crash identifier
    suidGenPrivate(&crashsuid, 0xff);
    suidEncodeBytes(crashid, &crashsuid);

    string exename = 0, report = 0, exedir = 0, chandler = 0, cmdl = 0;
    wchar_t *tmp;
    fsExe(&exename);
    pathFilename(&exename, exename);
    pathRemoveExt(&exename, exename);

    tmp = fsPathToNT(path);
    crashdir = cstrDupw(tmp);

    // precompute filename we're going to use
    processname = xaAlloc((size_t)strLen(exename) + 1);
    strCopyOut(exename, 0, processname, strLen(exename) + 1);
    processnamew = strToUTF16A(exename);

    strNConcat(&report, path, _S"\\", exename, _S"-", (string)crashid, _S".report");
    tmp = fsPathToNT(report);
    reportfile = cstrDupw(tmp);

    fsExeDir(&exedir);
    pathJoin(&chandler, exedir, _S"crashhandler.exe");
    tmp = fsPathToNT(chandler);
    crashhandler = cstrDupw(tmp);

    // if there isn't a crashhandler in the current dir, check up one level
    if (!fsIsFile(chandler)) {
        pathParent(&exedir, exedir);
        pathJoin(&chandler, exedir, _S"crashhandler.exe");
        if (fsIsFile(chandler)) {
            // found it in the parent, use that one
            xaFree(crashhandler);
            tmp = fsPathToNT(chandler);
            crashhandler = cstrDupw(tmp);
        }
        // if the above doesn't find anything, intentionally falls through to
        // blindly using the original exedir, which will gracefully fail later
    }

    string reportPlatform = 0;
    pathToPlatform(&reportPlatform, report);
    strNConcat(&cmdl, _S"crashhandler.exe \"", reportPlatform, _S"\"");
    crashhandlercmdline = strToUTF16A(cmdl);

    strDestroy(&reportPlatform);
    strDestroy(&exename);
    strDestroy(&report);
    strDestroy(&exedir);
    strDestroy(&chandler);
    strDestroy(&cmdl);

    return true;
}

_no_inline _no_return void dbgCrashNow(int skip)
{
    lazyInit(&_dbgCrashInitState, _dbgCrashInit, 0);

    stframes = dbgStackTrace(skip + 1, ST_MAX_FRAMES, stacktrace);
    __try {
        *(char*)(0) = 0;
    }
    __except (dbgExceptionFilter(GetExceptionInformation()))
    {
    }
}

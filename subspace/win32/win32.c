#include <cx/log.h>
#include <cx/platform/base.h>
#include <cx/string.h>

#include <cx/format.h>
#include <cx/platform/win.h>
#include <cx/platform/win/win_time.h>
#include <cx/utils.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#pragma comment(                                                                   \
    linker,                                                                        \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " \
    "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

void fatalError(strref msg, bool winerr)
{
    string mboxtext = 0;

    if (winerr) {
        WCHAR winmsgbuf[1024];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, winmsgbuf, 1024, NULL);
        string winmsg = 0;
        strFromUTF16(&winmsg, winmsgbuf, cstrLenw(winmsgbuf));
        strFormat(&mboxtext, _S"${string}\n\n${string}", stvar(strref, msg), stvar(string, winmsg));
        strDestroy(&winmsg);
    } else {
        strDup(&mboxtext, msg);
    }

    logFmt(Fatal, _S"FATAL ERROR: ${string}", stvar(string, mboxtext));
    MessageBoxW(NULL, strToUTF16S(mboxtext), strToUTF16S(_S"Subspace"), MB_ICONERROR | MB_OK);

    strDestroy(&mboxtext);

    ExitProcess(1);
}

static DYNAMIC_TIME_ZONE_INFORMATION dtzi;

static LazyInitState timezones_init;
static void initTimeZones(void* dummy)
{
    GetDynamicTimeZoneInformation(&dtzi);
}

int64 toLocalTime(int64 time)
{
    lazyInit(&timezones_init, initTimeZones, NULL);

    FILETIME ft;
    SYSTEMTIME st;
    SYSTEMTIME lst;
    TIME_ZONE_INFORMATION tzi;
    TimeParts parts;
    timeDecompose(&parts, time);
    timeToFileTime(time, &ft);
    FileTimeToSystemTime(&ft, &st);
    GetTimeZoneInformationForYear(parts.year, &dtzi, &tzi);
    SystemTimeToTzSpecificLocalTime(&tzi, &st, &lst);
    SystemTimeToFileTime(&lst, &ft);
    return timeFromFileTime(&ft);
}

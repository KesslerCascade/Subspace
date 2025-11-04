#include "log.h"
#include "control/controlclient.h"
#include "ftl/stdlib.h"
#include "subspacegame.h"

#include <stdarg.h>

static bool useclient  = false;
static int logmaxlevel = LOG_Count;

void log_client()
{
    useclient = true;
}

int log_set_max_level(int level)
{
    int old     = logmaxlevel;
    logmaxlevel = level;
    return old;
}

void log_str(int level, const char* str)
{
    if (level > logmaxlevel)
        return;

    ControlMsg* msg = controlMsgCreate(_S"Log");
    cfieldSet(msg, _S"level", int32, level);
    cfieldSet(msg, _S"msg", strref, (strref)str);

    if (useclient) {
        controlClientQueue(msg);
    } else {
        controlSendMsg(&control, msg);
        controlMsgDestroy(msg);
    }
}

// static buffer avoids allocation but means this can ONLY be used from the main thread
static char buf[1024];
void log_fmt(int level, const char* fmt, ...)
{
    if (level > logmaxlevel)
        return;

    va_list arp;
    va_start(arp, fmt);
    vsnprintf(buf, 1024, fmt, arp);
    va_end(arp);
    log_str(level, buf);
}

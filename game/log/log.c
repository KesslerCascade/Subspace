#include "log.h"
#include "control/controlclient.h"
#include "ftl/stdlib.h"
#include "subspacegame.h"

#include <stdarg.h>
#include "minicrt.h"

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

    if (useclient) {
        ControlMsg* msg = controlAllocMsg(2, CF_ALLOC_AUTO);
        strcpy(msg->hdr.cmd, "Log");

        strcpy(msg->fields[0]->h.name, "level");
        msg->fields[0]->h.ftype   = CF_INT;
        msg->fields[0]->d.cfd_int = level;

        strcpy(msg->fields[1]->h.name, "msg");
        msg->fields[1]->h.ftype   = CF_STRING;
        size_t len                = strlen(str);
        msg->fields[1]->d.cfd_str = malloc(len + 1);
        memcpy(msg->fields[1]->d.cfd_str, str, len + 1);

        controlClientQueue(msg);

    } else {
        ControlMsgHeader hdr = { .cmd = "Log", .nfields = 2 };

        ControlField f1 = { 0 };
        strcpy(f1.h.name, "level");
        f1.h.ftype   = CF_INT;
        f1.d.cfd_int = level;

        ControlField f2 = { 0 };
        strcpy(f2.h.name, "msg");
        f2.h.ftype   = CF_STRING;
        f2.d.cfd_str = (char*)str;

        ControlField* fields[2] = { &f1, &f2 };
        controlPutMsg(&control, &hdr, fields);
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
    xvsnprintf(buf, 1024, fmt, arp);
    va_end(arp);
    log_str(level, buf);
}

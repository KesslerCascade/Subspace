#include "netsocket.h"

#include "feature/feature.h"
#include "ftl/misc.h"
#include "controlconnect.h"
#include "subspacegame.h"

#include "minicrt.h"

bool controlConnect(socket_t* sock)
{
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(settings.addr);
    addr.sin_port             = htons(settings.port);

    socket_t csock = socket(PF_INET, SOCK_STREAM, 0);
    if (!csock) {
        osShowError("An unexpected network error occurred.");
        return false;
    }

    if (connect(csock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        osShowError("Failed to connect to main Subspace program.");
        return false;
    }

    *sock = csock;
    return true;
}

void controlDisconnect(socket_t* sock)
{
    netClose(*sock);
    *sock = 0;
}

void controlSendGameStart(ControlState* cs)
{
    ControlMsgHeader mh = { 0 };
    ControlField vf = { 0 }, majvf = { 0 }, minvf = { 0 }, cookief = { 0 };

    mh.nfields = 4;
    strcpy(mh.cmd, "GameStart");
    strcpy(vf.h.name, "ver");
    vf.h.ftype   = CF_STRING;
    vf.d.cfd_str = (char*)subspace_version_str;

    strcpy(majvf.h.name, "major");
    majvf.h.ftype   = CF_INT;
    majvf.d.cfd_int = subspace_version_maj;

    strcpy(minvf.h.name, "minor");
    minvf.h.ftype   = CF_INT;
    minvf.d.cfd_int = subspace_version_min;

    strcpy(cookief.h.name, "cookie");
    cookief.h.ftype    = CF_INT;
    cookief.h.flags    = CF_UNSIGNED;
    cookief.d.cfd_uint = settings.cookie;

    ControlField* fields[4] = { &vf, &majvf, &minvf, &cookief };
    controlPutMsg(cs, &mh, fields);
    while (controlSend(cs)) {}
}

// simple handler for getting the startup information packet decoded before we have nicities like
// the client thread or better memory allocation up and running
static char strbuf[MAX_PATH];
int controlRecvLaunchCmd(ControlState* cs)
{
    int retries = 5;
    fd_set rset;
    struct timeval sto = { 0 };
    sto.tv_sec         = 10;
    while (!controlMsgReady(cs)) {
        if (cs->closed)
            return RLC_Error;

        FD_ZERO(&rset);
        FD_SET(cs->sock, &rset);
        select(cs->sock + 1, &rset, NULL, NULL, &sto);

        if (!FD_ISSET(cs->sock, &rset) && retries-- <= 0)
            return RLC_Timeout;
    }

    ControlMsgHeader hdr;
    if (!controlGetHeader(cs, &hdr))
        return RLC_Error;

    if (!strcmp(hdr.cmd, "Exit"))
        return RLC_Exit;

    if (strcmp(hdr.cmd, "Launch") != 0)
        return RLC_Error;

    for (int i = 0; i < hdr.nfields; i++) {
        ControlField f;
        f.d.cfd_str = strbuf;
        f.count     = MAX_PATH;
        if (!controlGetField(cs, &f, CF_ALLOC_PRE))
            return RLC_Error;

        if (!strcmp(f.h.name, "launchmode")) {
            settings.mode = f.d.cfd_int;
            if (settings.mode < LAUNCH_PLAY || settings.mode > LAUNCH_VALIDATE)
                return RLC_Error;
        }
        if (!strcmp(f.h.name, "gamedir")) {
            settings.gameDir = sstrdup(f.d.cfd_str);
        }
        if (!strcmp(f.h.name, "gameprogram")) {
            settings.gameProgram = sstrdup(f.d.cfd_str);
        }
        if (!strcmp(f.h.name, "gamepath")) {
            settings.gamePath = sstrdup(f.d.cfd_str);
        }
        if (!strcmp(f.h.name, "saveoverride")) {
            // FTL needs this to end in a backslash, so ensure that it does
            if (f.d.cfd_str[strlen(f.d.cfd_str) - 1] == '\\') {
                settings.saveOverride = sstrdup(f.d.cfd_str);
            } else {
                size_t len            = strlen(f.d.cfd_str);
                settings.saveOverride = smalloc(len + 2);
                memcpy(settings.saveOverride, f.d.cfd_str, len);
                settings.saveOverride[len]     = '\\';
                settings.saveOverride[len + 1] = '\0';
            }
        }
    }

    controlRecvDone(cs);
    return RLC_Launch;
}

void controlSendValidate(ControlState* cs, bool success, int failreason)
{
    ControlMsg* msg;
    msg = controlAllocMsg(3, CF_ALLOC_SALLOC);

    strcpy(msg->hdr.cmd, "Validate");
    controlMsgBool(msg, 0, "result", success);

    if (success) {
        ControlField* verf = msg->fields[1];
        strcpy(verf->h.name, "ver");
        verf->h.ftype          = CF_INT;
        verf->h.flags          = CF_ARRAY;
        verf->count            = 3;
        verf->d.cfd_int_arr    = smalloc(sizeof(int) * 3);
        verf->d.cfd_int_arr[0] = g_version_major;
        verf->d.cfd_int_arr[1] = g_version_minor;
        verf->d.cfd_int_arr[2] = g_version_rev;

        ControlField* featf = msg->fields[2];
        strcpy(featf->h.name, "features");
        featf->h.ftype       = CF_STRING;
        featf->h.flags       = CF_ARRAY;
        featf->d.cfd_str_arr = smalloc(sizeof(char*));

        fillValidateFeatures(featf);
    } else {
        controlMsgInt(msg, 1, "reason", failreason);
        controlMsgInt(msg, 2, "unused", 0);
    }

    controlPutMsg(cs, &msg->hdr, msg->fields);
    while (controlSend(cs)) {}
    controlMsgFree(msg, CF_ALLOC_SALLOC);
}

int controlStartupHandshake(ControlState* cs)
{
    controlSendGameStart(cs);

    int lcmd = controlRecvLaunchCmd(cs);
    switch (lcmd) {
    case RLC_Timeout:
        osShowError("Main subspace program did not respond");
        break;
    case RLC_Error:
        osShowError("An unexpected communication error occured");
        break;
    case RLC_Exit:
        break;
    }
    return lcmd;
}

void controlSendLaunchFail(ControlState* cs, int failreason)
{
    if (settings.mode == LAUNCH_VALIDATE) {
        // in validation mode we want to send back a validation result, not a launch failure
        controlSendValidate(cs, false, failreason);
        return;
    }

    ControlMsgHeader mh = { 0 };
    ControlField ff     = { 0 };

    mh.nfields = 1;
    strcpy(mh.cmd, "LaunchFail");
    strcpy(ff.h.name, "reason");
    ff.h.ftype   = CF_INT;
    ff.d.cfd_int = failreason;

    ControlField* fields[1] = { &ff };
    controlPutMsg(cs, &mh, fields);
    while (controlSend(cs)) {}
}

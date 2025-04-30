#include "controlconnect.h"
#include "net.h"
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
    }

    controlRecvDone(cs);
    return RLC_Launch;
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

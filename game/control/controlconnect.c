#include <cx/string.h>
#include "netsocket.h"

#include "feature/feature.h"
#include "ftl/misc.h"
#include "controlconnect.h"
#include "subspacegame.h"

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
    ControlMsg* msg = controlMsgCreate(_S "GameStart");

    cfieldSet(msg, _S"ver", strref, (strref)subspace_version_str);
    cfieldSet(msg, _S"major", int32, subspace_version_maj);
    cfieldSet(msg, _S"minor", int32, subspace_version_min);
    cfieldSet(msg, _S"cookie", uint32, settings.cookie);

    controlSendMsg(cs, msg);
    while (controlSendBuffer(cs)) {}
    controlMsgDestroy(msg);
}

int controlRecvLaunchCmd(ControlState* cs)
{
    int ret     = RLC_Error;
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

    ControlMsg* msg = controlRecvMsg(cs);
    if (!msg)
        goto out;   // RLC_Error

    if (!strcmp(msg->hdr.cmd, "Exit")) {
        ret = RLC_Exit;
        goto out;
    }

    if (strcmp(msg->hdr.cmd, "Launch") != 0)
        goto out;   // RLC_Error

    settings.mode = cfieldValD(int32, msg->fields, _S"launchmode", -1);
    if (settings.mode < LAUNCH_PLAY || settings.mode > LAUNCH_VALIDATE)
        goto out;   // RLC_Error;

    strref str       = cfieldString(msg->fields, _S"gamedir");
    settings.gameDir = xa_strdup(strC(str));

    str                  = cfieldString(msg->fields, _S"gameprogram");
    settings.gameProgram = xa_strdup(strC(str));

    str               = cfieldString(msg->fields, _S"gamepath");
    settings.gamePath = xa_strdup(strC(str));

    str = cfieldString(msg->fields, _S"saveoverride");
    if (!strEmpty(str)) {
        // FTL needs this to end in a backslash, so ensure that it does
        char* strbuf = strBuffer(&str, 0);
        if (strbuf[strLen(str) - 1] == '\\') {
            settings.saveOverride = xa_strdup(strC(str));
        } else {
            size_t len            = strLen(str);
            settings.saveOverride = xaAlloc(len + 2);
            memcpy(settings.saveOverride, strbuf, len);
            settings.saveOverride[len]     = '\\';
            settings.saveOverride[len + 1] = '\0';
        }
        strDestroy(&str);
    }

    ret = RLC_Launch;

out:
    controlMsgDestroy(msg);
    return ret;
}

void controlSendValidate(ControlState* cs, bool success, int failreason)
{
    ControlMsg* msg;
    msg = controlMsgCreate(_S "Validate");
    cfieldSet(msg, _S"result", bool, success);

    if (success) {
        sa_int32 ver;
        saInit(&ver, int32, 3);
        saPush(&ver, int32, g_version_major);
        saPush(&ver, int32, g_version_minor);
        saPush(&ver, int32, g_version_rev);
        htInsert(&msg->fields, strref, _S"ver", stvar, stvar(sarray, ver));
        saDestroy(&ver);

        sa_string features;
        saInit(&features, string, 16);
        fillValidateFeatures(&features);
        htInsert(&msg->fields, strref, _S"features", stvar, stvar(sarray, features));
        saDestroy(&features);
    } else {
        cfieldSet(msg, _S"reason", int32, failreason);
        cfieldSet(msg, _S"unused", int32, 0);   // TODO: can remove now that we don't need to know
                                              // number of fields ahead of time?
    }

    controlSendMsg(cs, msg);
    while (controlSendBuffer(cs)) {}
    controlMsgDestroy(msg);
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

    ControlMsg* msg = controlMsgCreate(_S "LaunchFail");
    cfieldSet(msg, _S"reason", int32, failreason);
    controlSendMsg(cs, msg);
    while (controlSendBuffer(cs)) {}
    controlMsgDestroy(msg);
}

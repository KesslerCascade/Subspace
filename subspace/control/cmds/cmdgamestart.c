#include "control/cmds.h"
#include "gamemgr/gamemgr.h"

void cmdGameStart(GameInst* inst, ControlClient* client, ControlMsg* msg, hashtable fields)
{
    if (inst)
        return;   // why is an already registered instance sending GameStart??

    Subspace* ss = client->ss;

    uint32 cookie = cfieldValD(uint32, fields, _S"cookie", 0);

    // link up the game instance and the cookie
    inst = cookie != 0 ? gmgrAcquireByCookie(ss->gmgr, cookie) : NULL;
    if (!inst && ss->devmode && cookie == 0xDEADBEEF) {
        // special cookie for manually running a client in dev mode.
        // in this case we create one when it connects
        inst = ginstCreateForClient(ss->gmgr, client, cookie);
        // use the default FTL executable for it
        ssdStringOut(ss->settings, _S"ftl/exe", &inst->exepath);
        gmgrReg(ss->gmgr, inst);

        objDestroyWeak(&ss->curinst);
        ss->curinst = objGetWeak(GameInst, inst);
    }

    if (inst) {
        // link the instance and client together
        objDestroyWeak(&client->inst);
        objDestroyWeak(&inst->client);
        client->inst = objGetWeak(GameInst, inst);
        inst->client = objGetWeak(ControlClient, client);
    }

    if (inst && !strEmpty(inst->exepath)) {
        string saveovr = 0;
        ssdStringOut(ss->settings, _S"ftl/saveoverride", &saveovr);
        int nargs = strEmpty(saveovr) ? 4 : 5;

        ControlMsg* rmsg  = controlNewMsg("Launch", nargs);
        string tmpstr     = 0;
        rmsg->hdr.replyid = msg->hdr.msgid;
        controlMsgInt(rmsg, 0, "launchmode", inst->mode);

        pathParent(&tmpstr, inst->exepath);
        pathToPlatform(&tmpstr, tmpstr);
        controlMsgStr(rmsg, 1, "gamedir", tmpstr);

        pathFilename(&tmpstr, inst->exepath);
        pathToPlatform(&tmpstr, tmpstr);
        controlMsgStr(rmsg, 2, "gameprogram", tmpstr);

        pathToPlatform(&tmpstr, inst->exepath);
        controlMsgStr(rmsg, 3, "gamepath", tmpstr);

        if (!strEmpty(saveovr)) {
            pathToPlatform(&tmpstr, saveovr);
            controlMsgStr(rmsg, 4, "saveoverride", tmpstr);
        }

        strDestroy(&saveovr);
        strDestroy(&tmpstr);
        cclientQueue(client, rmsg);
    } else {
        if (!inst) {
            logFmt(Warn,
                   _S"Game connected with invalid cookie ${uint(hex,min:8)}",
                   stvar(uint32, cookie));
        }

        // Something wrong with this client, tell it to exit
        ControlMsg* rmsg  = controlNewMsg("Exit", 0);
        rmsg->hdr.replyid = msg->hdr.msgid;
        cclientQueue(client, rmsg);
    }
}

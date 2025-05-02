#include "gamemgr/gamemgr.h"
#include "cmds.h"

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
    }

    if (inst) {
        // link the instance and client together
        objDestroyWeak(&client->inst);
        objDestroyWeak(&inst->client);
        client->inst = objGetWeak(GameInst, inst);
        inst->client = objGetWeak(ControlClient, client);
    }

    if (inst && !strEmpty(inst->exepath)) {
        ControlMsg* rmsg  = controlNewMsg("Launch", 4);
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

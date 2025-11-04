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

        // set this client as the focused instance
        subspaceSetGame(ss, inst);
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

        ControlMsg* rmsg  = controlMsgCreate(_S"Launch");
        string tmpstr     = 0;
        rmsg->hdr.replyid = msg->hdr.msgid;
        cfieldSet(rmsg, _S"launchmode", int32, inst->mode);

        pathParent(&tmpstr, inst->exepath);
        pathToPlatform(&tmpstr, tmpstr);
        cfieldSet(rmsg, _S"gamedir", string, tmpstr);

        pathFilename(&tmpstr, inst->exepath);
        pathToPlatform(&tmpstr, tmpstr);
        cfieldSet(rmsg, _S"gameprogram", string, tmpstr);

        pathToPlatform(&tmpstr, inst->exepath);
        cfieldSet(rmsg, _S"gamepath", string, tmpstr);

        if (!strEmpty(saveovr)) {
            if (strEq(saveovr, _S"[User]")) {
                strDup(&tmpstr, _S"user/");
            } else {
                pathMakeAbsolute(&saveovr, saveovr);
                pathToPlatform(&tmpstr, saveovr);
            }
            cfieldSet(rmsg, _S"saveoverride", string, tmpstr);
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
        ControlMsg* rmsg  = controlMsgCreate(_S"Exit");
        rmsg->hdr.replyid = msg->hdr.msgid;
        cclientQueue(client, rmsg);
    }
}

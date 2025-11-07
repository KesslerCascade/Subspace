#include <cx/utils/lazyinit.h>

#include "control/controlclient.h"
#include "ftl/stdlib.h"
#include "gamelog.h"
#include "subspacegame.h"

#include <stdarg.h>

LazyInitState gameloginitstate;

static Mutex gameloglock;
saDeclarePtr(ControlMsg);
static sa_ControlMsg deferred;
static atomic(bool) useclient;

static void gameloginit(void* unused)
{
    mutexInit(&gameloglock);
    saInit(&deferred, ptr, 32);
}

static void gamelogDest(int level, _In_opt_ LogCategory* cat, int64 timestamp, _In_opt_ strref msg,
                        uint32 batchid, _In_opt_ void* userdata)
{
    ControlMsg* cmsg = controlMsgCreate(_S"LogMsg");
    cfieldSet(cmsg, _S"level", int32, level);
    cfieldSet(cmsg, _S"timestamp", int64, timestamp);
    cfieldSet(cmsg, _S"msg", strref, (strref)msg);
    cfieldSet(cmsg, _S"batchid", uint32, batchid);

    if (atomicLoad(bool, &useclient, Relaxed)) {
        controlClientQueue(cmsg);
    } else {
        withMutex (&gameloglock) {
            saPush(&deferred, ptr, cmsg);
        }
    }
}

void gameLogSwitchToClientQueue(void)
{
    atomicStore(bool, &useclient, true, Relaxed);
    gameLogSend();
}

void gameLogRegister(void)
{
    lazyInit(&gameloginitstate, gameloginit, 0);
    logRegisterDest(LOG_Debug, NULL, gamelogDest, NULL);
}

void gameLogSend(void)
{
    lazyInit(&gameloginitstate, gameloginit, 0);

    logFlush();

    withMutex (&gameloglock) {
        foreach (sarray, i, ControlMsg*, msg, deferred) {
            controlSendMsg(&control, msg);
            controlMsgDestroy(msg);
        }
        saClear(&deferred);
        controlSendBuffer(&control);
    }
}

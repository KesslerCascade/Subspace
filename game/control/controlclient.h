#pragma once

#include "control.h"

typedef struct MessageQueue {
    uint32_t maxmsgs;
    uint32_t nmsgs;
    ControlMsg** msgs;
} MessageQueue;

enum RecvLaunchCmdResult { RLC_Timeout = 0, RLC_Launch, RLC_Exit, RLC_Error };

// Standalone static startup messages without memory allocation
void controlSendGameStart(ControlState* cs);
int controlRecvLaunchCmd(ControlState* cs);

bool controlClientStart(void);
void controlClientStop(void);
void controlClientNotify(void);

void controlClientQueue(ControlMsg** msg);   // queue an outbound message
void controlClientProcess(void);   // dispatch inbound messages to callbacks in main thread

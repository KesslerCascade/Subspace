#pragma once

#include "control.h"

typedef struct MessageQueue {
    uint32_t maxmsgs;
    uint32_t nmsgs;
    ControlMsg** msgs;
} MessageQueue;

// Standalone static startup messages without memory allocation
void controlSendStartup(ControlState* cs);
void controlRecvStartup(ControlState* cs);

bool controlClientStart(void);
void controlClientStop(void);
void controlClientNotify(void);

void controlClientQueue(ControlMsg** msg);   // queue an outbound message
void controlClientProcess(void);   // dispatch inbound messages to callbacks in main thread

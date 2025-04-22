#pragma once

#include "controlclient.h"

typedef struct MessageQueue {
    uint32_t maxmsgs;
    uint32_t nmsgs;
    ControlMsg** msgs;
    controlclientcb_t* cbs;
} MessageQueue;

MessageQueue* msgqCreate(int initsz, bool withcbs);
void msgqAdd(MessageQueue* queue, ControlMsg* msg, controlclientcb_t cb);
void msgqClear(MessageQueue* queue);
void msgqDestroy(MessageQueue* queue);

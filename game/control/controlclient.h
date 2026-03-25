#pragma once

#include "control.h"

typedef void (*controlclientcb_t)(ControlMsg* msg, hashtable fields);

bool controlClientStart(void);
void controlClientStop(void);
void controlClientNotify(void);
bool controlClientConnected(void);

void controlClientQueue(ControlMsg* msg);   // queue an outbound message
void controlClientProcessInbound(void);     // dispatch inbound messages to callbacks in main thread
void controlClientProcessOutbound(void);    // release queued outbound messages to send
void controlClientRegister(strref cmd, controlclientcb_t cb);

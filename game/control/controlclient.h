#pragma once

#include "control.h"

typedef void (*controlclientcb_t)(ControlMsg* msg);

bool controlClientStart(void);
void controlClientStop(void);
void controlClientNotify(void);

void controlClientQueue(ControlMsg* msg);   // queue an outbound message
void controlClientProcess(void);   // dispatch inbound messages to callbacks in main thread
void controlClientRegister(const char* cmd, controlclientcb_t cb);

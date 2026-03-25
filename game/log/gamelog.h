#pragma once

#include <cx/log.h>

void gameLogRegister(void);

// Force sync and transmit pending logs. This only has an effect after startup but before the client queue is being used.
void gameLogSend(void);

// Notify log system that control client thread is up and running and it should switch to that instead of direct sends.
void gameLogSwitchToClientQueue(void);

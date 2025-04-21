#pragma once

#include "control.h"

bool controlServerStart(void);
void controlServerStop(void);
void controlServerNotify(void);
int controlPort();

void controlServerQueue(ControlMsg** msg);   // queue an outbound message

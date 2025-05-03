#pragma once

#include "control.h"
#include "proto.h"

bool controlConnect(socket_t *sock);
void controlDisconnect(socket_t* sock);

enum RecvLaunchCmdResult { RLC_Timeout = 0, RLC_Launch, RLC_Exit, RLC_Error };

// Standalone static startup messages without memory allocation
void controlSendGameStart(ControlState* cs);
int controlRecvLaunchCmd(ControlState* cs);
void controlSendValidate(ControlState* cs, bool success, int failreason);

// controlSendGameStart + controlRecvLaunchCmd + error handling
int controlStartupHandshake(ControlState* cs);

void controlSendLaunchFail(ControlState* cs, int failreason);
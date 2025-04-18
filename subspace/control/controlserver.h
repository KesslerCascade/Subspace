#pragma once

#include "control.h"
#include "net.h"
#include <cx/container.h>
#include <cx/thread.h>
#include <cx/thread/prqueue.h>

saDeclare(socket_t);
saDeclarePtr(ControlState);

typedef struct ControlServer {
    Thread *thread;
    sa_ControlState clients;

    socket_t svrsock;
    int port;

    socket_t notifysock;    // dummy socket to wake up the thread
    int notifyport;
} ControlServer;

bool controlServerStart(void);
void controlServerStop(void);
void controlServerNotify(void);
int controlPort();
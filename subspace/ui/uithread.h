#pragma once

#include <cx/taskqueue.h>

typedef struct Subspace Subspace;

typedef struct UIThread {
    TaskQueue* uiq;
    TaskQueue* uiworkers;
    Subspace* ss;
    bool started;
} UIThread;

bool uiInit(UIThread* uithr, Subspace* ss);
bool uiShutdown(UIThread* uithr);
void uiStart(UIThread* uithr);
bool uiStop(UIThread* uithr);
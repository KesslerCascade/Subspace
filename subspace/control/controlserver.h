#pragma once

#include <cx/obj.h>
#include "control.h"
#include "subspace.h"

typedef struct Task Task;
typedef struct ControlClient ControlClient;
typedef _objfactory_guaranteed Task* (*ctask_factory_t)(ControlClient* client, ControlMsg* msg);

bool controlServerStart(Subspace* subspace);
void controlServerStop(void);
void controlServerNotify(void);
bool controlServerRegisterHandler(const char* cmd, ctask_factory_t handler);
ctask_factory_t controlServerGetHandler(const char* cmd);
int controlPort();

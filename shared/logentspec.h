#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LOG_MAX_PARAMS 4
typedef enum LogParamEnum { LP_NONE = 0, LP_INT, LP_STRING } LogParamEnum;

typedef struct LogEntSpec {
    const char* id;
    bool combat;
    int numParams;
    int priority;
    LogParamEnum paramTypes[LOG_MAX_PARAMS];
    const char* paramNames[LOG_MAX_PARAMS];
} LogEntSpec;

extern LogEntSpec Log_Scrap;
extern LogEntSpec Log_Victory;
extern LogEntSpec Log_Defeat;
extern LogEntSpec Log_Abandon;
extern LogEntSpec Log_HullDamage;
extern LogEntSpec Log_Jump;
extern LogEntSpec Log_Wait;
extern LogEntSpec Log_Sector;
extern LogEntSpec Log_Event;
extern LogEntSpec Log_Ship;
extern LogEntSpec Log_Start;
extern LogEntSpec Log_InvAdd;
extern LogEntSpec Log_InvMove;
extern LogEntSpec Log_InvRemove;
extern LogEntSpec Log_Resource;
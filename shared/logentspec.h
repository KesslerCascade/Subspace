#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LOG_MAX_PARAMS 4
typedef enum LogParamEnum { LP_NONE = 0, LP_INT, LP_STRING } LogParamEnum;

typedef struct LogEntSpec {
    const char* id;
    bool combat;
    int numParams;
    LogParamEnum paramTypes[LOG_MAX_PARAMS];
    const char* paramNames[LOG_MAX_PARAMS];
} LogEntSpec;

extern LogEntSpec Log_Scrap;
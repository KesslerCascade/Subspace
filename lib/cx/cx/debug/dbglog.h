#pragma once

#include <cx/cx.h>

// memory log bufer to be included in crash dumps

CX_C_BEGIN

#define DBGLOG_SIZE 65535
extern char *dbgLog;

void dbgLogEnable(int level);
void dbgLogDisable();

CX_C_END

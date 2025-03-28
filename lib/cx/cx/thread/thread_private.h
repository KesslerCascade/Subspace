#pragma once

#include "thread.h"

_Ret_valid_ Thread *_thrPlatformCreate();
bool _thrPlatformStart(Thread *thread);
bool _thrPlatformWait(Thread *thread, int64 timeout);

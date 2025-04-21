#include "osdep.h"
#include <windows.h>
#include <process.h>
#include "ftl/stdlib.h"

bool osStartThread(threadfunc_t func, void* data)
{
    return _beginthread(func, 0, data) > 0;
}
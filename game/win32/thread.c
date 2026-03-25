#include <cx/cx.h>
#include <process.h>
#include <windows.h>
#include "ftl/stdlib.h"
#include "osdep.h"

bool osStartThread(threadfunc_t func, void* data)
{
    return _beginthread(func, 0, data) > 0;
}
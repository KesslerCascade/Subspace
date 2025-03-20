#ifdef _WIN32

#include <windows.h>
#include "minicrt.h"

void lock_init(lock_t* lck)
{
    InitializeCriticalSection((CRITICAL_SECTION*)lck);
}

void lock_acq(lock_t* lck)
{
    EnterCriticalSection((CRITICAL_SECTION*)lck);
}

void lock_rel(lock_t* lck)
{
    LeaveCriticalSection((CRITICAL_SECTION*)lck);
}
#endif
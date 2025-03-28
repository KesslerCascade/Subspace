#pragma once

#include <cx/cx.h>
#include "futex.h"
#include "aspin.h"
#include "mutex.h"

// Condition variable: A cross between an event and a mutex

enum CONDVAR_Flags {
    CONDVAR_NoSpin = 1,     // do not use adaptive spin, use kernel futex only
};

typedef struct CondVar {
    Futex seq;
    atomic(uint32) lastseq;
    AdaptiveSpin aspin;
} CondVar;

void _cvarInit(_Out_ CondVar *cv, uint32 flags);
#define cvarInit(cv, ...) _cvarInit(cv, opt_flags(__VA_ARGS__))

void cvarDestroy(_Inout_ CondVar *cv);

_Requires_lock_held_(*m)
bool cvarWaitTimeout(_Inout_ CondVar *cv, _Inout_ Mutex *m, int64 timeout);
_Requires_lock_held_(*m)
_meta_inline bool cvarWait(_Inout_ CondVar *cv, _Inout_ Mutex *m)
{
    return cvarWaitTimeout(cv, m, timeForever);
}
bool cvarSignal(_Inout_ CondVar *cv);
bool cvarBroadcast(_Inout_ CondVar *cv);

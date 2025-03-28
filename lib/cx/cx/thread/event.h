#pragma once

#include <cx/cx.h>
#include "futex.h"
#include "aspin.h"

// Event: Similar to auto-reset events on Windows.
// Basically a semaphore that has a maximum value of 1, but includes some extra
// features such as releasing an arbitrary number of threads on demand.

// Also can be locked in a signaled state that turns it into the equivalent of
// a manual-reset event. This is useful for things like thread shutdown, where
// you want to avoid any potential races without having to spam the event.

CX_C_BEGIN

typedef struct UIEvent UIEvent;
typedef struct Event {
    // Futex values:
    //  0 - Event is not signaled, any thread will have to wait
    //  1 - Event is signaled
    // >1 - Event is signaled for multiple waiters to wake up
    // -1 - Event is locked, all waits will complete instantly
    Futex ftx;
    atomic(int32) waiters;
    AdaptiveSpin aspin;
    UIEvent *uiev;
} Event;

enum EVENTINITFUNC_FLAGS {
    EV_Spin = 1,            // Use adaptive spinloop instead of going straight to sleep
    EV_UIEvent = 2,         // May be woken up early by platform-specific UI events
};

// A common pattern is an event that is shared between two threads for the purpose of
// one signaling the other when something is complete. This can be make managing the
// lifetime of the event itself difficult, as it is not safe for it to live either
// on the stack of the caller, or for one side or the other to free it.

// SharedEvent provides a wrapper of event that implements an extremely simple reference
// count so that the event can be cleaned up when both threads are done with it.

typedef struct SharedEvent {
    Event ev;
    atomic(uintptr) ref;
} SharedEvent;

// Events normally do not use the adaptive spin framework.
// This is because we assume that events are used for occasional signaling between threads,
// and when a thread waits on an event it's more likely than not to need to sleep. It would
// be a waste of CPU for the event to spin waiting for a signal that is very unlikely to
// come before the spinloop ends.
//
// For events used by high-performance queues or other situations where it is expected that
// there will usually be work to do, the caller should initialize the Event with a "Spin" flag,
// which will cause the event to use adaptive spinning.
void _eventInit(_Out_ Event *e, uint32 flags);
#define eventInit(e, ...) _eventInit(e, opt_flags(__VA_ARGS__))

#define eventSignal(e) eventSignalMany(e, 1)
bool eventSignalMany(_Inout_ Event *e, int32 count);
bool eventSignalAll(_Inout_ Event *e);

bool eventWaitTimeout(_Inout_ Event *e, uint64 timeout);
_meta_inline void eventWait(_Inout_ Event *e)
{
    eventWaitTimeout(e, timeForever);
}

// signals the event and locks it in the signaled state, so threads attempting to wait on it
// always return immediately
bool eventSignalLock(_Inout_ Event *e);
// manually reset the event to an unsignaled state
bool eventReset(_Inout_ Event *e);
void eventDestroy(_Pre_valid_ _Post_invalid_ Event *e);

_Ret_valid_
SharedEvent *sheventCreate(uint32 flags);
_Ret_valid_
SharedEvent *sheventAcquire(_In_ SharedEvent *ev);
_At_(*pev, _Pre_maybenull_ _Post_null_)
void sheventRelease(_Inout_ SharedEvent** pev);

CX_C_END

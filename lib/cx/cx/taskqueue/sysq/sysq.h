#pragma once

#include <cx/taskqueue/taskqueue.h>

// Similar to systhread, sysqueue is a task queue for background tasks run asynchronously by the
// library itself. It is started on an as-needed basis and shut down at program exit.

bool _sysqAdd(_In_ BasicTask* task);
// bool sysqAdd(BasicTask *task);
// Add a task to the system queue to run immediately.
#define sysqAdd(task) _sysqAdd(BasicTask(task))

// void sysqRun(BasicTask **ptask);
// Convenience function to run a task and release it.
#define syqRun(ptask)      \
    do {                   \
        sysqAdd(*ptask);   \
        objRelease(ptask); \
    } while (0)

bool _sysqSchedule(_In_ ComplexTask* task, int64 delay);
// bool sysqSchedule(ComplexTask *task, int64 delay);
// Add a task to the system queue to run after a delay. Requires a complex task.
#define sysqSchedule(task, delay) _sysqSchedule(ComplexTask(task), delay)

bool _sysqDefer(_In_ ComplexTask* task);
// bool sysqDefer(ComplexTask *task);
// Add a task to the queue to be held indefinitely until advanced with taskAdvance.
#define sysqDefer(task) _sysqDefer(ComplexTask(task))

// Runs a custom function on a thread in a task queue's worker pool
bool sysqCall(_In_ UserTaskCB func, _In_opt_ void* userdata);

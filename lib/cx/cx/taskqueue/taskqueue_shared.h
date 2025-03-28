#pragma once

typedef struct TaskQueue TaskQueue;
typedef struct LogCategory LogCategory;
typedef struct Event Event;

// Callback for UI events. Should return true if all UI events were processed,
// or false if there are more pending and the worker should continue looping.
typedef bool (*TQUICallback)(TaskQueue* tq);

enum TaskQueueFlagsEnum {
    // The task queue needs to periodically perform maintenance of the queue to purge completed
    // tasks and release deferred tasks. Without this flag, the maintenance is scheduled on an idle
    // worker thread. With it, a dedicated manager thread performs it on a schedule and as needed.
    TQ_ManagerThread = 0x0001,

    // Periodically check the queue for stalled tasks and emit log messages to help debug the state
    // of the queue.
    TQ_Monitor = 0x0002,

    // Create a queue that can only accept non-complex tasks -- i.e. BasicTask or Task derived
    // objects. Uses less complex code with less overhead.
    TQ_NoComplex = 0x0004,

    // No thread pool, the queue's tick() function must be called manually. This is not compatible
    // with TQ_ManagerThread or TQ_Monitor;
    TQ_Manual = 0x0008,

    // In manual mode, tick() runs as many tasks as possible. The Oneshot flag changes this behavior
    // to only ever run a single task per call to tick(), which may be desired if this queue is
    // being ticked in a loop with other processing.
    TQ_Oneshot = 0x0010,
};

typedef struct TaskQueueThreadPoolConfig {
    int wInitial;      // initial number of workers upon starting the queue
    int wIdle;         // absolute minimum number of workers when queue is completely idle
    int wBusy;         // target number of workers when the queue is busy
    int wMax;          // absolute maximum number of workers

    int64 tIdle;       // length of time before the queue is considered idle
    int64 tRampUp;     // how long to wait between addings workers to the pool
    int64 tRampDown;   // how long to wait before removing a worker to the pool

    int loadFactor;    // average queue depth per worker that is acceptable

    TQUICallback ui;   // if set, this pool is handling UI events and should use an EV_UIEvent
                       // event, as well as call this callback from the worker thread(s)
} TaskQueueThreadPoolConfig;

typedef struct TaskQueueMonitorConfig {
    int64 mInterval;        // If nonzero, how often for the queue monitor to run
    int64 mSuppress;        // How long to suppress monitor output after a warning is emitted
    int64 mTaskRunning;     // Warn about tasks that have been running on a worker for longer than
                            // this time
    int64 mTaskWaiting;     // Warn about tasks that have been waiting for a worker to run on longer
                            // than this time
    int64 mTaskStalled;     // Warn about deferred tasks that have not made progress in this time
    LogCategory* mLogCat;   // If set, send monitor logs to a custom category
} TaskQueueMonitorConfig;

// Configuration for pool of queue workers
typedef struct TaskQueueConfig {
    uint32 flags;
    int64 mGC;              // how often a garbage collection cycle should run
    TaskQueueThreadPoolConfig pool;
    TaskQueueMonitorConfig monitor;
} TaskQueueConfig;

enum TaskStateEnum {
    TASK_Created,
    TASK_Waiting,
    TASK_Running,
    TASK_Scheduled,
    TASK_Deferred,
    TASK_Succeeded,
    TASK_Failed,
    TASK_State_Mask = 0x7fffffff,
    TASK_Cancelled  = 0x80000000
};

typedef struct TaskControl {
    // ---- Task and derivatives only ----

    // Event to signal after task is completed or failed.
    Event* notifyev;

    // ---- ComplexTask and derivatives only ----

    // How long to wait to run this task again.
    // A delay of -1 indicates that the task should be run whenever any other task completes.
    int64 delay;
} TaskControl;

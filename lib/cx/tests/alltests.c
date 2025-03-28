#include <stdio.h>
#include <cx/time.h>

// Run all tests in a single process, to make manual testing easier

typedef struct testspec {
    const char *descr;
    const char *module;
    const char *test;
} testspec;

static const testspec tests[] = {
{ "debug: Assertions", "debugtest", "assert" },
{ "xalloc: Allocation", "cxmemtest", "alloc" },
{ "xalloc: Usable Size", "cxmemtest", "usable_size" },
{ "xalloc: Free", "cxmemtest", "free" },
{ "xalloc: Reallocation", "cxmemtest", "realloc" },
{ "closure: Basic Closure", "closuretest", "closure" },
{ "closure: Closure Chain", "closuretest", "chain" },
{ "format: Integers", "fmttest", "int" },
{ "format: Floating Point", "fmttest", "float" },
{ "format: Objects", "fmttest", "object" },
{ "format: Pointers", "fmttest", "ptr" },
{ "format: Strings", "fmttest", "string" },
{ "format: SUID", "fmttest", "suid" },
{ "format: Array indexing", "fmttest", "array" },
{ "format: Hashtable lookup", "fmttest", "hash" },
{ "format: Defaults", "fmttest", "default" },
{ "format: Error handling", "fmttest", "error" },
{ "log: Log Levels", "logtest", "levels" },
{ "log: Batch", "logtest", "batch" },
{ "log: Shutdown", "logtest", "shutdown" },
{ "log: Categories", "logtest", "categories" },
{ "log: Deferred logging", "logtest", "defer" },
{ "object: Interface", "objtest", "iface" },
{ "object: Class Inheritance", "objtest", "inherit" },
{ "object: Interface Inheritance", "objtest", "ifinherit" },
{ "object: Override", "objtest", "override" },
{ "object: Abstract Class", "objtest", "abstract" },
{ "object: Static Cast", "objtest", "cast" },
{ "object: Dynamic Cast", "objtest", "dyncast" },
{ "object: Object Array", "objtest", "obj_array" },
{ "object: Weak References", "objtest", "weakref" },
{ "sarray: Int Array", "sarraytest", "int" },
{ "sarray: Sorted Int Array", "sarraytest", "sorted_int" },
{ "sarray: String Array", "sarraytest", "string" },
{ "sarray: Sort", "sarraytest", "sort" },
{ "sarray: String Sort", "sarraytest", "string_sort" },
{ "string: Join", "strtest", "join" },
{ "string: Append/Prepend", "strtest", "append" },
{ "string: Substrings", "strtest", "substr" },
{ "string: Compare", "strtest", "compare" },
{ "string: Long Strings", "strtest", "longstring" },
{ "string: Ropes", "strtest", "rope" },
{ "convert: Numeric Types", "converttest", "numeric" },
{ "convert: Strings", "converttest", "string" },
{ "convert: Objects", "converttest", "object" },
{ "thread: Basic multithreading", "thrtest", "basic" },
{ "thread: Futex", "thrtest", "futex" },
{ "thread: Futex with timeout", "thrtest", "timeout" },
{ "thread: Semaphore", "thrtest", "sema" },
{ "thread: Mutex", "thrtest", "mutex" },
{ "thread: Reader/writer lock", "thrtest", "rwlock" },
{ "thread: Event", "thrtest", "event" },
{ "thread: Event (with spin)", "thrtest", "event_s" },
{ "thread: Condition Variable", "thrtest", "condvar" },
{ "meta: Block Wrapping", "metatest", "wrap" },
{ "meta: Protected Blocks", "metatest", "protect" },
{ "meta: Basic Try/Catch" , "metatest", "ptry" },
{ "meta: Unhandled Exceptions" , "metatest", "unhandled" },
{ "meta: Rethrow" , "metatest", "rethrow" },
{ "meta: Cross-function Exception Handling" , "metatest", "xfunc" },
{ "ssd: Semi-structured data tree" , "ssdtest", "tree" },
{ "ssd: Single value root" , "ssdtest", "single" },
{ "ssd: Subtrees" , "ssdtest", "subtree" },
{ "ssd: Arrays" , "ssdtest", "array" },
{ "streambuffer: Push" , "sbtest", "push" },
{ "streambuffer: Direct Push" , "sbtest", "direct" },
{ "streambuffer: Pull" , "sbtest", "pull" },
{ "streambuffer: Peek-ahead" , "sbtest", "peek" },
{ "streambuffer: String Adapter", "sbtest", "string" },
{ "lineparse: Explicit EOL", "lineparsetest", "explicit" },
{ "lineparse: Auto EOL", "lineparsetest", "auto" },
{ "lineparse: Mixed EOL", "lineparsetest", "mixed" },
{ "lineparse: Push Mode", "lineparsetest", "push" },
{ "json: Parse", "jsontest", "parse" },
{ "json: Output", "jsontest", "out" },
{ "json: Tree Parse", "jsontest", "treeparse" },
{ "json: Tree Output", "jsontest", "treeout" },
{ "math: PCG Random Integer", "mathtest", "pcgint" },
{ "math: PCG Random Float", "mathtest", "pcgfloat" },
{ "math: PCG Error Condition", "mathtest", "pcgerror" },
{ "math: FP Comparison", "mathtest", "floatcmp" },
{ "fs: Path Matching", "fstest", "pathmatch" },
{ "prqueue: Basic Pointer-Ring Queue", "prqtest", "basic" },
{ "prqueue: Multithreaded Queue", "prqtest", "mt" },
{ "prqueue: Queue Growth", "prqtest", "grow" },
{ "prqueue: Garbage Collection", "prqtest", "gc" },
{ "prqueue: Dynamic MPMC", "prqtest", "mpmc" },
{ "taskqueue: Tasks", "tqtest", "task" },
{ "taskqueue: Failed Tasks", "tqtest", "failure" },
{ "taskqueue: Concurrency (in-worker)", "tqtest", "concurrency_inworker" },
{ "taskqueue: Concurrency (dedicated)", "tqtest", "concurrency_dedicated" },
{ "taskqueue: Call", "tqtest", "call" },
{ "taskqueue: Scheduled Tasks", "tqtest", "sched" },
{ "taskqueue: Monitor (in-worker)", "tqtest", "monitor_inworker" },
{ "taskqueue: Monitor (dedicated)", "tqtest", "monitor_dedicated" },
{ "taskqueue: Dependencies", "tqtest", "depend" },
{ "taskqueue: Require Mutex", "tqtest", "reqmutex" },
{ "taskqueue: Require FIFO", "tqtest", "reqfifo" },
{ "taskqueue: Require LIFO", "tqtest", "reqlifo" },
{ "taskqueue: Require Gate", "tqtest", "reqgate" },
{ "taskqueue: Dependency Timeout", "tqtest", "timeout" },
{ "taskqueue: Manual", "tqtest", "manual" },
{ "taskqueue: Oneshot", "tqtest", "oneshot" },
{ "taskqueue: Multiphase", "tqtest", "multiphase" },
{ 0 }
};

extern int main(int argc, const char *argv[]);

int alltests(int argc, const char *argv[])
{
    const testspec *s;
    int ntests = 0;

    for (s = &tests[0]; s->descr; s++) {
        ntests++;
    }

    if (ntests == 0) {
        puts("No tests to run!");
        return 1;
    }

    int npassed = 0;
    int64 totalstart = clockTimer();
    const char *fake_argv[3];

    fake_argv[0] = argv[0];

    puts("Running tests...");

    for (int i = 0; i < ntests; i++)
    {
        int64 tstart = clockTimer();
        printf("%4d/%d: %s ", i + 1, ntests, tests[i].descr);
        int ndots = 40 - (int)strlen(tests[i].descr);
        for (int j = 0; j < ndots; j++)
            putchar('.');
        fflush(stdout);

        fake_argv[1] = tests[i].module;
        fake_argv[2] = tests[i].test;
        bool res = main(3, fake_argv);
        int64 tend = clockTimer();

        printf("    %s    %4d msec\n",
                res ? "Failed" : "Passed", (int)timeToMsec(tend - tstart));
        if (!res)
            npassed++;
        xaFlush();
    }

    int64 totalend = clockTimer();
    printf("\n%d%% tests passed, %d tests failed out of %d, total elapsed %d msec\n",
            npassed * 100 / ntests, ntests - npassed, ntests,
            (int)timeToMsec(totalend - totalstart));

    return ntests - npassed;
}

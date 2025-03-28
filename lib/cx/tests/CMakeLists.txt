include (${CX_TOP_SOURCE_DIR}/cmake/werror.cmake)
include (CTest)

create_test_sourcelist(testsrc
    test_runner.c
    debugtest.c
    cxmemtest.c
    closuretest.c
    fmttest.c
    logtest.c
    objtest.c
    sarraytest.c
    strtest.c
    thrtest.c
    metatest.c
    ssdtest.c
    sbtest.c
    lineparsetest.c
    jsontest.c
    converttest.c
    mathtest.c
    fstest.c
    prqtest.c
    tqtest.c
    alltests.c)
add_sidl(converttestobj.sidl
    fmttestobj.sidl
    objtestobj.sidl
    tqtestobj.sidl)
set(test_runner_files
    ${testsrc}
    fmttestobj.c
    fmttestobj.h
    fmttestobj.sidl
    objtestobj.c
    objtestobj.h
    objtestobj.sidl
    converttestobj.c
    converttestobj.h
    converttestobj.sidl
    tqtestobj.c
    tqtestobj.h
    tqtestobj.sidl
    common.h)

source_group("" FILES ${test_runner_files})

add_executable(test_runner ${test_runner_files})
target_include_directories(test_runner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_runner cx)

if(CX_PLATFORM_IS_WASM)
    target_link_options(test_runner PRIVATE "-sPTHREAD_POOL_SIZE=16" "-sPROXY_TO_PTHREAD" "-sEXIT_RUNTIME")
endif()

set_property(TARGET test_runner PROPERTY FOLDER "tests")

add_test(NAME "\"debug: Assertions\"" COMMAND test_runner debugtest assert)
add_test(NAME "\"xalloc: Allocation\"" COMMAND test_runner cxmemtest alloc)
add_test(NAME "\"xalloc: Usable Size\"" COMMAND test_runner cxmemtest usable_size)
add_test(NAME "\"xalloc: Free\"" COMMAND test_runner cxmemtest free)
add_test(NAME "\"xalloc: Reallocation\"" COMMAND test_runner cxmemtest realloc)
add_test(NAME "\"closure: Basic Closure\"" COMMAND test_runner closuretest closure)
add_test(NAME "\"closure: Closure Chain\"" COMMAND test_runner closuretest chain)
add_test(NAME "\"format: Integers\"" COMMAND test_runner fmttest int)
add_test(NAME "\"format: Floating Point\"" COMMAND test_runner fmttest float)
add_test(NAME "\"format: Objects\"" COMMAND test_runner fmttest object)
add_test(NAME "\"format: Pointers\"" COMMAND test_runner fmttest ptr)
add_test(NAME "\"format: Strings\"" COMMAND test_runner fmttest string)
add_test(NAME "\"format: SUID\"" COMMAND test_runner fmttest suid)
add_test(NAME "\"format: Array indexing\"" COMMAND test_runner fmttest array)
add_test(NAME "\"format: Hashtable lookup\"" COMMAND test_runner fmttest hash)
add_test(NAME "\"format: Defaults\"" COMMAND test_runner fmttest default)
add_test(NAME "\"format: Error handling\"" COMMAND test_runner fmttest error)
add_test(NAME "\"log: Log Levels\"" COMMAND test_runner logtest levels)
add_test(NAME "\"log: Batch\"" COMMAND test_runner logtest batch)
add_test(NAME "\"log: Shutdown\"" COMMAND test_runner logtest shutdown)
add_test(NAME "\"log: Categories\"" COMMAND test_runner logtest categories)
add_test(NAME "\"log: Deferred logging\"" COMMAND test_runner logtest defer)
add_test(NAME "\"object: Interface\"" COMMAND test_runner objtest iface)
add_test(NAME "\"object: Class Inheritance\"" COMMAND test_runner objtest inherit)
add_test(NAME "\"object: Interface Inheritance\"" COMMAND test_runner objtest ifinherit)
add_test(NAME "\"object: Override\"" COMMAND test_runner objtest override)
add_test(NAME "\"object: Abstract Class\"" COMMAND test_runner objtest abstract)
add_test(NAME "\"object: Static Cast\"" COMMAND test_runner objtest cast)
add_test(NAME "\"object: Dynamic Cast\"" COMMAND test_runner objtest dyncast)
add_test(NAME "\"object: Object Array\"" COMMAND test_runner objtest obj_array)
add_test(NAME "\"object: Weak References\"" COMMAND test_runner objtest weakref)
add_test(NAME "\"sarray: Int Array\"" COMMAND test_runner sarraytest int)
add_test(NAME "\"sarray: Sorted Int Array\"" COMMAND test_runner sarraytest sorted_int)
add_test(NAME "\"sarray: String Array\"" COMMAND test_runner sarraytest string)
add_test(NAME "\"sarray: Sort\"" COMMAND test_runner sarraytest sort)
add_test(NAME "\"sarray: String Sort\"" COMMAND test_runner sarraytest string_sort)
add_test(NAME "\"string: Join\"" COMMAND test_runner strtest join)
add_test(NAME "\"string: Append/Prepend\"" COMMAND test_runner strtest append)
add_test(NAME "\"string: Substrings\"" COMMAND test_runner strtest substr)
add_test(NAME "\"string: Compare\"" COMMAND test_runner strtest compare)
add_test(NAME "\"string: Long Strings\"" COMMAND test_runner strtest longstring)
add_test(NAME "\"string: Ropes\"" COMMAND test_runner strtest rope)
add_test(NAME "\"convert: Numeric Types\"" COMMAND test_runner converttest numeric)
add_test(NAME "\"convert: Strings\"" COMMAND test_runner converttest string)
add_test(NAME "\"convert: Objects\"" COMMAND test_runner converttest object)
add_test(NAME "\"thread: Basic multithreading\"" COMMAND test_runner thrtest basic)
add_test(NAME "\"thread: Futex\"" COMMAND test_runner thrtest futex)
add_test(NAME "\"thread: Futex with timeout\"" COMMAND test_runner thrtest timeout)
add_test(NAME "\"thread: Semaphore\"" COMMAND test_runner thrtest sema)
add_test(NAME "\"thread: Mutex\"" COMMAND test_runner thrtest mutex)
add_test(NAME "\"thread: Reader/writer lock\"" COMMAND test_runner thrtest rwlock)
add_test(NAME "\"thread: Event\"" COMMAND test_runner thrtest event)
add_test(NAME "\"thread: Event (with spin)\"" COMMAND test_runner thrtest event_s)
add_test(NAME "\"thread: Condition Variable\"" COMMAND test_runner thrtest condvar)
add_test(NAME "\"meta: Block Wrapping\"" COMMAND test_runner metatest wrap)
add_test(NAME "\"meta: Protected Blocks\"" COMMAND test_runner metatest protect)
add_test(NAME "\"meta: Basic Try/Catch\"" COMMAND test_runner metatest ptry)
add_test(NAME "\"meta: Unhandled Exceptions\"" COMMAND test_runner metatest unhandled)
add_test(NAME "\"meta: Rethrow\"" COMMAND test_runner metatest rethrow)
add_test(NAME "\"meta: Cross-function Exception Handling\"" COMMAND test_runner metatest xfunc)
add_test(NAME "\"ssd: Semi-structured data tree\"" COMMAND test_runner ssdtest tree)
add_test(NAME "\"ssd: Single value root\"" COMMAND test_runner ssdtest single)
add_test(NAME "\"ssd: Subtrees\"" COMMAND test_runner ssdtest subtree)
add_test(NAME "\"ssd: Arrays\"" COMMAND test_runner ssdtest array)
add_test(NAME "\"streambuffer: Push\"" COMMAND test_runner sbtest push)
add_test(NAME "\"streambuffer: Direct Push\"" COMMAND test_runner sbtest direct)
add_test(NAME "\"streambuffer: Pull\"" COMMAND test_runner sbtest pull)
add_test(NAME "\"streambuffer: Peek-ahead\"" COMMAND test_runner sbtest peek)
add_test(NAME "\"streambuffer: String Adapter\"" COMMAND test_runner sbtest string)
add_test(NAME "\"lineparse: Explicit EOL\"" COMMAND test_runner lineparsetest explicit)
add_test(NAME "\"lineparse: Auto EOL\"" COMMAND test_runner lineparsetest auto)
add_test(NAME "\"lineparse: Mixed EOL\"" COMMAND test_runner lineparsetest mixed)
add_test(NAME "\"lineparse: Push Mode\"" COMMAND test_runner lineparsetest push)
add_test(NAME "\"json: Parse\"" COMMAND test_runner jsontest parse)
add_test(NAME "\"json: Output\"" COMMAND test_runner jsontest out)
add_test(NAME "\"json: Tree Parse\"" COMMAND test_runner jsontest treeparse)
add_test(NAME "\"json: Tree Output\"" COMMAND test_runner jsontest treeout)
add_test(NAME "\"math: PCG Random Integer\"" COMMAND test_runner mathtest pcgint)
add_test(NAME "\"math: PCG Random Float\"" COMMAND test_runner mathtest pcgfloat)
add_test(NAME "\"math: PCG Error Condition\"" COMMAND test_runner mathtest pcgerror)
add_test(NAME "\"math: FP Comparison\"" COMMAND test_runner mathtest floatcmp)
add_test(NAME "\"fs: Path Matching\"" COMMAND test_runner fstest pathmatch)
add_test(NAME "\"prqueue: Basic Pointer-Ring Queue\"" COMMAND test_runner prqtest basic)
add_test(NAME "\"prqueue: Multithreaded Queue\"" COMMAND test_runner prqtest mt)
add_test(NAME "\"prqueue: Queue Growth\"" COMMAND test_runner prqtest grow)
add_test(NAME "\"prqueue: Garbage Collection\"" COMMAND test_runner prqtest gc)
add_test(NAME "\"prqueue: Dynamic MPMC\"" COMMAND test_runner prqtest mpmc)
add_test(NAME "\"taskqueue: Tasks\"" COMMAND test_runner tqtest task)
add_test(NAME "\"taskqueue: Failed Tasks\"" COMMAND test_runner tqtest failure)
add_test(NAME "\"taskqueue: Concurrency (in-worker)\"" COMMAND test_runner tqtest concurrency_inworker)
add_test(NAME "\"taskqueue: Concurrency (dedicated)\"" COMMAND test_runner tqtest concurrency_dedicated)
add_test(NAME "\"taskqueue: Call\"" COMMAND test_runner tqtest call)
add_test(NAME "\"taskqueue: Scheduled Tasks\"" COMMAND test_runner tqtest sched)
add_test(NAME "\"taskqueue: Monitor (in-worker)\"" COMMAND test_runner tqtest monitor_inworker)
add_test(NAME "\"taskqueue: Monitor (dedicated)\"" COMMAND test_runner tqtest monitor_dedicated)
add_test(NAME "\"taskqueue: Dependencies\"" COMMAND test_runner tqtest depend)
add_test(NAME "\"taskqueue: Require Mutex\"" COMMAND test_runner tqtest reqmutex)
add_test(NAME "\"taskqueue: Require FIFO\"" COMMAND test_runner tqtest reqfifo)
add_test(NAME "\"taskqueue: Require LIFO\"" COMMAND test_runner tqtest reqlifo)
add_test(NAME "\"taskqueue: Require Gate\"" COMMAND test_runner tqtest reqgate)
add_test(NAME "\"taskqueue: Dependency Timeout\"" COMMAND test_runner tqtest timeout)
add_test(NAME "\"taskqueue: Manual\"" COMMAND test_runner tqtest manual)
add_test(NAME "\"taskqueue: Oneshot\"" COMMAND test_runner tqtest oneshot)
add_test(NAME "\"taskqueue: Multiphase\"" COMMAND test_runner tqtest multiphase)

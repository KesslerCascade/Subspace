set(TQ_WORKER_SRCS
    taskqueue/worker/tqmanualworker.c
    taskqueue/worker/tqthreadworker.c
    taskqueue/worker/tqworker.c
    )
set(TQ_WORKER_AUTOGEN
    taskqueue/worker/tqmanualworker.cxh
    taskqueue/worker/tqthreadworker.cxh
    taskqueue/worker/tqworker.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_WORKER_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_WORKER_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

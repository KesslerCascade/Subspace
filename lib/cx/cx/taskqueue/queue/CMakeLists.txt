set(TQ_QUEUE_SRCS
    taskqueue/queue/tqcomplex.c
    taskqueue/queue/tqueue.c
    )
set(TQ_QUEUE_AUTOGEN
    taskqueue/queue/tqcomplex.cxh
    taskqueue/queue/tqueue.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_QUEUE_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_QUEUE_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

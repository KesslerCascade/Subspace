set(TQ_TASK_SRCS
    taskqueue/task/basictask.c
    taskqueue/task/complextask.c
    taskqueue/task/mptask.c
    taskqueue/task/task.c
    )
set(TQ_TASK_AUTOGEN
    taskqueue/task/basictask.cxh
    taskqueue/task/complextask.cxh
    taskqueue/task/mptask.cxh
    taskqueue/task/task.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_TASK_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_TASK_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

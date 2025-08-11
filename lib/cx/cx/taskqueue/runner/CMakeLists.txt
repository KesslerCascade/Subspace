set(TQ_RUNNER_SRCS
    taskqueue/runner/tqrmanual.c
    taskqueue/runner/tqrthreadpool.c
    taskqueue/runner/tqrunner.c
    )
set(TQ_RUNNER_AUTOGEN
    taskqueue/runner/tqrmanual.cxh
    taskqueue/runner/tqrthreadpool.cxh
    taskqueue/runner/tqrunner.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_RUNNER_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_RUNNER_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

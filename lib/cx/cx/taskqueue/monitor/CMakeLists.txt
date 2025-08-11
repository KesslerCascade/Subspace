set(TQ_MONITOR_SRCS
    taskqueue/monitor/tqmonitor.c
    taskqueue/monitor/tqtpmonitor.c
    )
set(TQ_MONITOR_AUTOGEN
    taskqueue/monitor/tqmonitor.cxh
    taskqueue/monitor/tqtpmonitor.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_MONITOR_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_MONITOR_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

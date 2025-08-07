set(TQ_RESOURCE_SRCS
    taskqueue/resource/taskresource.c
    taskqueue/resource/trfifo.c
    taskqueue/resource/trlifo.c
    taskqueue/resource/trmutex.c
    )
set(TQ_RESOURCE_AUTOGEN
    taskqueue/resource/taskresource.cxh
    taskqueue/resource/trfifo.cxh
    taskqueue/resource/trlifo.cxh
    taskqueue/resource/trmutex.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_RESOURCE_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_RESOURCE_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

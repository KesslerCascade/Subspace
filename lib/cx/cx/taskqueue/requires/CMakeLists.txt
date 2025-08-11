set(TQ_REQUIRES_SRCS
    taskqueue/requires/taskrequires.c
    taskqueue/requires/taskrequiresgate.c
    taskqueue/requires/taskrequiresresource.c
    taskqueue/requires/taskrequirestask.c
    )
set(TQ_TREQUIRES_AUTOGEN
    taskqueue/requires/taskrequires.cxh
    taskqueue/requires/taskrequiresgate.cxh
    taskqueue/requires/taskrequiresresource.cxh
    taskqueue/requires/taskrequirestask.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_REQUIRES_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_TREQUIRES_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

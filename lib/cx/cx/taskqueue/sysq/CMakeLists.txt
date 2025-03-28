set(TQ_SYSQ_SRCS
    taskqueue/sysq/sysq.c
    )
set(TQ_SYSQ_INCLUDES
    taskqueue/sysq/sysq.h
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_SYSQ_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_INCLUDES
    ${TASKQUEUE_INCLUDES}
    ${TQ_SYSQ_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

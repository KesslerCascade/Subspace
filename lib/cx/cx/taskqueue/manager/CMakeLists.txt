set(TQ_MANAGER_SRCS
    taskqueue/manager/tqmanager.c
    taskqueue/manager/tqmdedicated.c
    taskqueue/manager/tqminworker.c
    taskqueue/manager/tqmmanual.c
    taskqueue/manager/tqmthreadpool.c
    )
set(TQ_MANAGER_AUTOGEN
    taskqueue/manager/tqmanager.cxh
    taskqueue/manager/tqmdedicated.cxh
    taskqueue/manager/tqminworker.cxh
    taskqueue/manager/tqmmanual.cxh
    taskqueue/manager/tqmthreadpool.cxh
    )

set(TASKQUEUE_SRCS
    ${TASKQUEUE_SRCS}
    ${TQ_MANAGER_SRCS}
    PARENT_SCOPE)
set(TASKQUEUE_AUTOGEN
    ${TASKQUEUE_AUTOGEN}
    ${TQ_MANAGER_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

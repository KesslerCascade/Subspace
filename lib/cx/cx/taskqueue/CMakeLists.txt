set(TASKQUEUE_SRCS
    taskqueue/taskqueue.c
    taskqueue/userfunctask.c)
set(TASKQUEUE_INCLUDES
    taskqueue/taskqueue.h
    taskqueue/taskqueue_private.h
    taskqueue/taskqueue_shared.h
    taskqueue/userfunctask.h)
set(TASKQUEUE_SIDL
    taskqueue/userfunctask.sidl)

add_subdirectory(manager)
add_subdirectory(monitor)
add_subdirectory(queue)
add_subdirectory(requires)
add_subdirectory(resource)
add_subdirectory(runner)
add_subdirectory(sysq)
add_subdirectory(task)
add_subdirectory(worker)

set(CX_SRCS
    ${CX_SRCS}
    ${TASKQUEUE_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${TASKQUEUE_INCLUDES}
    PARENT_SCOPE)
set(CX_SIDL
    ${CX_SIDL}
    ${TASKQUEUE_SIDL}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

set(SYS_SRCS
    sys/entry.c
    sys/hostid.c)
set(SYS_INCLUDES
    sys/entry.h
    sys/hostid.h
    sys/hostid_private.h)

set(CX_SRCS
    ${CX_SRCS}
    ${SYS_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${SYS_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

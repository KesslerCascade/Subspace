set(SUID_SRCS
    suid/stype_suid.c
    suid/suid.c)
set(SUID_INCLUDES
    suid/stype_suid.h
    suid/suid.h)

set(CX_SRCS
    ${CX_SRCS}
    ${SUID_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${SUID_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

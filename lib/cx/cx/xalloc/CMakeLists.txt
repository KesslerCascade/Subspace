set(XALLOC_SRCS
    xalloc/cstrutil.c
    xalloc/oom.c
    xalloc/xalloc.c
    xalloc/xaoutput.c)
set(XALLOC_INCLUDES
    xalloc/cstrutil.h
    xalloc/xalloc.h
    xalloc/xalloc_private.h)

set(CX_SRCS
    ${CX_SRCS}
    ${XALLOC_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${XALLOC_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

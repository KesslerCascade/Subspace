set(META_SRCS
    meta/meta.c)
set(META_INCLUDES
    meta/block.h
    meta/pblock.h
    meta/ptry.h)

set(CX_SRCS
    ${CX_SRCS}
    ${META_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${META_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

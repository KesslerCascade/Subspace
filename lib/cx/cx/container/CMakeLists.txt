set(CONTAINER_SRCS
    container/hashtable.c
    container/sarray.c
    container/stype_hashtable.c
    container/stype_sarray.c)
set(CONTAINER_INCLUDES
    container/foreach.h
    container/hashtable.h
    container/hashtable_private.h
    container/sarray.h
    container/sarray_private.h
    container/stype_hashtable.h
    container/stype_sarray.h)

set(CX_SRCS
    ${CX_SRCS}
    ${CONTAINER_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${CONTAINER_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

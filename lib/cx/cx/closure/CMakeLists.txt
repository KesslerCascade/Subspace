set(CLOSURE_SRCS
    closure/cchain.c
    closure/closure.c
    closure/stype_closure.c)
set(CLOSURE_INCLUDES
    closure/cchain.h
    closure/closure.h
    closure/closure_private.h
    closure/stype_closure.h)

set(CX_SRCS
    ${CX_SRCS}
    ${CLOSURE_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${CLOSURE_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

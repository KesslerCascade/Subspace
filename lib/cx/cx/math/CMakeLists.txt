set(MATH_SRCS
    math/pcg.c)
set(MATH_INCLUDES
    math/lcg.h
    math/pcg.h)

set(CX_SRCS
    ${CX_SRCS}
    ${MATH_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${MATH_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

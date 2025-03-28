set(TIME_SRCS
    time/time.c)
set(TIME_INCLUDES
    time/clock.h
    time/time.h)

set(CX_SRCS
    ${CX_SRCS}
    ${TIME_SRCS}
    PARENT_SCOPE)

set(CX_INCLUDES
    ${CX_INCLUDES}
    ${TIME_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

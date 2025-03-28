set(LOG_SRCS
    log/log.c
    log/logdefer.c
    log/logdest.c
    log/logfile.c
    log/logmembuf.c
    log/logqueue.c
    log/logthread.c)
set(LOG_INCLUDES
    log/log.h
    log/logdefer.h
    log/logfile.h
    log/logmembuf.h
    log/log_private.h)

set(CX_SRCS
    ${CX_SRCS}
    ${LOG_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${LOG_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

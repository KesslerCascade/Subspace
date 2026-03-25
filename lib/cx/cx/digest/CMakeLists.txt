set(DIGEST_SRCS
    digest/digest.c
    digest/md5.c
    digest/sha1.c
    digest/sha256.c)
set(DIGEST_INCLUDES
    digest/digest.h
    digest/digest_private.h)

set(CX_SRCS
    ${CX_SRCS}
    ${DIGEST_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${DIGEST_SRCS}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

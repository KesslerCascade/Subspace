SET(FS_VFSFS_SRCS
    fs/vfsfs/vfsfs.c
    fs/vfsfs/vfsfsfile.c)
SET(FS_VFSFS_INCLUDES
    fs/vfsfs/vfsfs.h
    fs/vfsfs/vfsfsfile.h)
set(FS_VFSFS_SIDL
    fs/vfsfs/vfsfs.sidl
    fs/vfsfs/vfsfsfile.sidl)

set(CX_SRCS
    ${CX_SRCS}
    ${FS_VFSFS_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${FS_VFSFS_INCLUDES}
    PARENT_SCOPE)
set(CX_SIDL
    ${CX_SIDL}
    ${FS_VFSFS_SIDL}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

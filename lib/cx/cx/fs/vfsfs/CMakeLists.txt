SET(FS_VFSFS_SRCS
    fs/vfsfs/vfsfs.c
    fs/vfsfs/vfsfsfile.c)
SET(FS_VFSFS_INCLUDES
    fs/vfsfs/vfsfs.h
    fs/vfsfs/vfsfsfile.h)
set(FS_VFSFS_AUTOGEN
    fs/vfsfs/vfsfs.cxh
    fs/vfsfs/vfsfsfile.cxh)

set(CX_SRCS
    ${CX_SRCS}
    ${FS_VFSFS_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${FS_VFSFS_INCLUDES}
    PARENT_SCOPE)
set(CX_AUTOGEN
    ${CX_AUTOGEN}
    ${FS_VFSFS_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

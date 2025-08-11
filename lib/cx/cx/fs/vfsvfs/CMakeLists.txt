set(FS_VFSVFS_SRCS
    fs/vfsvfs/vfsvfs.c
    fs/vfsvfs/vfsvfsfile.c)
set(FS_VFSVFS_INCLUDES
    fs/vfsvfs/vfsvfs.h
    fs/vfsvfs/vfsvfsfile.h)
set(FS_VFSVFS_AUTOGEN
    fs/vfsvfs/vfsvfs.cxh
    fs/vfsvfs/vfsvfsfile.cxh)

set(CX_SRCS
    ${CX_SRCS}
    ${FS_VFSVFS_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${FS_VFSVFS_INCLUDES}
    PARENT_SCOPE)
set(CX_AUTOGEN
    ${CX_AUTOGEN}
    ${FS_VFSVFS_AUTOGEN}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

set(PLATFORM_MSVC_INCLUDES
    platform/msvc/msvc_atomic.h
    platform/msvc/msvc_cpu.h)

set(CX_INCLUDES
    ${CX_INCLUDES}
    ${PLATFORM_MSVC_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

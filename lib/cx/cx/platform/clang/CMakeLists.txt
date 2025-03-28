set(PLATFORM_CLANG_INCLUDES
    platform/clang/clang_atomic.h
    platform/clang/clang_cpu.h)

set(CX_INCLUDES
    ${CX_INCLUDES}
    ${PLATFORM_CLANG_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

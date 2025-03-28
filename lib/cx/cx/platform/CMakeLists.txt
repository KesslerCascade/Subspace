set(PLATFORM_INCLUDES
    platform/base.h
    platform/cpp.h
    platform/cpu.h
    platform/os.h
    platform/uievent.h)

if(CX_COMPILER_IS_CLANG)
    add_subdirectory(clang)
elseif(CX_COMPILER_IS_MSVC)
    add_subdirectory(msvc)
endif()

add_subdirectory(unix)
add_subdirectory(win)
add_subdirectory(wasm)

set(CX_SRCS
    ${CX_SRCS}
    ${PLATFORM_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${PLATFORM_INCLUDES}
    PARENT_SCOPE)
set(CX_SIDL
    ${CX_SIDL}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)

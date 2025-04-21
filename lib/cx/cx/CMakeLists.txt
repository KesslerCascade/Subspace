include (CheckTypeSize)
find_package (Threads REQUIRED)

include (${CX_TOP_SOURCE_DIR}/cmake/werror.cmake)
include (${CX_TOP_SOURCE_DIR}/cmake/buildtype.cmake)

option (CX_USE_SYSTEM_MALLOC "Use system allocator rather than mimalloc" OFF)

add_subdirectory(closure)
add_subdirectory(container)
add_subdirectory(debug)
add_subdirectory(format)
add_subdirectory(fs)
add_subdirectory(log)
add_subdirectory(math)
add_subdirectory(meta)
add_subdirectory(obj)
add_subdirectory(platform)
add_subdirectory(serialize)
add_subdirectory(settings)
add_subdirectory(ssdtree)
add_subdirectory(string)
add_subdirectory(stype)
add_subdirectory(suid)
add_subdirectory(sys)
add_subdirectory(taskqueue)
add_subdirectory(thread)
add_subdirectory(time)
add_subdirectory(utils)
add_subdirectory(xalloc)

set(CX_BASE_INCLUDES
    include/cx/closure.h
    include/cx/common.h
    include/cx/container.h
    include/cx/cx.h
    include/cx/debug.h
    include/cx/fs.h
    include/cx/json.h
    include/cx/math.h
    include/cx/meta.h
    include/cx/obj.h
    include/cx/platform.h
    include/cx/serialize.h
    include/cx/ssdtree.h
    include/cx/static_config.h
    include/cx/string.h
    include/cx/sys.h
    include/cx/taskqueue.h
    include/cx/thread.h
    include/cx/time.h
    include/cx/utils.h
    include/cx/xalloc.h)

if (CX_COMPILER_IS_MSVC)
    set(CX_NATVIS_FILES
        cx.natvis)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${CX_BASE_INCLUDES}
    ${CX_SRCS}
    ${CX_SIDL}
    ${CX_INCLUDES}
    ${CX_NATVIS_FILES}
    ${CX_CMAKE_FILES})

add_sidl(${CX_SIDL})

add_library(cx STATIC
    ${CX_SRCS}
    ${CX_SIDL}
    ${CX_INCLUDES}
    ${CX_BASE_INCLUDES}
    ${CX_NATVIS_FILES}
    ${CX_CMAKE_FILES})

target_link_libraries(cx ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} mimalloc mbedtls mbedx509 mbedcrypto)
target_include_directories(cx SYSTEM PUBLIC ${CX_TOP_SOURCE_DIR} ${CX_TOP_SOURCE_DIR}/cx/include ${CX_TOP_BINARY_DIR})
target_compile_definitions(cx PRIVATE UNICODE _UNICODE)
if (CX_COMPILER_IS_MSVC)
    target_link_options(cx PUBLIC /NATVIS:${CMAKE_CURRENT_SOURCE_DIR}/cx.natvis)
endif()

if (CX_USE_SYSTEM_MALLOC)
    target_compile_definitions(cx PRIVATE XALLOC_USE_SYSTEM_MALLOC)
endif()

target_compile_definitions(cx PUBLIC
    $<${isdebug}:DEBUG_BUILD>
    $<${isdevordebug}:DEV_BUILD>
    $<${isrelease}:RELEASE_BUILD>
    $<${isdebug}:DEBUG_LEVEL=2>
    $<${isdev}:DEBUG_LEVEL=1>
    $<${isrelease}:DEBUG_LEVEL=0>)

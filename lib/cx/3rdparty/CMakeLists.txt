# 3rdparty CMake philosohpy:
# Try to make as few changes as possible to the included library's CMakeLists.txt and override
# things here instead. While not always possible and sometimes edits need to be made, this
# helps reduce merge conflicts.

include (${CX_TOP_SOURCE_DIR}/cmake/buildtype.cmake)

set(MI_OVERRIDE          OFF CACHE INTERNAL "Override the standard malloc interface (e.g. define entry points for malloc() etc)" FORCE)
set(MI_OSX_INTERPOSE     OFF CACHE INTERNAL "Use interpose to override standard malloc on macOS" FORCE)
set(MI_OSX_ZONE          OFF CACHE INTERNAL "Use malloc zone to override standard malloc on macOS" FORCE)
set(MI_WIN_REDIRECT      OFF CACHE INTERNAL "Use redirection module ('mimalloc-redirect') on Windows if compiling mimalloc as a DLL" FORCE)
set(MI_WIN_USE_FIXED_TLS OFF CACHE INTERNAL "Use a fixed TLS slot on Windows to avoid extra tests in the malloc fast path" FORCE)
set(MI_BUILD_SHARED      OFF CACHE INTERNAL "Build shared library" FORCE)
set(MI_BUILD_STATIC      OFF CACHE INTERNAL "Build static library" FORCE)
set(MI_BUILD_OBJECT      OFF CACHE INTERNAL "Build object library" FORCE)
set(MI_BUILD_OBJLIB      ON  CACHE INTERNAL "Build cmake object library" FORCE)
set(MI_BUILD_TESTS       OFF CACHE INTERNAL "Build test executables" FORCE)
if(DIAGNOSTIC)
    set(MI_GUARDED       ON  CACHE INTERNAL "Build with guard pages behind certain object allocations (implies MI_NO_PADDING=ON)" FORCE)
endif()
add_subdirectory(mimalloc)
target_compile_definitions(mimalloc-objlib PRIVATE
    $<${isdebug}:MI_DEBUG=3>
    $<${isdev}:MI_DEBUG=2>
    $<${isrelease}:MI_DEBUG=0>
    $<${isrelease}:MI_SECURE=4>)
add_library(mimalloc ALIAS mimalloc-objlib)

set(ENABLE_PROGRAMS OFF CACHE INTERNAL "Build Mbed TLS programs." FORCE)
set(ENABLE_TESTING  OFF CACHE INTERNAL "Build Mbed TLS tests." FORCE)

set(USE_STATIC_MBEDTLS_LIBRARY OFF CACHE INTERNAL "Build Mbed TLS static library." FORCE)
set(USE_OBJECT_MBEDTLS_LIBRARY ON  CACHE INTERNAL "Build mbed TLS object library." FORCE)
add_subdirectory(mbedtls)
target_include_directories(mbedtls PRIVATE ${CX_TOP_SOURCE_DIR}/cx/include)
target_include_directories(mbedx509 PRIVATE ${CX_TOP_SOURCE_DIR}/cx/include)
target_include_directories(mbedcrypto PRIVATE ${CX_TOP_SOURCE_DIR}/cx/include)
target_compile_definitions(mbedtls PRIVATE
    $<${isdebug}:DEBUG_LEVEL=2>
    $<${isdev}:DEBUG_LEVEL=1>
    $<${isrelease}:DEBUG_LEVEL=0>)
target_compile_definitions(mbedx509 PRIVATE
    $<${isdebug}:DEBUG_LEVEL=2>
    $<${isdev}:DEBUG_LEVEL=1>
    $<${isrelease}:DEBUG_LEVEL=0>)
target_compile_definitions(mbedcrypto PRIVATE
    $<${isdebug}:DEBUG_LEVEL=2>
    $<${isdev}:DEBUG_LEVEL=1>
    $<${isrelease}:DEBUG_LEVEL=0>)

set(PCRE2_SHOW_REPORT     OFF CACHE INTERNAL "Show the final configuration report" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE INTERNAL "Build pcre2grep" FORCE)
set(PCRE2_BUILD_TESTS     OFF CACHE INTERNAL "Build the tests" FORCE)
set(BUILD_STATIC_LIBS     ON  CACHE INTERNAL "Build static libraries." FORCE)
set(BUILD_OBJECT_LIBS     ON  CACHE INTERNAL "Build object libraries." FORCE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(pcre2)
target_include_directories(pcre2-8-static PRIVATE ${CX_TOP_SOURCE_DIR})
target_link_libraries(pcre2-8-static cx)
target_include_directories(pcre2-8-object PRIVATE ${CX_TOP_SOURCE_DIR})

if (CX_LUA)
add_subdirectory(lua)
endif()

organize_targets(CLEAR_FOLDER PREPEND 3rdparty)
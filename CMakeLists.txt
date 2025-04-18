cmake_minimum_required (VERSION 3.21)

include (lib/cx/cmake/organize_targets.cmake)

set(TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SHARED_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shared)

project (subspace)

enable_language(C)
enable_language(CXX)

include_directories(${TOP_SOURCE_DIR}/shared)

if (BUILD_GAME AND WIN32)
    # Disable compiler features that depend on the standard library
    add_definitions(/GS-) # no buffer security check
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}") # no runtime checks
    add_compile_definitions(_WIN32_WINNT=0x0501)
endif()

add_subdirectory(lib)

include (lib/cx/cmake/werror.cmake)

if (BUILD_GAME)
    add_subdirectory(game)
else()
    add_subdirectory(subspace)
endif()
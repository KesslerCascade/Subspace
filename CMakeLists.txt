cmake_minimum_required (VERSION 3.21)

set(TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

project (subspace)

enable_language(C)
enable_language(CXX)

include_directories(${TOP_SOURCE_DIR}/shared)

if (BUILD_CLIENT AND WIN32)
    # Disable compiler features that depend on the standard library
    add_definitions(/GS-) # no buffer security check
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}") # no runtime checks
    add_compile_definitions(_WIN32_WINNT=0x0501)
endif()

add_subdirectory(lib)

if (BUILD_CLIENT)
    add_subdirectory(client)
endif()
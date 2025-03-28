cmake_minimum_required (VERSION 3.21)

set(CX_TOP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Location of CX source files" FORCE)
set(CX_TOP_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Location of CX build files" FORCE)

project (cx C CXX)

if(PROJECT_IS_TOP_LEVEL)
  if(CMAKE_CONFIGURATION_TYPES)
     list(APPEND CMAKE_CONFIGURATION_TYPES Dev DevNoOpt)
  endif()

  set(CMAKE_C_FLAGS_DEV "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_EXE_LINKER_FLAGS_DEV "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
  set(CMAKE_SHARED_LINKER_FLAGS_DEV "${CMAKE_SHARD_LINKER_FLAGS_DEBUG}")

  set(CMAKE_C_FLAGS_DEVNOOPT "${CMAKE_C_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_DEVNOOPT "${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_EXE_LINKER_FLAGS_DEVNOOPT "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
  set(CMAKE_SHARED_LINKER_FLAGS_DEVNOOPT "${CMAKE_SHARD_LINKER_FLAGS_DEBUG}")

  set(CX_LUA_DEFAULT ON)
  set(CX_STANDALONE 1)
else()
  set(CX_LUA_DEFAULT OFF)
endif()

option(CX_LUA "Build Lua scripting engine and cxlua library" ${CX_LUA_DEFAULT})

include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/sidl.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/organize_targets.cmake)

string(REGEX MATCH "Clang" CX_COMPILER_IS_CLANG "${CMAKE_C_COMPILER_ID}")
string(REGEX MATCH "GNU" CX_COMPILER_IS_GNU "${CMAKE_C_COMPILER_ID}")
string(REGEX MATCH "MSVC" CX_COMPILER_IS_MSVC "${CMAKE_C_COMPILER_ID}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CX_PLATFORM_IS_WINDOWS 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CX_PLATFORM_IS_WASM 1)
else()
    set(CX_PLATFORM_IS_UNIX 1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -sMALLOC=emmalloc -sINITIAL_MEMORY=2047MB")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)            # for MSVC
set(CMAKE_CODELITE_USE_TARGETS ON)

add_subdirectory(3rdparty)
add_subdirectory(cx)
add_subdirectory(cxobjgen)
add_subdirectory(misc)

if(CX_LUA)
  add_subdirectory(cxlua)
endif()

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(tests)
endif()

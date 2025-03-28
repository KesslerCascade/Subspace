add_library(lua STATIC
    lapi.c
    lauxlib.c
    lbaselib.c
    lcode.c
    lcorolib.c
    lctype.c
    ldblib.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    loadlib.c
    lobject.c
    lopcodes.c
    loslib.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lundump.c
    lutf8lib.c
    lvm.c
    lzio.c)

target_link_libraries(lua cx)
target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(lua ${MATH_LIBRARY})
endif()

if(CX_PLATFORM_IS_WINDOWS)
    target_compile_definitions(lua PRIVATE LUA_USE_WINDOWS)
elseif(CX_PLATFORM_IS_UNIX)
    target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
endif()
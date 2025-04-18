if(WIN32)
    add_executable(subspacedbg
        win32/subspacedbg.c)
    target_compile_definitions(subspacedbg PRIVATE _UNICODE UNICODE)

    set(CMAKE_WIN32_EXECUTABLE true)
endif()

set(SUBSPACE_SIDL
    lua/task/luacall.sidl
    lua/task/luaexec.sidl
    lua/task/luatask.sidl)

add_sidl(${SUBSPACE_SIDL})

add_executable(subspace
    control/controlserver.c
    lua/log/lualog.c
    lua/lua/lualua.c
    lua/task/luacall.c
    lua/task/luaexec.c
    lua/task/luatask.c
    lua/subspace/luasubspace.c
    lua/luamodules.c
    lua/luathread.c
    ui/uithread.c
    win32/win32.c
    filesys.c
    log.c
    subspace.c
    version.c
    ../lib/cx/cx/cx.natvis
    ${SHARED_SOURCE_DIR}/control.c
    ${SHARED_SOURCE_DIR}/net.c
    ${SUBSPACE_SIDL})
target_link_libraries(subspace cxlua iupluacontrols sqlite)
target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_compile_definitions(subspace PRIVATE SUBSYSTEM_WINDOWS _UNICODE UNICODE)
    target_compile_options(subspace PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspace PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()
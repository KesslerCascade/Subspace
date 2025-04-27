if(WIN32)
    add_executable(subspacedbg
        win32/subspacedbg.c
        )
    target_compile_definitions(subspacedbg PRIVATE _UNICODE UNICODE)

    set(CMAKE_WIN32_EXECUTABLE true)
endif()

set(EXTRA_SIDL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBSPACE_SIDL
    control/controlclient.sidl
    control/controltask.sidl
    control/controlserver.sidl
    control/cmds/cmdgamestart.sidl
    control/cmds/cmdlaunchfail.sidl
    control/cmds/cmdlog.sidl
    gamemgr/gameinst.sidl
    gamemgr/gamemgr.sidl
    lang/langdb.sidl
    ui/subspaceui.sidl

add_sidl(${SUBSPACE_SIDL})

add_executable(subspace
    control/controlclient.c
    control/controlserver.c
    control/controltask.c
    control/cmds/cmdgamestart.c
    control/cmds/cmdlaunchfail.c
    control/cmds/cmdlog.c
    gamemgr/gameinst.c
    gamemgr/gamemgr.c
    lang/lang.c
    lang/langdb.c
    ui/subspaceui.c
    win32/process.c
    win32/win32.c
    filesys.c
    log.c
    subspace.c
    version.c
    ../lib/cx/cx/cx.natvis
    ${SHARED_SOURCE_DIR}/control.c
    ${SHARED_SOURCE_DIR}/net.c
    ${SUBSPACE_SIDL})
target_link_libraries(subspace cx iupcontrols iupplot sqlite)
target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_compile_definitions(subspace PRIVATE SUBSYSTEM_WINDOWS _UNICODE UNICODE)
    target_compile_options(subspace PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspace PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()
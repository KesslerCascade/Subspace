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
    db/database.sidl
    feature/feature.sidl
    feature/featureregistry.sidl
    feature/frameadv/frameadv.sidl
    feature/infoblock/infoblock.sidl
    feature/numerichull/numerichull.sidl
    feature/timewarp/timewarp.sidl
    gamemgr/gameinst.sidl
    gamemgr/gamemgr.sidl
    lang/langdb.sidl
    ui/mainwin.sidl
    ui/optionswin.sidl
    ui/subspaceui.sidl
    ui/image/image.sidl
    ui/image/imageload.sidl
    ui/image/imageresample.sidl
    ui/page/optionspage.sidl
    ui/page/general/optionsgeneral.sidl
    ui/panel/panel.sidl
    ui/panel/gameinfo/gameinfopanel.sidl
    ui/panel/welcome/welcomepanel.sidl
    ui/util/iupattrdispatch.sidl
    ui/util/iuprefreshdispatch.sidl
    ui/util/iupsetimagedispatch.sidl
    ui/util/uiupdatedispatch.sidl
    )

add_sidl(${SUBSPACE_SIDL})

add_executable(subspace
    control/controlclient.c
    control/controlfields.c
    control/controlserver.c
    control/controltask.c
    control/cmds/cmdfeaturestate.c
    control/cmds/cmdgamestart.c
    control/cmds/cmdgamestate.c
    control/cmds/cmdlaunchfail.c
    control/cmds/cmdloading.c
    control/cmds/cmdlog.c
    control/cmds/cmds.c
    control/cmds/cmdvalidate.c
    db/database.c
    feature/feature.c
    feature/features.c
    feature/featureregistry.c
    feature/frameadv/frameadv.c
    feature/infoblock/infoblock.c
    feature/numerichull/numerichull.c
    feature/timewarp/timewarp.c
    gamemgr/gameinst.c
    gamemgr/gamemgr.c
    lang/lang.c
    lang/langdb.c
    ui/mainwin.c
    ui/mainwinlayout.c
    ui/optionswin.c
    ui/subspaceui.c
    ui/image/image.c
    ui/image/imageload.c
    ui/image/imageresample.c
    ui/image/resample.c
    ui/image/drivers/imagepng.c
    ui/image/drivers/imagesvg.c
    ui/page/optionspage.c
    ui/page/general/optionsgeneral.c
    ui/panel/panel.c
    ui/panel/gameinfo/gameinfopanel.c
    ui/panel/welcome/welcomepanel.c
    ui/util/iupattrdispatch.c
    ui/util/iuploadimage.c
    ui/util/iuprefreshdispatch.c
    ui/util/iupsetimagedispatch.c
    ui/util/uiupdatedispatch.c
    win32/process.c
    win32/subspace.manifest
    win32/win32.c
    filesys.c
    log.c
    subspace.c
    version.c
    ../lib/cx/cx/cx.natvis
    ${SHARED_SOURCE_DIR}/control.c
    ${SHARED_SOURCE_DIR}/net.c
    ${SUBSPACE_SIDL})
target_link_libraries(subspace cx iupcontrols iupplot sqlite nanosvg nanosvgrast)
target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_compile_definitions(subspace PRIVATE SUBSYSTEM_WINDOWS _UNICODE UNICODE)
    target_compile_options(subspace PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspace PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()
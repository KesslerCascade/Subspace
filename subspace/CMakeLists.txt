cmake_minimum_required (VERSION 3.21)

include (../lib/cx/cmake/organize_targets.cmake)

set(TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SHARED_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../shared)

project (subspace)

enable_language(C)
enable_language(CXX)

include_directories(${SHARED_SOURCE_DIR})

add_subdirectory(../lib lib)
add_subdirectory(../tools tools)

include (../lib/cx/cmake/werror.cmake)

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
    feature/infoblock/infoblockpage.sidl
    feature/numerichull/numerichull.sidl
    feature/timewarp/timewarp.sidl
    feature/timewarp/timewarppage.sidl
    feature/tweaks/tweaks.sidl
    feature/tweaks/tweakspage.sidl
    gamemgr/gameinst.sidl
    gamemgr/gamemgr.sidl
    kbmgr/kbmgr.sidl
    kbmgr/keybind.sidl
    lang/langdb.sidl
    ui/mainwin.sidl
    ui/settingswin.sidl
    ui/subspaceui.sidl
    ui/image/image.sidl
    ui/image/imageload.sidl
    ui/image/imageresample.sidl
    ui/page/settingspage.sidl
    ui/page/features/featurespage.sidl
    ui/page/keybinds/keybindspage.sidl
    ui/page/setup/setuppage.sidl
    ui/panel/panel.sidl
    ui/panel/gameinfo/gameinfopanel.sidl
    ui/panel/notableevent/notableeventpanel.sidl
    ui/panel/scrapgraph/scrapgraphpanel.sidl
    ui/panel/sectordetail/sectordetailpanel.sidl
    ui/panel/welcome/welcomepanel.sidl
    ui/util/iupattrdispatch.sidl
    ui/util/iuprefreshdispatch.sidl
    ui/util/iupsetimagedispatch.sidl
    ui/util/uiupdatedispatch.sidl
    )

add_sidl(${SUBSPACE_SIDL})

add_executable(subspace
    control/cmds.c
    control/controlclient.c
    control/controlfields.c
    control/controlserver.c
    control/controltask.c
    control/cmds/cmdfeaturestate.c
    control/cmds/cmdgameready.c
    control/cmds/cmdgamestart.c
    control/cmds/cmdgamestate.c
    control/cmds/cmdlaunchfail.c
    control/cmds/cmdloading.c
    control/cmds/cmdlog.c
    control/cmds/cmdvalidate.c
    db/database.c
    feature/feature.c
    feature/features.c
    feature/featureregistry.c
    feature/frameadv/frameadv.c
    feature/infoblock/infoblock.c
    feature/infoblock/infoblockpage.c
    feature/numerichull/numerichull.c
    feature/timewarp/timewarp.c
    feature/timewarp/timewarppage.c
    feature/tweaks/tweaks.c
    feature/tweaks/tweakspage.c
    gamemgr/gameinst.c
    gamemgr/gamemgr.c
    kbmgr/kbmgr.c
    kbmgr/keybind.c
    kbmgr/keymap.c
    lang/lang.c
    lang/langdb.c
    ui/about.c
    ui/mainwin.c
    ui/mainwinlayout.c
    ui/mainwinmenu.c
    ui/settingswin.c
    ui/subspaceui.c
    ui/image/image.c
    ui/image/imageload.c
    ui/image/imageresample.c
    ui/image/resample.c
    ui/image/drivers/imagepng.c
    ui/image/drivers/imagesvg.c
    ui/page/settingspage.c
    ui/page/features/featurespage.c
    ui/page/keybinds/keybindspage.c
    ui/page/setup/setuppage.c
    ui/panel/panel.c
    ui/panel/gameinfo/gameinfopanel.c
    ui/panel/notableevent/notableeventpanel.c
    ui/panel/scrapgraph/scrapgraphpanel.c
    ui/panel/sectordetail/sectordetailpanel.c
    ui/panel/welcome/welcomepanel.c
    ui/util/iupattrdispatch.c
    ui/util/iuploadimage.c
    ui/util/iuprefreshdispatch.c
    ui/util/iupsetimagedispatch.c
    ui/util/settip.c
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
target_link_libraries(subspace cx iupcontrols iupplot sqlite nanosvg nanosvgrast libpkg)
target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_compile_definitions(subspace PRIVATE SUBSYSTEM_WINDOWS _UNICODE UNICODE)
    target_compile_options(subspace PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspace PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()

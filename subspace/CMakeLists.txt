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
    set(CMAKE_WIN32_EXECUTABLE true)
endif()

set(EXTRA_CXAUTOGEN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(SUBSPACE_AUTOGEN
    control/controlclient.cxh
    control/controltask.cxh
    control/controlserver.cxh
    db/database.cxh
    db/dbstmt.cxh
    feature/feature.cxh
    feature/featureregistry.cxh
    feature/frameadv/frameadv.cxh
    feature/infoblock/infoblock.cxh
    feature/infoblock/infoblockpage.cxh
    feature/numerichull/numerichull.cxh
    feature/practicemode/practicemode.cxh
    feature/runtracker/runtracker.cxh
    feature/runtracker/runtrackerpage.cxh
    feature/savemanager/savemanager.cxh
    feature/savemanager/savemanagerpage.cxh
    feature/screenshot/screenshot.cxh
    feature/screenshot/screenshotpage.cxh
    feature/timewarp/timewarp.cxh
    feature/timewarp/timewarppage.cxh
    feature/tweaks/tweaks.cxh
    feature/tweaks/tweakspage.cxh
    gamedata/gamedata.cxh
    gamedata/loadtext.cxh
    gamemgr/gameinst.cxh
    gamemgr/gamemgr.cxh
    kbmgr/kbmgr.cxh
    kbmgr/keybind.cxh
    lang/langdb.cxh
    run/hulltracker.cxh
    run/logent.cxh
    run/logrelay.cxh
    run/runinfo.cxh
    run/scraptracker.cxh
    run/sectorinfo.cxh
    ui/subspaceui.cxh
    ui/image/image.cxh
    ui/image/imageload.cxh
    ui/image/imageresample.cxh
    ui/page/settingspage.cxh
    ui/page/features/featurespage.cxh
    ui/page/keybinds/keybindspage.cxh
    ui/page/setup/setuppage.cxh
    ui/panel/panel.cxh
    ui/panel/gameinfo/gameinfopanel.cxh
    ui/panel/gameinfo/statlayoutinfo.cxh
    ui/panel/hullgraph/hullgraphpanel.cxh
    ui/panel/runsummary/runsummarypanel.cxh
    ui/panel/scrapgraph/scrapgraphpanel.cxh
    ui/panel/sectordetail/sectordetailpanel.cxh
    ui/panel/welcome/welcomepanel.cxh
    ui/util/iupattrdispatch.cxh
    ui/util/iuprefreshdispatch.cxh
    ui/util/iupsetimagedispatch.cxh
    ui/util/uiupdatedispatch.cxh
    ui/win/mainwin.cxh
    ui/win/runhistory.cxh
    ui/win/settingswin.cxh
    )

add_cxautogen(${SUBSPACE_AUTOGEN})

add_executable(subspace
    control/cmds.c
    control/controlclient.c
    control/controlfields.c
    control/controlserver.c
    control/controltask.c
    control/cmds/cmdautosave.c
    control/cmds/cmdbeacon.c
    control/cmds/cmdfeaturestate.c
    control/cmds/cmdgameready.c
    control/cmds/cmdgamestart.c
    control/cmds/cmdgamestate.c
    control/cmds/cmdlaunchfail.c
    control/cmds/cmdloadgame.c
    control/cmds/cmdloading.c
    control/cmds/cmdlog.c
    control/cmds/cmdnewgame.c
    control/cmds/cmdrunlog.c
    control/cmds/cmdsector.c
    control/cmds/cmdstats.c
    control/cmds/cmdvalidate.c
    db/database.c
    db/dbstmt.c
    db/schema.c
    feature/feature.c
    feature/features.c
    feature/featureregistry.c
    feature/frameadv/frameadv.c
    feature/infoblock/infoblock.c
    feature/infoblock/infoblockpage.c
    feature/numerichull/numerichull.c
    feature/practicemode/practicemode.c
    feature/runtracker/runtracker.c
    feature/runtracker/runtrackerpage.c
    feature/savemanager/savemanager.c
    feature/savemanager/savemanagerpage.c
    feature/screenshot/screenshot.c
    feature/screenshot/screenshotpage.c
    feature/timewarp/timewarp.c
    feature/timewarp/timewarppage.c
    feature/tweaks/tweaks.c
    feature/tweaks/tweakspage.c
    gamedata/gamedata.c
    gamedata/loadtext.c
    gamemgr/gameinst.c
    gamemgr/gamemgr.c
    kbmgr/kbmgr.c
    kbmgr/keybind.c
    kbmgr/keymap.c
    lang/lang.c
    lang/langdb.c
    run/hulltracker.c
    run/logent.c
    run/logrelay.c
    run/rundb.c
    run/runinfo.c
    run/runinforundir.c
    run/scraptracker.c
    run/sectorinfo.c
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
    ui/panel/gameinfo/statlayoutinfo.c
    ui/panel/hullgraph/hullgraphpanel.c
    ui/panel/runsummary/runsummarypanel.c
    ui/panel/scrapgraph/scrapgraphpanel.c
    ui/panel/sectordetail/sectordetailpanel.c
    ui/panel/welcome/welcomepanel.c
    ui/util/iupattrdispatch.c
    ui/util/iuploadimage.c
    ui/util/iuprefreshdispatch.c
    ui/util/iupsetimagedispatch.c
    ui/util/settip.c
    ui/util/uiupdatedispatch.c
    ui/win/about.c
    ui/win/mainwin.c
    ui/win/mainwinlayout.c
    ui/win/mainwinmenu.c
    ui/win/quitconfirm.c
    ui/win/runhistory.c
    ui/win/settingswin.c
    util/pathfilter.c
    util/pathsubst.c
    win32/process.c
    win32/subspace.manifest
    win32/subspace.rc
    win32/win32.c
    filesys.c
    log.c
    misc.c
    subspace.c
    version.c
    ../lib/cx/cx/cx.natvis
    ${SHARED_SOURCE_DIR}/control.c
    ${SHARED_SOURCE_DIR}/logentspec.c
    ${SHARED_SOURCE_DIR}/net.c
    ${SUBSPACE_AUTOGEN})
target_compile_definitions(subspace PRIVATE $<$<CONFIG:Release>:SUBSPACE_RELEASE>)
target_link_libraries(subspace cx iupcontrols iupplot sqlite nanosvg nanosvgrast png_static libjpeg libpkg cxml)
target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
    target_include_directories(subspace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_compile_definitions(subspace PRIVATE SUBSYSTEM_WINDOWS _UNICODE UNICODE)
    target_compile_options(subspace PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspace PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()

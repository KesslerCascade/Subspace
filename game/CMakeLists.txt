cmake_minimum_required (VERSION 3.21)

include (../lib/cx/cmake/organize_targets.cmake)

set(TOP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SHARED_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../shared)

project (subspacegame)

enable_language(C)
enable_language(CXX)

include_directories(${SHARED_SOURCE_DIR})

if (WIN32)
    # Disable compiler features that depend on the standard library
    add_compile_options(/GS-) # no buffer security check
    STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}") # no runtime checks
endif()

add_subdirectory(../lib lib)

include (../lib/cx/cmake/werror.cmake)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(GAME_OSDEP
        win32/codegen/asm.c
        win32/codegen/codegen.c
        win32/codegen/hookgen.c
        win32/codegen/wrapgen.c
        win32/ftl/graphics/ftl_csurface.c
        win32/ftl/graphics/ftl_freetype.c
        win32/ftl/ftl_capp.c
        win32/ftl/ftl_cevent.c
        win32/ftl/ftl_cfps.c
        win32/ftl/ftl_combatcontrol.c
        win32/ftl/ftl_commandgui.c
        win32/ftl/ftl_completeship.c
        win32/ftl/ftl_filehelper.c
        win32/ftl/ftl_ftlbutton.c
        win32/ftl/ftl_globals.c
        win32/ftl/ftl_mainmenu.c
        win32/ftl/ftl_misc.c
        win32/ftl/ftl_mousecontrol.c
        win32/ftl/ftl_osdep.c
        win32/ftl/ftl_resourcecontrol.c
        win32/ftl/ftl_scorekeeper.c
        win32/ftl/ftl_shipmanager.c
        win32/ftl/ftl_ship.c
        win32/ftl/ftl_shipmanager.c
        win32/ftl/ftl_shipstatus.c
        win32/ftl/ftl_starmap.c
        win32/ftl/ftl_startup.c
        win32/ftl/ftl_stdlib.c
        win32/ftl/ftl_textlibrary.c
        win32/ftl/ftl_tutorialmanager.c
        win32/ftl/ftl_worldmanager.c
        win32/hook/analysis.c
        win32/hook/export.c
        win32/hook/hook.c
        win32/hook/symbol.c
        win32/loader/loader.c
        win32/loader/rtld.c
        win32/loader/rtldoverrides.c
        win32/patch/patch_osdep.c
        win32/entry.c
        win32/kernel32_overrides.c
        win32/subspacegame.rc
        win32/osdep.c
        win32/thread.c
        win32/winerror.c
        win32/winmain.c)
    set(CMAKE_WIN32_EXECUTABLE true)
endif()

add_executable(subspacegame
    control/controlclient.c
    control/controlconnect.c
    control/messagequeue.c
    control/cmds.c
    control/cmds/cmdbindkey.c
    control/cmds/cmdcleartostart.c
    control/cmds/cmdenablefeature.c
    control/cmds/cmdfeaturesettings.c
    feature/feature.c
    feature/frameadv.c
    feature/infoblock.c
    feature/numerichull.c
    feature/timewarp.c
    feature/tweaks.c
    ftl/graphics/colors.c
    ftl/ftl.c
    ftl/struct.c
    hook/addrsort.c
    hook/disasmtrace.c
    hook/module.c
    hook/string.c
    hook/symbol.c
    hook/capp/capp_onexecute.c
    hook/capp/capp_onkeydown.c
    hook/capp/capp_onloop.c
    hook/capp/capp_onrender.c
    hook/cfps/cfps_onloop.c
    hook/cfps/cfps_targetframetime.c
    hook/combatcontrol/combatcontrol_rendertarget.c
    hook/commandgui/commandgui_keydown.c
    hook/commandgui/commandgui_onloop.c
    hook/commandgui/commandgui_renderstatic.c
    hook/filehelper/filehelper_getuserfolder.c
    hook/ftlbutton/ftlbutton_onrender.c
    hook/mainmenu/mainmenu_open.c
    hook/mousecontrol/mousecontrol_onrender.c
    hook/resourcecontrol/resourcecontrol_renderloadingbar.c
    hook/shipstatus/shipstatus_renderhealth.c
    hook/shipstatus/shipstatus_onrender.c
    hook/textlibrary/textlibrary_gettext.c
    input/keybinds.c
    input/keyboard.c
    log/log.c
    patch/patch.c
    patch/required.c
    subspacegame.c
    version.c
    ${SHARED_SOURCE_DIR}/control.c
    ${SHARED_SOURCE_DIR}/net.c
    ${GAME_OSDEP})
target_include_directories(subspacegame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(subspacegame PRIVATE SUBSPACE_GAME)
target_link_libraries(subspacegame disasm minicrt)
if (WIN32)
    target_include_directories(subspacegame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_link_libraries(subspacegame ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib kernel32 ws2_32)
    target_compile_options(subspacegame PRIVATE "$<$<CONFIG:Release>:/Zi>")
    target_link_options(subspacegame PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
endif()

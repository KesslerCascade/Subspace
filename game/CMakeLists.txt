if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(GAME_OSDEP
        win32/codegen/asm.c
        win32/codegen/codegen.c
        win32/codegen/hookgen.c
        win32/codegen/wrapgen.c
        win32/ftl/graphics/ftl_freetype.c
        win32/ftl/ftl_capp.c
        win32/ftl/ftl_cevent.c
        win32/ftl/ftl_cfps.c
        win32/ftl/ftl_combatcontrol.c
        win32/ftl/ftl_commandgui.c
        win32/ftl/ftl_completeship.c
        win32/ftl/ftl_globals.c
        win32/ftl/ftl_misc.c
        win32/ftl/ftl_mousecontrol.c
        win32/ftl/ftl_osdep.c
        win32/ftl/ftl_shipmanager.c
        win32/ftl/ftl_ship.c
        win32/ftl/ftl_shipmanager.c
        win32/ftl/ftl_shipstatus.c
        win32/ftl/ftl_starmap.c
        win32/ftl/ftl_startup.c
        win32/ftl/ftl_stdlib.c
        win32/ftl/ftl_textlibrary.c
        win32/ftl/ftl_worldmanager.c
        win32/hook/analysis.c
        win32/hook/disasmtrace.c
        win32/hook/export.c
        win32/hook/hook.c
        win32/hook/string.c
        win32/hook/symbol.c
        win32/loader/loader.c
        win32/loader/rtld.c
        win32/loader/rtldoverrides.c
        win32/patch/osdep/patch_main.c
        win32/patch/seq/seq_osdep.c
        win32/entry.c
        win32/kernel32_overrides.c
        win32/winmain.c)
    set(CMAKE_WIN32_EXECUTABLE true)
endif()

add_executable(subspacegame
    ftl/ftl.c
    ftl/struct.c
    hook/module.c
    hook/symbol.c
    hook/capp/capp_onexecute.c
    hook/capp/capp_onkeydown.c
    hook/capp/capp_onloop.c
    hook/cfps/cfps_onloop.c
    hook/combatcontrol/combatcontrol_rendertarget.c
    hook/commandgui/commandgui_keydown.c
    hook/mousecontrol/mousecontrol_onrender.c
    hook/shipstatus/shipstatus_renderhealth.c
    hook/textlibrary/textlibrary_gettext.c
    patch/patch.c
    patch/capp/patch_capp_onexecute.c
    patch/capp/patch_capp_onkeydown.c
    patch/capp/patch_capp_onloop.c
    patch/cfps/patch_cfps_onloop.c
    patch/cfps/patch_cfps_targetticktime.c
    patch/combatcontrol/patch_combatcontrol_rendertarget.c
    patch/commandgui/patch_commandgui_keydown.c
    patch/mousecontrol/patch_mousecontrol_onrender.c
    patch/seq/seq_numerichull.c
    patch/seq/seq_required.c
    patch/seq/seq_timewarp.c
    patch/ship/validate_ship.c
    patch/shipstatus/patch_shipstatus_renderhealth.c
    patch/shipstatus/validate_shipstatus.c
    patch/textlibrary/patch_textlibrary_gettext.c
    timewarp/timewarp.c
    subspacegame.c
    ${GAME_OSDEP})
target_include_directories(subspacegame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(subspacegame disasm minicrt)
if (WIN32)
    target_include_directories(subspacegame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_link_libraries(subspacegame ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
endif()
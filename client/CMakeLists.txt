if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(CLIENT_OSDEP
        win32/codegen/asm.c
        win32/codegen/codegen.c
        win32/codegen/hookgen.c
        win32/codegen/wrapgen.c
        win32/ftl/graphics/functions_freetype.c
        win32/ftl/functions_capp.c
        win32/ftl/functions_cevent.c
        win32/ftl/functions_cfps.c
        win32/ftl/functions_commandgui.c
        win32/ftl/functions_completeship.c
        win32/ftl/functions_mem.c
        win32/ftl/functions_misc.c
        win32/ftl/functions_mousecontrol.c
        win32/ftl/functions_osdep.c
        win32/ftl/functions_shipmanager.c
        win32/ftl/functions_ship.c
        win32/ftl/functions_shipmanager.c
        win32/ftl/functions_shipstatus.c
        win32/ftl/functions_starmap.c
        win32/ftl/functions_startup.c
        win32/ftl/functions_textlibrary.c
        win32/ftl/functions_worldmanager.c
        win32/ftl/globals.c
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

add_executable(subspace-client
    capp/onexecute.c
    capp/onkeydown.c
    capp/onloop.c
    commandgui/keydown.c
    ftl/ftl.c
    ftl/struct.c
    hook/module.c
    hook/symbol.c
    mousecontrol/onrender.c
    patch/patch.c
    patch/capp/patch_capp_onexecute.c
    patch/capp/patch_capp_onkeydown.c
    patch/capp/patch_capp_onloop.c
    patch/commandgui/patch_commandgui_keydown.c
    patch/mousecontrol/patch_mousecontrol_onrender.c
    patch/seq/seq_numerichull.c
    patch/seq/seq_required.c
    patch/ship/validate_ship.c
    patch/shipstatus/patch_shipstatus_renderhealth.c
    patch/shipstatus/validate_shipstatus.c
    patch/textlibrary/patch_textlibrary_gettext.c
    shipstatus/renderhealth.c
    textlibrary/gettext.c
    subspaceclient.c
    ${CLIENT_OSDEP})
target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(subspace-client disasm minicrt)
if (WIN32)
    target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_link_libraries(subspace-client ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
endif()
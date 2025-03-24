if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(CLIENT_OSDEP
        win32/codegen/asm.c
        win32/codegen/codegen.c
        win32/codegen/hookgen.c
        win32/codegen/wrapgen.c
        win32/ftl/functions_startup.c
        win32/hook/analysis.c
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
    ftl/ftl.c
    hook/module.c
    patch/patch.c
    patch/seq/seq_startup.c
    subspaceclient.c
    ${CLIENT_OSDEP})
target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(subspace-client disasm minicrt)
if (WIN32)
    target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win32)
    target_link_libraries(subspace-client ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
endif()
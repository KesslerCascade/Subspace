if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(CLIENT_OSDEP
        codegen/win32/asm.c
        codegen/win32/codegen.c
        codegen/win32/hookgen.c
        codegen/win32/wrapgen.c
        hook/win32/export.c
        hook/win32/hook.c
        hook/win32/string.c
        hook/win32/symbol.c
        loader/win32/loader.c
        loader/win32/rtld.c
        loader/win32/rtldoverrides.c
        win32/entry.c)
    set(CMAKE_WIN32_EXECUTABLE true)
endif()

add_executable(subspace-client
    hook/module.c
    patch/patch.c
    subspaceclient.c
    ${CLIENT_OSDEP})
target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(subspace-client disasm minicrt)
if (WIN32)
    target_link_libraries(subspace-client ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
endif()
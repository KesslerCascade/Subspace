if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")
    set(CLIENT_OSDEP
        loader/win32/loader.c
        loader/win32/rtld.c
        loader/win32/rtldoverrides.c
        win32/entry.c)
    set(CMAKE_WIN32_EXECUTABLE true)
endif()

add_executable(subspace-client
    ${CLIENT_OSDEP})
target_include_directories(subspace-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(subspace-client disasm minicrt)
if (WIN32)
    target_link_libraries(subspace-client ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
endif()
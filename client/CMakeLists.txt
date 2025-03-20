SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB /ENTRY:entry")

add_executable(ftlsubspace WIN32
    loader/rtld.c
    loader/rtldoverrides.c
    main.c)
target_link_libraries(ftlsubspace minicrt ${CMAKE_CURRENT_SOURCE_DIR}/tlsarray.lib)
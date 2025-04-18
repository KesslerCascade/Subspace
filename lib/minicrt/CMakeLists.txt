add_library(minicrt STATIC
    hashtbl.c
    lazyinit.c
    lock.c
    mem.c
    minicrt.c
    sbuflite.c
    smalloc.c
    string.c
    xprintf.c)
target_include_directories(minicrt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(minicrt STATIC
    lazyinit.c
    lock.c
    mem.c
    minicrt.c
    smalloc.c
    string.c)
target_include_directories(minicrt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(minicrt STATIC
    mem.c
    minicrt.c
    smalloc.c
    string.c
    xprintf.c)
target_include_directories(minicrt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(minicrt cx)
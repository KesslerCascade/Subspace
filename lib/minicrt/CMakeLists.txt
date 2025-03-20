add_library(minicrt STATIC
    mem.c
    minicrt.c
    string.c)
target_include_directories(minicrt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
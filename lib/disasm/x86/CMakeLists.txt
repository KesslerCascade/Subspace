add_library(disasm STATIC
    asmserv.c
    assembl.c
    disasm.c)
target_include_directories(disasm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
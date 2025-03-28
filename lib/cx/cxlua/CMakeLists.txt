add_library(cxlua STATIC
    cxlua.c)

target_link_libraries(cxlua cx lua)
target_include_directories(cxlua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

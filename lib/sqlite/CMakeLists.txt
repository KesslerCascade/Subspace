add_library(sqlite STATIC
    sqlite3.c)
target_link_libraries(sqlite PUBLIC cx)
target_include_directories(sqlite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
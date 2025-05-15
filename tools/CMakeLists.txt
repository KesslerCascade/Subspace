add_executable(mkdat
    mkdat.c)

target_link_libraries(mkdat cx libpkg)

organize_targets(CLEAR_FOLDER PREPEND tools)
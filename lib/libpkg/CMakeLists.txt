set(EXTRA_SIDL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBPKG_SIDL
    pkgfile/pkgfile.sidl
    pkgfs/pkgfs.sidl
    pkgfs/pkgfsfile.sidl
)

add_sidl(${LIBPKG_SIDL})

add_library(libpkg STATIC
    pkgfile/pkgfile.c
    pkgfs/pkgfs.c
    pkgfs/pkgfsfile.c
    ${LIBPKG_SIDL}
)
target_link_libraries(libpkg PUBLIC cx zlib)
target_include_directories(libpkg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
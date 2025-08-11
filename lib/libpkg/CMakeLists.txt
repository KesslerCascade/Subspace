set(EXTRA_CXAUTOGEN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBPKG_AUTOGEN
    pkgfile/pkgfile.cxh
    pkgfs/pkgfs.cxh
    pkgfs/pkgfsfile.cxh
)

add_cxautogen(${LIBPKG_AUTOGEN})

add_library(libpkg STATIC
    pkgfile/pkgfile.c
    pkgfs/pkgfs.c
    pkgfs/pkgfsfile.c
    ${LIBPKG_AUTOGEN}
)
target_link_libraries(libpkg PUBLIC cx zlib)
target_include_directories(libpkg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
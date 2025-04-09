set(SKIP_INSTALL_ALL 1)

if (BUILD_GAME)
    add_subdirectory(disasm)
    add_subdirectory(minicrt)
else()
    set(as_subproject ZLIB)

    set(CX_LUA ON CACHE INTERNAL "Build Lua scripting engine and cxlua library" FORCE)
    add_subdirectory(cx)

    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries" FORCE)
    set(ZLIB_BUILD_EXAMPLES OFF CACHE INTERNAL "Enable Zlib Examples"  FORCE)
    add_subdirectory(zlib)
    target_link_libraries(zlib cx)

    set(PNG_SHARED OFF CACHE INTERNAL "Build shared lib" FORCE)
    set(PNG_STATIC ON CACHE INTERNAL "Build static lib" FORCE)
    set(PNG_TOOLS OFF CACHE INTERNAL "Build the libpng tools" FORCE)
    set(PNG_TESTS OFF CACHE INTERNAL "Build libpng tests" FORCE)
    add_subdirectory(libpng)
    target_link_libraries(png_static PUBLIC zlib cx)
    add_library(png ALIAS png_static)

    set(BUILD_SHARED_LIBS   OFF CACHE INTERNAL "Build shared libraries" FORCE)
    set(FT_DISABLE_BZIP2    ON  CACHE INTERNAL "Disable support of bzip2 compressed fonts." FORCE)
    set(FT_DISABLE_HARFBUZZ ON  CACHE INTERNAL "Disable HarfBuzz (used for improving auto-hinting of OpenType fonts)." FORCE)
    set(FT_DISABLE_BROTLI   ON  CACHE INTERNAL "Disable support of compressed WOFF2 fonts." FORCE)
    add_subdirectory(freetype)
    target_link_libraries(freetype cx zlib png_static)

    add_subdirectory(iup)
    target_compile_definitions(iup PRIVATE XALLOC_REMAP_MALLOC)
    target_compile_definitions(iuplua PRIVATE XALLOC_REMAP_MALLOC)

    add_subdirectory(cd)
    target_compile_definitions(cd PRIVATE XALLOC_REMAP_MALLOC)

    add_subdirectory(sqlite)
endif()

organize_targets(CLEAR_FOLDER PREPEND lib)
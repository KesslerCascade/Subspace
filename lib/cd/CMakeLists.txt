set(CD_SOURCES
    src/cd.c
    src/cd_active.c
    src/cd_attributes.c
    src/cd_bitmap.c
    src/cd_image.c
    src/cd_primitives.c
    src/cd_text.c
    src/cd_util.c
    src/cd_vectortext.c
    src/rgb2map.c
    src/wd.c
    src/wdhdcpy.c
    src/drv/cdirgb.c
    src/gdiplus/cdwdbufp.cpp
    src/gdiplus/cdwgdiplus.c
    src/gdiplus/cdwinp.cpp
    src/gdiplus/cdwnativep.cpp
    src/sim/cd_truetype.c
    src/sim/cdfontex.c
    src/sim/sim_linepolyfill.c
    src/sim/sim_primitives.c
    src/sim/sim_text.c
    src/sim/sim.c
    src/win32/cdwdbuf.c
    src/win32/cdwdib.c
    src/win32/cdwimg.c
    src/win32/cdwin.c
    src/win32/cdwnative.c)

add_library(cd STATIC
    ${CD_SOURCES}
)
target_link_libraries(cd cx png freetype)
target_compile_definitions(cd PRIVATE XALLOC_REMAP_MALLOC COBJMACROS _UNICODE UNICODE)
target_include_directories(cd
    PRIVATE
        src src/sim
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CDLUA_SOURCES
    src/lua5/cdlua5.c
    src/lua5/cdvoid5.c
    src/lua5/cdlua5ctx.c
    src/lua5/cdlua5_active.c
    src/lua5/cdlua5_canvas.c)

add_library(cdlua STATIC
    ${CDLUA_SOURCES}
)
target_link_libraries(cdlua cd lua)
target_compile_definitions(cdlua PRIVATE XALLOC_REMAP_MALLOC COBJMACROS _UNICODE UNICODE)
target_include_directories(cdlua
    PRIVATE
        src/lua5
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

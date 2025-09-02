set(LIBEXIF_SOURCES
    libexif/exif-byte-order.c
    libexif/exif-content.c
    libexif/exif-data.c
    libexif/exif-entry.c
    libexif/exif-format.c
    libexif/exif-gps-ifd.c
    libexif/exif-ifd.c
    libexif/exif-loader.c
    libexif/exif-log.c
    libexif/exif-mem.c
    libexif/exif-mnote-data.c
    libexif/exif-tag.c
    libexif/exif-utils.c
    libexif/canon/exif-mnote-data-canon.c
    libexif/canon/mnote-canon-entry.c
    libexif/canon/mnote-canon-tag.c
    libexif/fuji/exif-mnote-data-fuji.c
    libexif/fuji/mnote-fuji-entry.c
    libexif/fuji/mnote-fuji-tag.c
    libexif/olympus/exif-mnote-data-olympus.c
    libexif/olympus/mnote-olympus-entry.c
    libexif/olympus/mnote-olympus-tag.c
    libexif/pentax/exif-mnote-data-pentax.c
    libexif/pentax/mnote-pentax-entry.c
    libexif/pentax/mnote-pentax-tag.c)
add_library(libexif STATIC
    ${LIBEXIF_SOURCES}
)

target_compile_definitions(libexif PRIVATE GETTEXT_PACKAGE="")
target_include_directories(libexif PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libexif cx)
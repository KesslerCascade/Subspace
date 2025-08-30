#include <cx/container.h>
#include <cx/format.h>
#include <png.h>
#include "feature/screenshot/screenshot.h"

static void png_write_cb(png_structp png, png_bytep data, size_t sz)
{
    VFSFile* f = (VFSFile*)png_get_io_ptr(png);

    size_t didwrite = 0;
    vfsWrite(f, data, sz, &didwrite);
}

static void png_flush_cb(png_structp png)
{
    return;
}

bool Screenshot_writePng(_In_ Screenshot* self, VFSFile* out, uint8* data, int w, int h,
                         uint8* exifdata, uint32 exifdsz, string xmpdata, int64 timestamp)
{
    bool ret        = false;
    png_text* ptext = NULL;
    int itext       = 0;
    string temp     = 0;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        goto out;

    png_infop pinfo = png_create_info_struct(png);
    if (!pinfo)
        goto out;

    if (setjmp(png_jmpbuf(png)))
        goto out;

    png_byte** row_pointers = xaAlloc(h * sizeof(void*));

    png_set_write_fn(png, out, png_write_cb, png_flush_cb);

    png_set_IHDR(png,
                 pinfo,
                 w,
                 h,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    // "Extensions to the PNG 1.2 Specification 1.5.0" says the EXIF header should be omitted.
    if (exifdsz > 6)
        png_set_eXIf_1(png, pinfo, exifdsz - 6, exifdata + 6);

    if (!strEmpty(xmpdata)) {
        xaResize(&ptext, sizeof(png_text) * (itext + 1));
        ptext[itext].compression = PNG_ITXT_COMPRESSION_zTXt;
        ptext[itext].key         = xa_strdup("XML:com.adobe.xmp");
        ptext[itext].text        = xa_strdup(strC(xmpdata));
        ptext[itext].lang        = "";
        ptext[itext].lang_key    = "";
        itext++;
    }

    // Creation time, mostly for windows explorer
    TimeParts tp;
    timeDecompose(&tp, toLocalTime(timestamp));
    strFormat(&temp,
              _S"${0int(4)}:${0int(2)}:${0int(2)} ${0int(2)}:${0int(2)}:${0int(2)}",
              stvar(int32, tp.year),
              stvar(int32, tp.month),
              stvar(int32, tp.day),
              stvar(int32, tp.hour),
              stvar(int32, tp.minute),
              stvar(int32, tp.second));
    xaResize(&ptext, sizeof(png_text) * (itext + 1));
    ptext[itext].compression = PNG_TEXT_COMPRESSION_NONE;
    ptext[itext].itxt_length = 0;
    ptext[itext].key         = xa_strdup("Creation Time");
    ptext[itext].text        = xa_strdup(strC(temp));
    ptext[itext].text_length = strLen(temp);
    ptext[itext].lang        = "";
    ptext[itext].lang_key    = "";
    itext++;

    png_set_text(png, pinfo, ptext, itext);
    png_write_info(png, pinfo);

    for (int i = 0; i < h; i++) row_pointers[i] = &data[w * i * 3];
    png_write_rows(png, row_pointers, h);
    png_write_end(png, pinfo);

    ret = true;

out:
    xaFree(row_pointers);
    if (pinfo)
        png_destroy_write_struct(&png, &pinfo);
    else if (png)
        png_destroy_write_struct(&png, NULL);
    for (int i = 0; i < itext; i++) {
        xaFree(ptext[i].key);
        xaFree(ptext[i].text);
    }
    xaFree(ptext);
    strDestroy(&temp);
    return ret;
}

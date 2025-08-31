#include "feature/screenshot/screenshot.h"

#include <jerror.h>
#include <jpeglib.h>

#define JPEG_BUFFSZ (512 * 1024)

bool Screenshot_writeJpeg(_In_ Screenshot* self, VFSFile* out, uint8* data, int w, int h, uint8* exifdata, uint32 exifdsz, string xmpdata)
{
    bool ret      = false;
    uint8* outbuf = xaAlloc(JPEG_BUFFSZ);
    size_t outsz  = JPEG_BUFFSZ;
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr err;

    uint8** row_pointers = xaAlloc(h * sizeof(void*));

    info.err = jpeg_std_error(&err);
    jpeg_create_compress(&info);
    jpeg_mem_dest(&info, &outbuf, &outsz);
    info.image_width      = w;
    info.image_height     = h;
    info.input_components = 3;
    info.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&info);
    jpeg_set_quality(&info, ssdVal(int32, self->settings, _S"jpegquality", 95), TRUE);

    jpeg_start_compress(&info, TRUE);
    jpeg_write_marker(&info, 0xE1, exifdata, exifdsz);
    uint8* xmpbuf = xaAlloc(strLen(xmpdata) + 30);
    memcpy(xmpbuf, "http://ns.adobe.com/xap/1.0/\0", 29);
    strCopyOut(xmpdata, 0, xmpbuf + 29, strLen(xmpdata) + 1);
    jpeg_write_marker(&info, 0xE1, xmpbuf, strLen(xmpdata) + 30);
    xaFree(xmpbuf);

    for (int i = 0; i < h; i++) row_pointers[i] = &data[w * i * 3];
    jpeg_write_scanlines(&info, row_pointers, h);
    jpeg_finish_compress(&info);

    size_t didwrite;
    vfsWrite(out, outbuf, outsz, &didwrite);
    ret = (didwrite == outsz);

out:
    xaFree(row_pointers);
    xaFree(outbuf);

    return ret;
}

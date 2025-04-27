#include "ui/image/imagedrivers.h"
#include "ui/image/imageload.h"

#include <cx/utils/compare.h>
#include <png.h>

static void pngreadcb(png_structp png, png_bytep out, png_size_t count)
{
    ImageLoad* self = (ImageLoad*)png_get_io_ptr(png);
    if (!self) {
        png_error(png, "Internal PNG error");
        return;
    }

    if (self->srcfile) {
        size_t didread;
        if (!vfsRead(self->srcfile, out, count, &didread) || didread < count) {
            png_error(png, "Read error reading PNG file");
            return;
        }
    } else {
        if (self->srcbufsz < count) {
            png_error(png, "Insufficient data in PNG decode buffer");
            return;
        }

        memcpy(out, self->srcbufp, count);
        self->srcbufp += count;
        self->srcbufsz -= count;
    }
}

bool imageDriverPng(ImageLoad* self)
{
    bool ret = false;

    Image* img        = self->image;
    png_structp png   = NULL;
    png_infop pnginfo = NULL;
    png_infop pngend  = NULL;
    uint8** rows      = NULL;

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        goto finally;

    pnginfo = png_create_info_struct(png);
    if (!pnginfo)
        goto finally;

    pngend = png_create_info_struct(png);
    if (!pngend)
        goto finally;

    if (setjmp(png_jmpbuf(png))) {
        goto finally;
    }

    png_set_read_fn(png, self, pngreadcb);

    // read file header;
    png_read_info(png, pnginfo);
    png_set_expand(png);
    png_read_update_info(png, pnginfo);

    int depth = png_get_bit_depth(png, pnginfo);
    int ctype = png_get_color_type(png, pnginfo);
    if (depth != 8 || !(ctype == PNG_COLOR_TYPE_RGB_ALPHA || ctype == PNG_COLOR_TYPE_RGB)) {
        // Unsupported PNG type
        goto finally;
    }

    img->width  = png_get_image_width(png, pnginfo);
    img->height = png_get_image_height(png, pnginfo);
    img->bpp    = (ctype == PNG_COLOR_TYPE_RGB_ALPHA) ? 4 : 3;

    png_uint_32 dpix, dpiy;
    int restype;
    if (png_get_pHYs(png, pnginfo, &dpix, &dpiy, &restype)) {
        img->dpi = (dpix + dpiy) / 2;
        if (restype == PNG_RESOLUTION_METER)
            img->dpi = (int)((double)img->dpi / 39.3701 + 0.5);

        // default for some image editing tools is 72dpi
        // bring it up to the Windows default of 96dpi
        img->dpi = clamplow(img->dpi, 96);
    } else {
        img->dpi = 96;
    }

    img->pixels = xaAlloc(img->width * img->height * img->bpp);
    rows        = xaAlloc(sizeof(uint8*) * img->height);
    if (!(img->pixels && rows))
        goto finally;

    for (int i = 0; i < img->height; ++i) rows[i] = &img->pixels[i * img->width * img->bpp];

    png_read_image(png, rows);
    png_read_end(png, NULL);

    ret = true;

finally:
    if (rows)
        xaFree(rows);
    if (png)
        png_destroy_read_struct(&png, &pnginfo, &pngend);

    return ret;
}

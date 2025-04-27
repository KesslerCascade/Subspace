#include "ui/image/imagedrivers.h"
#include "ui/image/imageload.h"

#include <cx/utils/compare.h>
#include <nanosvg.h>
#include <nanosvgrast.h>

bool imageDriverSvg(ImageLoad* self)
{
    bool ret = false;

    Image* img           = self->image;
    NSVGimage* svg       = NULL;
    NSVGrasterizer* rast = NULL;

    if (self->srcfile && !self->srcbuf) {
        self->srcbuf = xaAlloc((size_t)self->srcfilesz + 1);
        size_t didread;
        vfsRead(self->srcfile, self->srcbuf, (size_t)self->srcfilesz, &didread);
        self->srcbuf[didread] = 0;
    }

    svg = nsvgParse(self->srcbuf, "px", 96);
    if (!svg)
        goto finally;

    float scalex = 1, scaley = 1;
    float dpiscale = (float)self->preferred_dpi / 96.f;

    // figure out size
    if (self->preferred_width == 0 && self->preferred_height == 0) {
        // use the SVG viewbox dimensions as-is
        img->width  = (int)(svg->width * dpiscale + 0.5);
        img->height = (int)(svg->height * dpiscale + 0.5);
        scalex = scaley = dpiscale;
    } else if (self->preferred_height == 0) {
        // width supplied, fill in height from aspect ratio
        img->width  = (int)(self->preferred_width * dpiscale + 0.5);
        img->height = (int)((float)self->preferred_width * dpiscale * svg->height / svg->width +
                            0.5);
        scalex = scaley = (float)self->preferred_width / svg->width * dpiscale;
    } else if (self->preferred_width == 0) {
        // height supplied, fill in width from aspect ratio
        img->width  = (int)((float)self->preferred_height * dpiscale * svg->width / svg->height +
                           0.5);
        img->height = (int)(self->preferred_height * dpiscale + 0.5);
        scalex = scaley = (float)self->preferred_height / svg->height * dpiscale;
    } else {
        // width and height explicitly specified
        img->width  = self->preferred_width;
        img->height = self->preferred_height;
        scalex      = (float)self->preferred_width / svg->width * dpiscale;
        scaley      = (float)self->preferred_height / svg->height * dpiscale;
    }

    img->pixels = xaAlloc(img->width * img->height * 4);
    img->dpi    = self->preferred_dpi;

    rast = nsvgCreateRasterizer();
    nsvgRasterize(rast,
                  svg,
                  0,
                  0,
                  scalex,
                  scaley,
                  img->pixels,
                  img->width,
                  img->height,
                  img->width * 4);

    ret = true;

finally:
    if (rast)
        nsvgDeleteRasterizer(rast);
    if (svg)
        nsvgDelete(svg);

    return ret;
}

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "image.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed Image* Image_createEmpty()
{
    Image* self;
    self = objInstCreate(Image);

    objInstInit(self);
    return self;
}

_objfactory_guaranteed Image* Image_createBlank(int w, int h)
{
    Image* self;
    self = objInstCreate(Image);

    self->width  = w;
    self->height = h;
    self->pixels = xaAlloc(w * h * 4, XA_Zero);

    objInstInit(self);
    return self;
}

_objfactory_guaranteed Image* Image_createSolid(int w, int h, int r, int g, int b)
{
    Image* self;
    self = objInstCreate(Image);

    self->width  = w;
    self->height = h;
    self->pixels = xaAlloc(w * h * 4);

    objInstInit(self);

        size_t rowsz = w * 4;
    uint8* row   = xaAlloc(rowsz);
    for (size_t x = 0; x < rowsz - 3; x += 4) {
        row[x]     = r;
        row[x + 1] = g;
        row[x + 2] = b;
        row[x + 3] = 255;
    }

    for (int y = 0; y < h; y++) {
        memcpy(&self->pixels[y * rowsz], row, rowsz);
    }

    xaFree(row);

    return self;
}

_objinit_guaranteed bool Image_init(_In_ Image* self)
{
    self->dpi = 96;
    self->bpp = 4;
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

Image* Image_slice(_In_ Image* self, int x, int y, int w, int h)
{
    Image* ret = imageCreateEmpty();

    int srcw = self->width;
    int srch = self->height;
    int bpp  = self->bpp;

    x = clamp(x, 0, srcw);
    y = clamp(y, 0, srch);
    w = clamp(w, 0, srcw - x);
    h = clamp(h, 0, srch - y);

    if (w == 0 || h == 0)
        return ret;

    ret->width  = w;
    ret->height = h;
    ret->bpp    = bpp;
    ret->dpi    = self->dpi;
    ret->pixels = xaAlloc(w * h * bpp);
    if (!ret->pixels)
        return ret;

    for (int r = 0; r < h; r++) {
        memcpy(&ret->pixels[r * w * bpp], &self->pixels[((r + y) * srcw + x) * bpp], w * bpp);
    }

    return ret;
}

Ihandle* Image_iupImage(_In_ Image* self)
{
    Ihandle* ih = NULL;

    if (self->bpp == 3) {
        ih = IupImageRGB(self->width, self->height, self->pixels);
    } else if (self->bpp == 4) {
        ih = IupImageRGBA(self->width, self->height, self->pixels);
    }
    if (self->dpi > 0) {
        string dpi = 0;
        strFromInt32(&dpi, self->dpi, 10);
        IupSetAttribute(ih, "DPI", strC(dpi));
        strDestroy(&dpi);
    }

    return ih;
}

void Image_clear(_In_ Image* self)
{
    if (self->pixels) {
        xaFree(self->pixels);
        self->pixels = 0;
    }
    self->width  = 0;
    self->height = 0;
    self->dpi    = 0;

    // reset DPI and BPP
    Image_init(self);
}

void Image_destroy(_In_ Image* self)
{
    if (self->pixels)
        xaFree(self->pixels);
}

// Autogen begins -----
bool Image_resample(_In_ Image* self, int nwidth, int nheight);
bool Image_resampleToDPI(_In_ Image* self, int ndpi);
#include "image.auto.inc"
// Autogen ends -------

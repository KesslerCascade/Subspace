#include <emmintrin.h>
#include "image.h"

#include <cx/utils.h>
#include <math.h>

// Image upsample / downsample is based on repetitive application of a pseudo-gaussian curve,
// followed by a final sharpening step.

#define exepi16_8(svar, n) (uint8)_mm_extract_epi16(svar, n)

#define vecpx4(pxA, pxB) \
    _mm_set_epi16((pxB)[3], (pxB)[2], (pxB)[1], (pxB)[0], (pxA)[3], (pxA)[2], (pxA)[1], (pxA)[0])
#define vecpx3(pxA, pxB) \
    _mm_set_epi16(0, (pxB)[2], (pxB)[1], (pxB)[0], 0, (pxA)[2], (pxA)[1], (pxA)[0])
#define vecout4(svar, opxA, opxB)   \
    (opxA)[0] = exepi16_8(svar, 0); \
    (opxA)[1] = exepi16_8(svar, 1); \
    (opxA)[2] = exepi16_8(svar, 2); \
    (opxA)[3] = exepi16_8(svar, 3); \
    (opxB)[0] = exepi16_8(svar, 4); \
    (opxB)[1] = exepi16_8(svar, 5); \
    (opxB)[2] = exepi16_8(svar, 6); \
    (opxB)[3] = exepi16_8(svar, 7)
#define vecout3(svar, opxA, opxB)   \
    (opxA)[0] = exepi16_8(svar, 0); \
    (opxA)[1] = exepi16_8(svar, 1); \
    (opxA)[2] = exepi16_8(svar, 2); \
    (opxB)[0] = exepi16_8(svar, 4); \
    (opxB)[1] = exepi16_8(svar, 5); \
    (opxB)[2] = exepi16_8(svar, 6)

static uint8 dummy[4];

typedef void (*rgbaCopyIn)(Image* self, bool vert, uint8* data, int span, int spansize, int lpad,
                           int rpad);
typedef void (*vecCopyOut)(Image* self, bool vert, uint8* outbuf, __m128i* vecdata, int span,
                           int spansize);

static bool _halfVec(Image* self, bool vert, uint8* outbuf, int spansize, int spans,
                     rgbaCopyIn copyin, vecCopyOut copyout)
{
    int outspansize = spansize / 2;

    // Input span needs extra 1 pixel at the beginning, and 2-3 at the end depending on if the
    // source size is even or odd, so that no conditional processing to handle the edges has to be
    // done in the inner loop. These must be even because we process 2 pixels at a time.
    uint8* rgbain = xaAlloc((spansize + 3 + ((spansize + 3) & 1)) * 4, XA_Align(16));
    if (!rgbain)
        return false;
    // image processing results are stored as 16-bit signed integers
    __m128i* vdout = xaAlloc((outspansize + (outspansize & 1)) * 8, XA_Align(16));
    if (!vdout) {
        xaFree(rgbain);
        return false;
    }

    const __m128i zeroes        = _mm_set1_epi16(0);
    const __m128i twofiftyfives = _mm_set1_epi16(255);
    const __m128i threes        = _mm_set1_epi16(3);
    const __m128i fours         = _mm_set1_epi16(4);

    uint8* ip;
    __m128i* op;
    for (int span = 0; span < spans; span++) {
        copyin(self, vert, rgbain, span, spansize, 1, 2);
        ip = rgbain;
        op = vdout;
        for (int i = 0; i < outspansize; i += 2) {
            *op = _mm_min_epi16(
                _mm_max_epi16(_mm_srai_epi16(
                                  _mm_add_epi16(_mm_add_epi16(_mm_add_epi16(vecpx4(&ip[0],
                                                                                   &ip[2 * 4]),
                                                                            vecpx4(&ip[3 * 4],
                                                                                   &ip[5 * 4])),
                                                              _mm_mullo_epi16(
                                                                  _mm_add_epi16(vecpx4(&ip[1 * 4],
                                                                                       &ip[3 * 4]),
                                                                                vecpx4(&ip[2 * 4],
                                                                                       &ip[4 * 4])),
                                                                  threes)),
                                                fours),
                                  3),
                              zeroes),
                twofiftyfives);
            ip += 4 * 4;
            op++;
        }
        copyout(self, vert, outbuf, vdout, span, outspansize);
    }

    xaFree(rgbain);
    xaFree(vdout);

    return true;
}

static bool _doubleVec(Image* self, bool vert, uint8* outbuf, int spansize, int spans,
                       rgbaCopyIn copyin, vecCopyOut copyout)
{
    int outspansize = spansize * 2;

    // Input span needs extra 1 pixel at the beginning, and 1-2 at the end depending on if the
    // source size is even or odd, so that no conditional processing to handle the edges has to be
    // done in the inner loop. These must be even because we process 2 pixels at a time.
    uint8* rgbain = xaAlloc((spansize + 2 + ((spansize + 2) & 1)) * 4, XA_Align(16));
    if (!rgbain)
        return false;
    // image processing results are stored as 16-bit signed integers
    __m128i* vdout = xaAlloc((outspansize + (outspansize & 1)) * 8, XA_Align(16));
    if (!vdout) {
        xaFree(rgbain);
        return false;
    }

    const __m128i zeroes        = _mm_set1_epi16(0);
    const __m128i twofiftyfives = _mm_set1_epi16(255);
    const __m128i twos          = _mm_set1_epi16(2);
    const __m128i threes        = _mm_set1_epi16(3);

    uint8* ip;
    __m128i* op;
    for (int span = 0; span < spans; span++) {
        copyin(self, vert, rgbain, span, spansize, 1, 1);
        ip = rgbain;
        op = vdout;
        for (int i = 0; i < outspansize; i += 2) {
            *op = _mm_min_epi16(
                _mm_max_epi16(
                    _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(vecpx4(&ip[1 * 4],
                                                                                      &ip[1 * 4]),
                                                                               threes),
                                                               vecpx4(&ip[0 * 4], &ip[2 * 4])),
                                                 twos),
                                   2),
                    zeroes),
                twofiftyfives);
            ip += 4;
            op++;
        }
        copyout(self, vert, outbuf, vdout, span, outspansize);
    }

    xaFree(rgbain);
    xaFree(vdout);

    return true;
}

// copy a row in
static void copyInDirect(Image* self, bool vert, uint8* data, int span, int spansize, int lpad,
                         int rpad)
{
    int i;
    int bpp = self->bpp;
    int stride;
    uint8 *ip, *op;

    if (vert) {
        devAssert(spansize == self->height);
        ip     = self->pixels + span * bpp;
        stride = self->width * bpp;
    } else {
        devAssert(spansize == self->width);
        ip     = self->pixels + span * self->width * bpp;
        stride = bpp;
    }
    op = data;

    if (bpp == 4 && !vert) {
        // special case where we can direct copy the pixel data
        for (i = 0; i < lpad; i++) {
            // duplicate first pixel
            *(uint32*)op = *(uint32*)ip;
            op += 4;
        }
        memcpy(op, ip, spansize * 4);
        op += spansize * 4;
        ip += spansize * 4 - 4;
        for (i = 0; i < rpad; i++) {
            // duplicate last pixel
            *(uint32*)op = *(uint32*)ip;
            op += 4;
        }
    } else if (bpp == 4) {
        for (i = 0; i < lpad; i++) {
            // duplicate first pixel
            *(uint32*)op = *(uint32*)ip;
            op += 4;
        }
        for (i = 0; i < spansize; i++) {
            *(uint32*)op = *(uint32*)ip;
            ip += stride;
            op += 4;
        }
        ip -= stride;
        for (i = 0; i < rpad; i++) {
            // duplicate last pixel
            *(uint32*)op = *(uint32*)ip;
            op += 4;
        }
    } else {
        for (i = 0; i < lpad; i++) {
            // duplicate first pixel
            *(uint32*)op = ip[0] | ip[1] << 8 | ip[2] << 16;
            op += 4;
        }
        for (i = 0; i < spansize; i++) {
            *(uint32*)op = ip[0] | ip[1] << 8 | ip[2] << 16;
            ip += stride;
            op += 4;
        }
        ip -= stride;
        for (i = 0; i < rpad; i++) {
            // duplicate last pixel
            *(uint32*)op = ip[0] | ip[1] << 8 | ip[2] << 16;
            op += 4;
        }
    }
}

typedef union M128_32 {
    __m128i i128;
    int32 i32[4];
} M128_32;

static _meta_inline void lerp4(uint8* op, uint8* ip1, uint8* ip2, int t)
{
    M128_32 out;
    __m128i p1  = _mm_set_epi32(ip1[3], ip1[2], ip1[1], ip1[0]);
    __m128i adj = _mm_set_epi32((ip2[3] - ip1[3]) * t,
                                (ip2[2] - ip1[2]) * t,
                                (ip2[1] - ip1[1]) * t,
                                (ip2[0] - ip1[0]) * t);
    out.i128    = _mm_srli_epi32(_mm_add_epi32(_mm_slli_epi32(p1, 16), adj), 16);
    op[0]       = (uint8)out.i32[0];
    op[1]       = (uint8)out.i32[1];
    op[2]       = (uint8)out.i32[2];
    op[3]       = (uint8)out.i32[3];
}

static _meta_inline void lerp3(uint8* op, uint8* ip1, uint8* ip2, int t)
{
    M128_32 out;
    __m128i p1  = _mm_set_epi32(0, ip1[2], ip1[1], ip1[0]);
    __m128i adj = _mm_set_epi32(0,
                                (ip2[2] - ip1[2]) * t,
                                (ip2[1] - ip1[1]) * t,
                                (ip2[0] - ip1[0]) * t);
    out.i128    = _mm_srli_epi32(_mm_add_epi32(_mm_slli_epi32(p1, 16), adj), 16);
    op[0]       = (uint8)out.i32[0];
    op[1]       = (uint8)out.i32[1];
    op[2]       = (uint8)out.i32[2];
    op[3]       = 0;
}

static void copyInInterp(Image* self, bool vert, uint8* data, int span, int spansize, int lpad,
                         int rpad)
{
    int i;
    int bpp = self->bpp;
    int stride;
    uint8 *ibase, *ip, *op;
    int di, step;

    if (vert) {
        step = ((self->height - 1) << 16) / (spansize - 1);
        devAssert(step < 65536);
        ibase  = self->pixels + span * bpp;
        stride = self->width * bpp;
    } else {
        step = ((self->width - 1) << 16) / (spansize - 1);
        devAssert(step < 65536);
        ibase  = self->pixels + span * self->width * bpp;
        stride = bpp;
    }
    op = data;

    di = 0;
    if (bpp == 4) {
        for (i = 0; i < lpad; i++) {
            // duplicate first pixel
            *(uint32*)op = *(uint32*)ibase;
            op += 4;
        }
        for (i = 0; i < spansize; i++) {
            lerp4(op,
                  &ibase[(size_t)((di >> 16) * stride)],
                  &ibase[(size_t)(((di >> 16) + 1) * stride)],
                  di & 0xffff);
            di += step;
            op += 4;
        }
        ip = &ibase[(size_t)(((di - step) >> 16) * stride)];
        for (i = 0; i < rpad; i++) {
            // duplicate last pixel
            *(uint32*)op = *(uint32*)ip;
            op += 4;
        }
    } else {
        for (i = 0; i < lpad; i++) {
            // duplicate first pixel
            ip           = ibase;
            *(uint32*)op = ip[0] | ip[1] << 8 | ip[2] << 16;
            op += 4;
        }
        for (i = 0; i < spansize; i++) {
            lerp3(op,
                  &ibase[(size_t)((di >> 16) * stride)],
                  &ibase[(size_t)(((di >> 16) + 1) * stride)],
                  di & 0xffff);
            di += step;
            op += 4;
        }
        ip = &ibase[(size_t)(((di - step) >> 16) * stride)];
        for (i = 0; i < rpad; i++) {
            // duplicate last pixel
            *(uint32*)op = ip[0] | ip[1] << 8 | ip[2] << 16;
            op += 4;
        }
    }
}

static void copyOutDirect(Image* self, bool vert, uint8* outbuf, __m128i* vecdata, int span,
                          int spansize)
{
    int i;

    int bpp = self->bpp;
    int stride;
    uint8* op;

    if (vert) {
        op     = outbuf + span * bpp;
        stride = self->width * bpp;
    } else {
        op     = outbuf + span * spansize * bpp;
        stride = bpp;
    }

    if (bpp == 4) {
        for (i = 0; i < spansize / 2; i++) {
            vecout4(*vecdata, op, op + stride);
            op += stride * 2;
            vecdata++;
        }
        if (spansize & 1) {
            // one more
            vecout4(*vecdata, op, dummy);
        }
    } else {
        for (i = 0; i < spansize / 2; i++) {
            vecout3(*vecdata, op, op + stride);
            op += stride * 2;
            vecdata++;
        }
        if (spansize & 1) {
            // one more
            vecout3(*vecdata, op, dummy);
        }
    }
}

static bool _halfImageX(Image* self, int targetWidth)
{
    bool ret    = false;
    int iwidth  = targetWidth * 2;
    int iheight = self->height;

    uint8* opixels = xaAlloc(targetWidth * iheight * self->bpp);

    ret = _halfVec(self,
                   false,
                   opixels,
                   iwidth,
                   iheight,
                   (self->width >= iwidth) ? copyInDirect : copyInInterp,
                   copyOutDirect);

    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
        self->width  = targetWidth;
    } else {
        xaFree(opixels);
    }
    return ret;
}

static bool _halfImageY(Image* self, int targetHeight)
{
    bool ret    = false;
    int iwidth  = self->width;
    int iheight = targetHeight * 2;

    uint8* opixels = xaAlloc(iwidth * targetHeight * self->bpp);

    ret = _halfVec(self,
                   true,
                   opixels,
                   iheight,
                   iwidth,
                   (self->height >= iheight) ? copyInDirect : copyInInterp,
                   copyOutDirect);

    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
        self->height = targetHeight;
    } else {
        xaFree(opixels);
    }
    return ret;
}

static bool _doubleImageX(Image* self, int targetWidth)
{
    bool ret    = false;
    int iwidth  = targetWidth / 2;
    int iheight = self->height;

    uint8* opixels = xaAlloc(targetWidth * iheight * self->bpp);

    ret = _doubleVec(self,
                     false,
                     opixels,
                     iwidth,
                     iheight,
                     (self->width >= iwidth) ? copyInDirect : copyInInterp,
                     copyOutDirect);

    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
        self->width  = targetWidth;
    } else {
        xaFree(opixels);
    }
    return ret;
}

static bool _doubleImageY(Image* self, int targetHeight)
{
    bool ret    = false;
    int iheight = targetHeight / 2;
    int iwidth  = self->width;

    uint8* opixels = xaAlloc(targetHeight * iwidth * self->bpp);

    ret = _doubleVec(self,
                     true,
                     opixels,
                     iheight,
                     iwidth,
                     (self->height >= iheight) ? copyInDirect : copyInInterp,
                     copyOutDirect);

    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
        self->height = targetHeight;
    } else {
        xaFree(opixels);
    }

    return ret;
}

static bool _sharpVec(Image* self, bool vert, uint8* outbuf, int spansize, int spans,
                      rgbaCopyIn copyin, vecCopyOut copyout)
{
    // Input span needs extra 1 pixel at the beginning, and 1-2 at the end depending on if the
    // source size is even or odd, so that no conditional processing to handle the edges has to be
    // done in the inner loop. These must be even because we process 2 pixels at a time.
    uint8* rgbain = xaAlloc((spansize + 2 + ((spansize + 2) & 1)) * 4, XA_Align(16));
    if (!rgbain)
        return false;
    // image processing results are stored as 16-bit signed integers
    __m128i* vdout = xaAlloc((spansize + (spansize & 1)) * 8, XA_Align(16));
    if (!vdout) {
        xaFree(rgbain);
        return false;
    }

    const __m128i zeroes        = _mm_set1_epi16(0);
    const __m128i twofiftyfives = _mm_set1_epi16(255);
    const __m128i sixes         = _mm_set1_epi16(6);
    const __m128i twos          = _mm_set1_epi16(2);

    uint8* ip;
    __m128i* op;
    for (int span = 0; span < spans; span++) {
        copyin(self, vert, rgbain, span, spansize, 1, 1);
        ip = rgbain;
        op = vdout;
        for (int i = 0; i < spansize; i += 2) {
            *op = _mm_min_epi16(
                _mm_max_epi16(
                    _mm_srai_epi16(_mm_add_epi16(_mm_sub_epi16(_mm_mullo_epi16(vecpx4(&ip[1 * 4],
                                                                                      &ip[2 * 4]),
                                                                               sixes),
                                                               _mm_add_epi16(vecpx4(&ip[0],
                                                                                    &ip[1 * 4]),
                                                                             vecpx4(&ip[2 * 4],
                                                                                    &ip[3 * 4]))),
                                                 twos),
                                   2),
                    zeroes),
                twofiftyfives);
            ip += 2 * 4;
            op++;
        }
        copyout(self, vert, outbuf, vdout, span, spansize);
    }

    xaFree(rgbain);
    xaFree(vdout);

    return true;
}

static bool _sharpen(Image* self)
{
    bool ret;
    int width  = self->width;
    int height = self->height;

    uint8* opixels = xaAlloc(width * height * self->bpp);
    ret = _sharpVec(self, false, opixels, self->width, self->height, copyInDirect, copyOutDirect);
    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
    } else {
        xaFree(opixels);
        return false;
    }

    opixels = xaAlloc(width * height * self->bpp);
    ret = _sharpVec(self, true, opixels, self->height, self->width, copyInDirect, copyOutDirect);
    if (ret) {
        xaFree(self->pixels);
        self->pixels = opixels;
    } else {
        xaFree(opixels);
    }

    return ret;
}

bool Image_resample(_In_ Image* self, int nwidth, int nheight)
{
    bool needsharp   = false;
    int targetWidth  = nwidth;
    int targetHeight = nheight;

    // find the biggest size that is a power-of-two factor larger
    // than our desired size, but is smaller than the actual image
    while (targetWidth * 2 < self->width) targetWidth *= 2;
    while (targetHeight * 2 < self->height) targetHeight *= 2;

    // first, reduce image by half steps as much as possible
    // the first step will use linear interpolation if necessary
    // (with as much resolution available as possible)
    while (self->width > nwidth) {
        if (!_halfImageX(self, targetWidth))
            return false;
        needsharp = true;
        targetWidth /= 2;
    }
    while (self->height > nheight) {
        if (!_halfImageY(self, targetHeight))
            return false;
        needsharp = true;
        targetHeight /= 2;
    }

    // EXPANSION - this comes second, because if we're reducing one dimension
    // and expanding the other, it's much more efficient to do the reduction first

    // if we are expanding to 2x or larger, do some doubling first
    while (self->height <= nheight / 2) {
        if (!_doubleImageY(self, self->height * 2))
            return false;
        needsharp = true;
    }
    // For the final phase of the expansion, it looks much nicer to interpolate to
    // double the target size and then reduce down
    if (nheight > self->height && nheight < self->height * 2) {
        if (!_halfImageY(self, nheight))
            return false;
        needsharp = true;
    }

    // do X axis expansion last because it's the fastest due to cache locality, which
    // helps offset the extra vertical resolution
    while (self->width <= nwidth / 2) {
        if (!_doubleImageX(self, self->width * 2))
            return false;
        needsharp = true;
    }
    if (nwidth > self->width) {
        if (!_halfImageX(self, nwidth))
            return false;
        needsharp = true;
    }

    devAssert(self->width == nwidth);
    devAssert(self->height == nheight);

    if (needsharp)
        return _sharpen(self);
    return true;
}

bool Image_resampleToDPI(_In_ Image* self, int ndpi)
{
    bool ret      = true;
    double factor = (double)ndpi / (double)self->dpi;
    if (factor != 1) {
        ret = Image_resample(self,
                             (int)(self->width * factor + 0.5),
                             (int)(self->height * factor + 0.5));
        if (ret)
            self->dpi = ndpi;
    }

    return ret;
}

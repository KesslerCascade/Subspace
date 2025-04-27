#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/fs.h>
#include <iup.h>

typedef struct Image Image;
typedef struct Image_WeakRef Image_WeakRef;
saDeclarePtr(Image);
saDeclarePtr(Image_WeakRef);

typedef struct Image_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    Image* (*slice)(_In_ void* self, int x, int y, int w, int h);
    Ihandle* (*iupImage)(_In_ void* self);
    bool (*resample)(_In_ void* self, int nwidth, int nheight);
    bool (*resampleToDPI)(_In_ void* self, int ndpi);
    void (*clear)(_In_ void* self);
} Image_ClassIf;
extern Image_ClassIf Image_ClassIf_tmpl;

typedef struct Image {
    union {
        Image_ClassIf* _;
        void* _is_Image;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int width;
    int height;
    int bpp;        // bytes per pixel, will be 3 or 4
    int dpi;
    uint8* pixels;
} Image;
extern ObjClassInfo Image_clsinfo;
#define Image(inst) ((Image*)(unused_noeval((inst) && &((inst)->_is_Image)), (inst)))
#define ImageNone ((Image*)NULL)

typedef struct Image_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Image_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Image_WeakRef;
#define Image_WeakRef(inst) ((Image_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Image_WeakRef)), (inst)))

_objfactory_guaranteed Image* Image_createEmpty();
// Image* imageCreateEmpty();
#define imageCreateEmpty() Image_createEmpty()

_objfactory_guaranteed Image* Image_createBlank(int w, int h);
// Image* imageCreateBlank(int w, int h);
#define imageCreateBlank(w, h) Image_createBlank(w, h)

_objfactory_guaranteed Image* Image_createSolid(int w, int h, int r, int g, int b);
// Image* imageCreateSolid(int w, int h, int r, int g, int b);
#define imageCreateSolid(w, h, r, g, b) Image_createSolid(w, h, r, g, b)

// Image* imageSlice(Image* self, int x, int y, int w, int h);
#define imageSlice(self, x, y, w, h) (self)->_->slice(Image(self), x, y, w, h)
// Ihandle* imageIupImage(Image* self);
#define imageIupImage(self) (self)->_->iupImage(Image(self))
// bool imageResample(Image* self, int nwidth, int nheight);
#define imageResample(self, nwidth, nheight) (self)->_->resample(Image(self), nwidth, nheight)
// bool imageResampleToDPI(Image* self, int ndpi);
#define imageResampleToDPI(self, ndpi) (self)->_->resampleToDPI(Image(self), ndpi)
// void imageClear(Image* self);
#define imageClear(self) (self)->_->clear(Image(self))


// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "imageresample.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_check ImageResample* ImageResample_toSize(Image* image, int width, int height)
{
    if (!(width > 0 && height > 0))
        return NULL;

    ImageResample* self;
    self = objInstCreate(ImageResample);

    self->image        = objAcquire(image);
    self->targetWidth  = width;
    self->targetHeight = height;

    self->name = _S"ImageResample";
    if (!objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

_objfactory_check ImageResample* ImageResample_toDPI(Image* image, int dpi)
{
    if (!(dpi > 0))
        return NULL;

    ImageResample* self;
    self = objInstCreate(ImageResample);

    self->image     = objAcquire(image);
    self->targetDPI = dpi;

    self->name = _S"ImageResample";
    if (!objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

_objinit_guaranteed bool ImageResample_init(_In_ ImageResample* self)
{
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

uint32 ImageResample_run(_In_ ImageResample* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                         _Inout_ TaskControl* tcon)
{
    bool success = false;

    if (self->targetDPI > 0) {
        success = imageResampleToDPI(self->image, self->targetDPI);
    } else {
        success = imageResample(self->image, self->targetWidth, self->targetHeight);
    }

    return success ? TASK_Result_Success : TASK_Result_Failure;
}

void ImageResample_destroy(_In_ ImageResample* self)
{
    // Autogen begins -----
    objRelease(&self->image);
    // Autogen ends -------
}

// Autogen begins -----
#include "imageresample.auto.inc"
// Autogen ends -------

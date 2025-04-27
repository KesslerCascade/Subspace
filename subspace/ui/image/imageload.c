// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "imageload.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/image/imageresample.h"
#include "imagedrivers.h"

static int findDriver(strref driver)
{
    if (strEqi(driver, _S"png")) {
        return IMAGE_DRIVER_PNG;
    }
    if (strEqi(driver, _S"svg")) {
        return IMAGE_DRIVER_SVG;
    }

    return IMAGE_DRIVER_NONE;
}

_objfactory_check ImageLoad* ImageLoad_loadFile(_In_opt_ strref driver, VFS* vfs, _In_opt_ strref fname)
{
    ImageLoad* self;
    self = objInstCreate(ImageLoad);

    self->driver = findDriver(driver);
    if (self->driver == IMAGE_DRIVER_NONE) {
        // try file extension if no driver specified
        string ext = 0;
        pathGetExt(&ext, fname);
        self->driver = findDriver(ext);
        strDestroy(&ext);
    }

    FSStat stat;
    if (vfsStat(vfs, fname, &stat) == FS_File) {
        self->srcfilesz = stat.size;
        self->srcfile   = vfsOpen(vfs, fname, FS_Read);
    }

    self->name = _S"ImageLoad";
    if (self->driver == IMAGE_DRIVER_NONE || !self->srcfilesz || !self->srcfile ||
        !objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

_objfactory_check ImageLoad* ImageLoad_loadBuffer(_In_opt_ strref driver, uint8* buf, size_t sz)
{
    ImageLoad* self;
    self = objInstCreate(ImageLoad);

    self->driver = findDriver(driver);

    // Some image drivers want a guarantee that the buffer is NULL-terminated,
    // for example SVG which is an XML document. Just go ahead and allocate an
    // extra byte since this is transient anyway.
    self->srcbufsz = sz;
    self->srcbuf   = xaAlloc(sz + 1);

    self->name = _S"ImageLoad";
    if (self->driver == IMAGE_DRIVER_NONE || !self->srcbuf || !objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    self->srcbufp = self->srcbuf;
    memcpy(self->srcbuf, buf, sz);
    self->srcbuf[sz] = 0;

    return self;
}

_objinit_guaranteed bool ImageLoad_init(_In_ ImageLoad* self)
{
    self->image         = imageCreateEmpty();
    self->preferred_dpi = 96;

    saPush(&self->phases, ptr, ImageLoad_loadPhase);
    saPush(&self->phases, ptr, ImageLoad_finishPhase);
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

uint32 ImageLoad_loadPhase(_In_ ImageLoad* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon)
{
    bool success = false;
    switch (self->driver) {
    case IMAGE_DRIVER_PNG:
        success = imageDriverPng(self);
        break;
    case IMAGE_DRIVER_SVG:
        success = imageDriverSvg(self);
        break;
    }

    // success or not, don't need source data anymore
    if (self->srcbuf) {
        xaFree(self->srcbuf);
        self->srcbufp = self->srcbuf = NULL;
        self->srcbufsz               = 0;
    }
    if (self->srcfile) {
        vfsClose(self->srcfile);
        self->srcfile   = NULL;
        self->srcfilesz = 0;
    }

    // if we're targeting a specific DPI and the loaded image was a different DPI, resample it
    if (self->preferred_dpi > 0 && self->image->dpi > 0 &&
        self->image->dpi != self->preferred_dpi) {
        ImageResample* rstask = imageresampleToDPI(self->image, self->preferred_dpi);
        ctaskDependOn(self, rstask);
        tqRun(tq, &rstask);
    }

    return success ? TASK_Result_Success : TASK_Result_Failure;
}

uint32 ImageLoad_finishPhase(_In_ ImageLoad* self, TaskQueue* tq, TQWorker* worker,
                             TaskControl* tcon)
{
    return TASK_Result_Success;
}

void ImageLoad_destroy(_In_ ImageLoad* self)
{
    vfsClose(self->srcfile);
    xaFree(self->srcbuf);
    // Autogen begins -----
    objRelease(&self->image);
    // Autogen ends -------
}

// Autogen begins -----
#include "imageload.auto.inc"
// Autogen ends -------

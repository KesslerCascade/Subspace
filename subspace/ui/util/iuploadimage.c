#include "iuploadimage.h"
#include "ui/subspaceui.h"
#include "ui/util/iupimageattachdispatch.h"

bool cbAttachImage(stvlist* cvars, stvlist* args)
{
    ImageLoad* self = stvlNextObj(args, ImageLoad);
    bool ret        = false;
    Weak(TaskQueue)* wtq;
    string name    = 0;
    TaskQueue* uiq = NULL;
    if (!stvlNext(cvars, weakref, &wtq) || !(uiq = objAcquireFromWeak(TaskQueue, wtq)))
        return false;

    Ihandle* ih = stvlNextPtr(cvars);
    if (!ih || !stvlNext(cvars, string, &name))
        goto out;

    IupImageAttachDispatch* itask = iupimageattachdispatchCreate(ih, name, self->image);
    tqRun(uiq, &itask);
    ret = true;

out:
    objRelease(&uiq);
    return ret;
}

void iupLoadImage(SubspaceUI* ui, Ihandle* control, strref attr, strref driver, strref filename)
{
    ImageLoad* iload     = imageloadLoadFile(driver, ui->ss->fs, filename);
    int dpi              = atoi(IupGetGlobal("SCREENDPI"));
    iload->preferred_dpi = (dpi > 0) ? dpi : 96;

    // Run it in the worker queue, but use a callback to dispatch the actual attribute setting back
    // to the main UI thread
    cchainAttach(&iload->oncomplete,
                 cbAttachImage,
                 stvar(weakref, objGetWeak(TaskQueue, ui->uiq)),
                 stvar(ptr, control),
                 stvar(string, _S"IMAGE"));
    tqRun(ui->uiworkers, &iload);
}

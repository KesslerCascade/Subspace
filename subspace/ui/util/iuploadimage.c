#include "iuploadimage.h"
#include "ui/subspaceui.h"
#include "ui/util/iuprefreshdispatch.h"
#include "ui/util/iupsetimagedispatch.h"

bool cbAttachImage(stvlist* cvars, stvlist* args)
{
    ImageLoad* self = stvlNextObj(args, ImageLoad);
    bool ret        = false;
    Weak(TaskQueue)* wtq;
    string name    = 0;
    TaskQueue* uiq = NULL;
    if (!stvlNext(cvars, weakref, &wtq) || !(uiq = objAcquireFromWeak(TaskQueue, wtq)))
        return false;

    if (!stvlNext(cvars, string, &name))
        goto out;

    IupSetImageDispatch* itask = iupsetimagedispatchCreate(name, self->image);
    tqRun(uiq, &itask);
    ret = true;

    Ihandle* torefresh = stvlNextPtr(cvars);
    if (torefresh) {
        IupRefreshDispatch* rtask = iuprefreshdispatchCreate(torefresh);
        tqRun(uiq, &rtask);
    }

out:
    objRelease(&uiq);
    return ret;
}

void iupLoadImage(Subspace* ss, strref iupname, strref driver, strref filename, Ihandle* torefresh)
{
    ImageLoad* iload     = imageloadLoadFile(driver, ss->fs, filename);
    int dpi              = atoi(IupGetGlobal("SCREENDPI"));
    iload->preferred_dpi = (dpi > 0) ? dpi : 96;

    // Run it in the worker queue, but use a callback to dispatch the actual attribute setting back
    // to the main UI thread
    cchainAttach(&iload->oncomplete,
                 cbAttachImage,
                 stvar(weakref, objGetWeak(TaskQueue, ss->ui->uiq)),
                 stvar(strref, iupname),
                 stvar(ptr, torefresh));
    tqRun(ss->workq, &iload);
}

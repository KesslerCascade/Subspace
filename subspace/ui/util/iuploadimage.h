#include "ui/image/imageload.h"
#include "subspace.h"

// Callback for use with ImageLoad to set the loaded image as an IUP object's attribute.
// cvars: weak(TaskQueue) uiq, ptr Ihandle*, string attrname, [optional] Ihandle *dlg_to_refresh
bool cbAttachImage(stvlist *cvars, stvlist *args);

// must be called from UI thread
void iupLoadImage(SubspaceUI* ui, strref iupname, strref driver, strref filename,
                  Ihandle* torefresh);

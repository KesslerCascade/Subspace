#pragma once

#include <iup.h>
#include <cx/obj.h>
#include "ui/subspaceui.h"

#ifndef ObjNone
#define ObjNone ((ObjInst*)NULL)
#endif

_meta_inline void _iupSetObj(Ihandle* ih, ObjInst* obj, ObjInst* parent, SubspaceUI* ui)
{
    if (obj)
        IupSetAttribute(ih, "OBJINST", (char*)obj);
    if (parent)
        IupSetAttribute(ih, "OBJPARENT", (char*)parent);
    if (ui) {
        IupSetAttribute(ih, "SUBSPACE", (char*)ui->ss);
        IupSetAttribute(ih, "SUBSPACEUI", (char*)ui);
    }
}
#define iupSetObj(ih, obj, parent, ui) _iupSetObj(ih, ObjInst(obj), ObjInst(parent), ui)

_meta_inline ObjInst* _iupGetObj(Ihandle* ih, const char *attr)
{
    return (ObjInst*)IupGetAttribute(ih, attr);
}

#define iupGetObj(clsname, ih) objDynCast(clsname, _iupGetObj(ih, "OBJINST"))
#define iupGetParentObj(clsname, ih) objDynCast(clsname, _iupGetObj(ih, "OBJPARENT"))

_meta_inline Subspace* iupGetSubspace(Ihandle* ih)
{
    return (Subspace*)IupGetAttribute(ih, "SUBSPACE");
}

_meta_inline SubspaceUI* iupGetUI(Ihandle* ih)
{
    return (SubspaceUI*)IupGetAttribute(ih, "SUBSPACEUI");
}
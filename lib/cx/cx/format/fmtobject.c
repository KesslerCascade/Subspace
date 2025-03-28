#include "format_private.h"
#include "formattable.h"
#include <cx/obj/objstdif.h>

_Use_decl_annotations_
bool _fmtParseObjectOpt(FMTVar *v, strref opt)
{
    // only for objects, save all options into an array in the FMTVar structure so
    // that Formattable has access to them later
    saPush(&v->fmtopts, strref, opt);
    return true;
}

_Use_decl_annotations_
bool _fmtObject(FMTVar *v, string *out)
{
    Formattable *fmtif = (Formattable*)v->fmtdata[0];
    Convertible *cvtif = (Convertible*)v->fmtdata[1];

    if (fmtif)
        return fmtif->format(*(ObjInst**)v->data, v, out);

    // if the object doesn't implement Formattable, it might implement
    // Convertible and can be converted to a string
    if (cvtif)
        return cvtif->convert(*(ObjInst **)v->data, stType(string), stArgPtr(string, out), 0);

    return false;
}

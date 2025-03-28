#pragma once

#include <cx/cx.h>

// Note that this uses a temporary, so the variant's lifetime is equal to the function
// scope that it was created in. It should be used for passing varargs by-value.
#ifndef __cplusplus
#define stvarInit(typen, val) { .data = { .st_##typen = val }, .type = stType(typen) }
#define stvar(typen, val) ((stvar){ .data = stArg(typen, val), .type = stType(typen) })

#define stvNone ((stvar){ .type = stType(none) })
#else
_meta_inline stvar _stvar(stype st, stgeneric val)
{
    stvar ret;
    ret.data = val;
    ret.type = st;
    return ret;
}
#define stvar(typen, val) _stvar(stType(typen), stArg(typen, val))

#define stvNone _stvar(0, stArg(int64, 0))
#endif

_meta_inline void stvarDestroy(stvar *stv)
{
    _stDestroy(stv->type, NULL, &stv->data, 0);
    stv->type = 0;
}

_meta_inline void stvarCopy(stvar *dvar, stvar svar)
{
    dvar->type = svar.type;
    _stCopy(svar.type, NULL, &dvar->data, svar.data, 0);
}

_meta_inline bool _stvarIs(stvar *svar, stype styp)
{
    return svar && stEq(svar->type, styp);
}
#define stvarIs(svar, type) _stvarIs(svar, stType(type))

// Convenience functions for quick access to several common types
_meta_inline string stvarString(stvar *svar)
{
    if (stvarIs(svar, string))
        return svar->data.st_string;
    return NULL;
}

_meta_inline string *stvarStringPtr(stvar *svar)
{
    if (stvarIs(svar, string))
        return &svar->data.st_string;
    return NULL;
}

_meta_inline ObjInst *stvarObjInst(stvar *svar)
{
    if (stvarIs(svar, object))
        return svar->data.st_object;
    return NULL;
}

_meta_inline ObjInst **stvarObjInstPtr(stvar *svar)
{
    if (stvarIs(svar, object))
        return &svar->data.st_object;
    return NULL;
}

#define stvarObj(class, svar) (objDynCast(class, stvarObjInst(svar)))

// Structure for walking a list of stvars with the convenience functions
typedef struct stvlist {
    int count;
    int cursor;
    stvar *vars;
} stvlist;

// Initialize a list from an array of stvars and a count -- i.e. from a pseudovariadic
// function's argument list
void stvlInit(stvlist *list, int count, stvar *vars);

// Initialize a list from an sarray of stvars
void _stvlInitSA(stvlist *list, stvar *vara);
#define stvlInitSA(list, vararray) _stvlInitSA(list, (vararray).a)

// Get the next variable of the specific type, if it exists
bool _stvlNext(stvlist *list, stype type, stgeneric *out);
#define stvlNext(list, type, pvar) _stvlNext(list, stCheckedPtrArg(type, pvar))

void *_stvlNextPtr(stvlist *list, stype type);
#define stvlNextPtr(list) _stvlNextPtr(list, stType(ptr))
#define stvlNextObj(list, class) objDynCast(class, (ObjInst*)_stvlNextPtr(list, stType(object)))

// Rewind the list
void stvlRewind(stvlist *list);

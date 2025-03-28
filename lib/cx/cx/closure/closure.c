#include "closure_private.h"
#include <cx/stype/stype_stvar.h>

_Use_decl_annotations_
closure _closureCreate(closureFunc func, int n, stvar cvars[])
{
    Closure *c = xaAlloc(sizeof(Closure) + n * sizeof(stvar));
    c->func = func;
    c->nvars = n;
    for(int i = 0; i < n; i++) {
        stvarCopy(&c->cvars[i], cvars[i]);
    }
    return (closure)c;
}

_Use_decl_annotations_
closure closureClone(closure cls)
{
    Closure *src = (Closure *)cls;
    Closure *c = xaAlloc(sizeof(Closure) + src->nvars * sizeof(stvar));
    c->func = src->func;
    c->nvars = src->nvars;
    for(int i = 0; i < src->nvars; i++) {
        stvarCopy(&c->cvars[i], src->cvars[i]);
    }
    return (closure)c;
}

_Use_decl_annotations_
bool _closureCall(closure cls, int n, stvar args[])
{
    Closure *c = (Closure *)cls;

    stvlist stv_cvars;
    stvlist stv_args;
    stvlInit(&stv_cvars, c->nvars, c->cvars);
    stvlInit(&stv_args, n, args);
    bool ret = c->func(&stv_cvars, &stv_args);

    return ret;
}

_Use_decl_annotations_
intptr _closureCompare(_In_ closure cls1, _In_ closure cls2)
{
    Closure *c1 = (Closure *)cls1;
    Closure *c2 = (Closure *)cls2;

    if(c1->func != c2->func)
        return (intptr)c1->func - (intptr)c2->func;

    if(c1->nvars != c2->nvars)
        return c1->nvars - c2->nvars;

    for(int i = 0; i < c1->nvars; i++) {
        intptr res = stCmp(stvar, c1->cvars[i], c2->cvars[i]);
        if(res != 0)
            return res;
    }

    return 0;
}

_Use_decl_annotations_
void closureDestroy(closure *cls)
{
    Closure *c = (Closure *)(*cls);

    for(int i = c->nvars - 1; i >= 0; --i) {
        stvarDestroy(&c->cvars[i]);
    }

    xaFree(*cls);
    *cls = NULL;
}

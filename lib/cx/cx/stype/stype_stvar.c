#include "cx/cx.h"

void stDtor_stvar(stype st, stgeneric *gen, uint32 flags)
{
    stvar *stv = gen->st_stvar;

    _stDestroy(stv->type, NULL, &stv->data, flags);
    stv->type = 0;
}

intptr stCmp_stvar(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    stvar *stv1 = gen1.st_stvar;
    stvar *stv2 = gen2.st_stvar;

    if (stv1->type != stv2->type)
        return stv1->type - stv2->type;

    return _stCmp(stv1->type, NULL, stv1->data, stv2->data, flags);
}

void stCopy_stvar(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags)
{
    stvar *dvar = dest->st_stvar;
    stvar *svar = src.st_stvar;

    dvar->type = svar->type;
    _stCopy(svar->type, NULL, &dvar->data, svar->data, flags);
}

uint32 stHash_stvar(stype st, stgeneric gen, uint32 flags)
{
    stvar *stv = gen.st_stvar;
    return _stHash(stv->type, NULL, stv->data, flags);
}

_Success_(return) _Check_return_
bool stConvert_stvar(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    stvar *svar = src.st_stvar;
    return _stConvert(destst, dest, svar->type, NULL, svar->data, flags);
}

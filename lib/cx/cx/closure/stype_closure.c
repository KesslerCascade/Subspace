#include "closure_private.h"
#include "cchain.h"
#include "stype_closure.h"

_Use_decl_annotations_
void stDtor_closure(stype st, stgeneric *stgen, flags_t flags)
{
    closureDestroy(&stgen->st_closure);
}

_Use_decl_annotations_
void stCopy_closure(stype st, stgeneric *dest, stgeneric src, flags_t flags)
{
    dest->st_closure = closureClone(src.st_closure);
}

_Use_decl_annotations_
intptr stCmp_closure(stype st, stgeneric stgen1, stgeneric stgen2, uint32 flags)
{
    return _closureCompare(stgen1.st_closure, stgen2.st_closure);
}

_Use_decl_annotations_
void stDtor_cchain(stype st, stgeneric *stgen, flags_t flags)
{
    cchainDestroy(&stgen->st_cchain);
}

_Use_decl_annotations_
void stCopy_cchain(stype st, stgeneric *dest, stgeneric src, flags_t flags)
{
    cchainClone(&dest->st_cchain, &src.st_cchain);
}

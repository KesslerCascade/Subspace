#include "cx/obj.h"
#include "cx/debug/assert.h"
#include "cx/utils/murmur.h"
#include "objstdif.h"
#include "stype_obj.h"

_Use_decl_annotations_
void stDtor_obj(stype st, stgeneric *gen, uint32 flags)
{
    objRelease(&gen->st_object);
}

_Use_decl_annotations_
void stCopy_obj(stype st, stgeneric *dest, stgeneric src, uint32 flags)
{
    dest->st_object = objAcquire(src.st_object);
}

_Use_decl_annotations_
intptr stCmp_obj(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    ObjInst* inst1 = gen1.st_object;
    ObjInst* inst2 = gen2.st_object;

    if (inst1->_clsinfo->_cmp)
        return inst1->_clsinfo->_cmp(inst1, inst2, flags);

    // If they only care about equality, a pointer compare will suffice for non-sortable objects
    if (flags & ST_Equality)
        return (intptr)inst1 - (intptr)inst2;

    devFatalError("Tried to sort an unsortable object");
    return -1;
}

_Use_decl_annotations_
uint32 stHash_obj(stype st, stgeneric gen, uint32 flags)
{
    ObjInst *inst = gen.st_object;

    devAssert(inst->_clsinfo->_hash);
    if (inst->_clsinfo->_hash)
        return inst->_clsinfo->_hash(inst, flags);

    devFatalError("Tried to hash an unhashable object");
    return 0;
}

_Use_decl_annotations_
bool stConvert_obj(stype    destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags)
{
    ObjInst *inst = src.st_object;

    Convertible *cvtif = objInstIf(src.st_object, Convertible);
    if (cvtif) {
        // zero out dest first
        // stConvert may be used with an uninitialized destination,
        // but the Convertible interface cannot
        memset(stGenPtr(destst, *dest), 0, stGetSize(destst));
        return cvtif->convert(inst, destst, dest, flags);
    }

    return false;
}

void stDtor_weakref(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *stgen, uint32 flags)
{
    objDestroyWeak(&stgen->st_weakref);
}

intptr stCmp_weakref(stype st, _In_ stgeneric stgen1, _In_ stgeneric stgen2, uint32 flags)
{
    // all weak references to the same object share the same pointer
    return (intptr)stgen1.st_weakref - (intptr)stgen2.st_weakref;
}

void stCopy_weakref(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, uint32 flags)
{
    dest->st_weakref = objCloneWeak(src.st_weakref);
}

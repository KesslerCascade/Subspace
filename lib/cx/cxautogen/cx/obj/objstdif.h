#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct Iterator Iterator;
typedef struct Iterator_WeakRef Iterator_WeakRef;
saDeclarePtr(Iterator);
saDeclarePtr(Iterator_WeakRef);

typedef struct Sortable {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Sortable;
extern Sortable Sortable_tmpl;

typedef struct Hashable {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*hash)(_In_ void* self, uint32 flags);
} Hashable;
extern Hashable Hashable_tmpl;

typedef struct Convertible {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // NOTE: While this is used by stConvert, the object interface is a higher level interface.
    // The normal convention of blindly overwriting the destination does not apply here. For
    // example, when called to convert to a string, the destination should be properly reused
    // or destroyed first.
    // The layer between stConvert and Convertible takes care of making sure the destination is
    // always initialized.
    bool (*convert)(_In_ void* self, stype st, stgeneric* dest, uint32 flags);
} Convertible;
extern Convertible Convertible_tmpl;

typedef struct IteratorIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*valid)(_In_ void* self);
    bool (*next)(_In_ void* self);
    bool (*get)(_In_ void* self, stvar* out);
} IteratorIf;
extern IteratorIf IteratorIf_tmpl;

typedef struct Iterable {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // Caller owns the iterator and must release it with objRelease
    Iterator* (*iter)(_In_ void* self);
} Iterable;
extern Iterable Iterable_tmpl;

typedef struct Iterator_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*valid)(_In_ void* self);
    bool (*next)(_In_ void* self);
    bool (*get)(_In_ void* self, stvar* out);
} Iterator_ClassIf;
extern Iterator_ClassIf Iterator_ClassIf_tmpl;

typedef struct Iterator {
    union {
        Iterator_ClassIf* _;
        void* _is_Iterator;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

} Iterator;
extern ObjClassInfo Iterator_clsinfo;
#define Iterator(inst) ((Iterator*)(unused_noeval((inst) && &((inst)->_is_Iterator)), (inst)))
#define IteratorNone ((Iterator*)NULL)

typedef struct Iterator_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Iterator_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Iterator_WeakRef;
#define Iterator_WeakRef(inst) ((Iterator_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Iterator_WeakRef)), (inst)))

// bool iteratorValid(Iterator* self);
#define iteratorValid(self) (self)->_->valid(Iterator(self))
// bool iteratorNext(Iterator* self);
#define iteratorNext(self) (self)->_->next(Iterator(self))
// bool iteratorGet(Iterator* self, stvar* out);
#define iteratorGet(self, out) (self)->_->get(Iterator(self), out)


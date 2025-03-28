#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/format/formattable.h>

typedef struct FmtTestClass FmtTestClass;
typedef struct FmtTestClass_WeakRef FmtTestClass_WeakRef;
typedef struct FmtTestClass2 FmtTestClass2;
typedef struct FmtTestClass2_WeakRef FmtTestClass2_WeakRef;
saDeclarePtr(FmtTestClass);
saDeclarePtr(FmtTestClass_WeakRef);
saDeclarePtr(FmtTestClass2);
saDeclarePtr(FmtTestClass2_WeakRef);

typedef struct FmtTestClass_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*format)(_In_ void* self, FMTVar* v, string* out);
} FmtTestClass_ClassIf;
extern FmtTestClass_ClassIf FmtTestClass_ClassIf_tmpl;

typedef struct FmtTestClass2_ClassIf {
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
} FmtTestClass2_ClassIf;
extern FmtTestClass2_ClassIf FmtTestClass2_ClassIf_tmpl;

typedef struct FmtTestClass {
    union {
        FmtTestClass_ClassIf* _;
        void* _is_FmtTestClass;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int32 iv;
    string sv;
} FmtTestClass;
extern ObjClassInfo FmtTestClass_clsinfo;
#define FmtTestClass(inst) ((FmtTestClass*)(unused_noeval((inst) && &((inst)->_is_FmtTestClass)), (inst)))
#define FmtTestClassNone ((FmtTestClass*)NULL)

typedef struct FmtTestClass_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_FmtTestClass_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} FmtTestClass_WeakRef;
#define FmtTestClass_WeakRef(inst) ((FmtTestClass_WeakRef*)(unused_noeval((inst) && &((inst)->_is_FmtTestClass_WeakRef)), (inst)))

_objfactory_guaranteed FmtTestClass* FmtTestClass_create(int32 ival, string sval);
// FmtTestClass* fmttestclassCreate(int32 ival, string sval);
#define fmttestclassCreate(ival, sval) FmtTestClass_create(ival, sval)

// bool fmttestclassFormat(FmtTestClass* self, FMTVar* v, string* out);
#define fmttestclassFormat(self, v, out) (self)->_->format(FmtTestClass(self), v, out)

typedef struct FmtTestClass2 {
    union {
        FmtTestClass2_ClassIf* _;
        void* _is_FmtTestClass2;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int32 iv;
    string sv;
} FmtTestClass2;
extern ObjClassInfo FmtTestClass2_clsinfo;
#define FmtTestClass2(inst) ((FmtTestClass2*)(unused_noeval((inst) && &((inst)->_is_FmtTestClass2)), (inst)))
#define FmtTestClass2None ((FmtTestClass2*)NULL)

typedef struct FmtTestClass2_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_FmtTestClass2_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} FmtTestClass2_WeakRef;
#define FmtTestClass2_WeakRef(inst) ((FmtTestClass2_WeakRef*)(unused_noeval((inst) && &((inst)->_is_FmtTestClass2_WeakRef)), (inst)))

_objfactory_guaranteed FmtTestClass2* FmtTestClass2_create(int32 ival, string sval);
// FmtTestClass2* fmttestclass2Create(int32 ival, string sval);
#define fmttestclass2Create(ival, sval) FmtTestClass2_create(ival, sval)

// bool fmttestclass2Convert(FmtTestClass2* self, stype st, stgeneric* dest, uint32 flags);
//
// NOTE: While this is used by stConvert, the object interface is a higher level interface.
// The normal convention of blindly overwriting the destination does not apply here. For
// example, when called to convert to a string, the destination should be properly reused
// or destroyed first.
// The layer between stConvert and Convertible takes care of making sure the destination is
// always initialized.
#define fmttestclass2Convert(self, st, dest, flags) (self)->_->convert(FmtTestClass2(self), st, dest, flags)


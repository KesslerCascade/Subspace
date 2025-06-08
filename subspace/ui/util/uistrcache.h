#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"

typedef struct UIStrCache UIStrCache;
typedef struct UIStrCache_WeakRef UIStrCache_WeakRef;
saDeclarePtr(UIStrCache);
saDeclarePtr(UIStrCache_WeakRef);

typedef const char const_char;

typedef struct UIStrCache_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    strref (*add)(_In_ void* self, _In_opt_ strref str);
    const_char* (*cadd)(_In_ void* self, _In_opt_ strref str);
    strref (*trans)(_In_ void* self, _In_opt_ strref lang_key);
    const_char* (*ctrans)(_In_ void* self, _In_opt_ strref lang_key);
} UIStrCache_ClassIf;
extern UIStrCache_ClassIf UIStrCache_ClassIf_tmpl;

typedef struct UIStrCache {
    union {
        UIStrCache_ClassIf* _;
        void* _is_UIStrCache;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    hashtable cache;
} UIStrCache;
extern ObjClassInfo UIStrCache_clsinfo;
#define UIStrCache(inst) ((UIStrCache*)(unused_noeval((inst) && &((inst)->_is_UIStrCache)), (inst)))
#define UIStrCacheNone ((UIStrCache*)NULL)

typedef struct UIStrCache_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_UIStrCache_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} UIStrCache_WeakRef;
#define UIStrCache_WeakRef(inst) ((UIStrCache_WeakRef*)(unused_noeval((inst) && &((inst)->_is_UIStrCache_WeakRef)), (inst)))

_objfactory_guaranteed UIStrCache* UIStrCache_create(Subspace* ss);
// UIStrCache* uiscCreate(Subspace* ss);
#define uiscCreate(ss) UIStrCache_create(ss)

// strref uiscAdd(UIStrCache* self, strref str);
#define uiscAdd(self, str) (self)->_->add(UIStrCache(self), str)
// const_char* uiscCadd(UIStrCache* self, strref str);
#define uiscCadd(self, str) (self)->_->cadd(UIStrCache(self), str)
// strref uiscTrans(UIStrCache* self, strref lang_key);
#define uiscTrans(self, lang_key) (self)->_->trans(UIStrCache(self), lang_key)
// const_char* uiscCtrans(UIStrCache* self, strref lang_key);
#define uiscCtrans(self, lang_key) (self)->_->ctrans(UIStrCache(self), lang_key)


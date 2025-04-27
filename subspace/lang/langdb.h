#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/serialize.h>

typedef struct LanguageDB LanguageDB;
typedef struct LanguageDB_WeakRef LanguageDB_WeakRef;
saDeclarePtr(LanguageDB);
saDeclarePtr(LanguageDB_WeakRef);

typedef struct LanguageDB_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // streambuffer must be in pull mode
    bool (*load)(_In_ void* self, StreamBuffer* sb, bool fallback);
    strref (*get)(_In_ void* self, _In_opt_ strref key);
} LanguageDB_ClassIf;
extern LanguageDB_ClassIf LanguageDB_ClassIf_tmpl;

typedef struct LanguageDB {
    union {
        LanguageDB_ClassIf* _;
        void* _is_LanguageDB;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    hashtable primary;
    hashtable fallback;
} LanguageDB;
extern ObjClassInfo LanguageDB_clsinfo;
#define LanguageDB(inst) ((LanguageDB*)(unused_noeval((inst) && &((inst)->_is_LanguageDB)), (inst)))
#define LanguageDBNone ((LanguageDB*)NULL)

typedef struct LanguageDB_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LanguageDB_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LanguageDB_WeakRef;
#define LanguageDB_WeakRef(inst) ((LanguageDB_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LanguageDB_WeakRef)), (inst)))

_objfactory_guaranteed LanguageDB* LanguageDB_create();
// LanguageDB* languagedbCreate();
#define languagedbCreate() LanguageDB_create()

// bool languagedbLoad(LanguageDB* self, StreamBuffer* sb, bool fallback);
//
// streambuffer must be in pull mode
#define languagedbLoad(self, sb, fallback) (self)->_->load(LanguageDB(self), sb, fallback)
// strref languagedbGet(LanguageDB* self, strref key);
#define languagedbGet(self, key) (self)->_->get(LanguageDB(self), key)


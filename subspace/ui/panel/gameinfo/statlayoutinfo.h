#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>

typedef struct StatLayoutInfo StatLayoutInfo;
typedef struct StatLayoutInfo_WeakRef StatLayoutInfo_WeakRef;
saDeclarePtr(StatLayoutInfo);
saDeclarePtr(StatLayoutInfo_WeakRef);

typedef struct StatLayoutInfo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*update)(_In_ void* self, Ihandle* mtx);
} StatLayoutInfo_ClassIf;
extern StatLayoutInfo_ClassIf StatLayoutInfo_ClassIf_tmpl;

typedef struct StatLayoutInfo {
    union {
        StatLayoutInfo_ClassIf* _;
        void* _is_StatLayoutInfo;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string title;
    string value;
    int col;
    int lin;
    bool span;
} StatLayoutInfo;
extern ObjClassInfo StatLayoutInfo_clsinfo;
#define StatLayoutInfo(inst) ((StatLayoutInfo*)(unused_noeval((inst) && &((inst)->_is_StatLayoutInfo)), (inst)))
#define StatLayoutInfoNone ((StatLayoutInfo*)NULL)

typedef struct StatLayoutInfo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_StatLayoutInfo_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} StatLayoutInfo_WeakRef;
#define StatLayoutInfo_WeakRef(inst) ((StatLayoutInfo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_StatLayoutInfo_WeakRef)), (inst)))

_objfactory_guaranteed StatLayoutInfo* StatLayoutInfo_create(_In_opt_ strref title, _In_opt_ strref value, bool span);
// StatLayoutInfo* statlayoutinfoCreate(strref title, strref value, bool span);
#define statlayoutinfoCreate(title, value, span) StatLayoutInfo_create(title, value, span)

// void statlayoutinfoUpdate(StatLayoutInfo* self, Ihandle* mtx);
#define statlayoutinfoUpdate(self, mtx) (self)->_->update(StatLayoutInfo(self), mtx)


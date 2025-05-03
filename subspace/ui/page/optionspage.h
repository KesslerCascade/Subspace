#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"
#include "lang/lang.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct OptionsPage OptionsPage;
typedef struct OptionsPage_WeakRef OptionsPage_WeakRef;
saDeclarePtr(OptionsPage);
saDeclarePtr(OptionsPage_WeakRef);

typedef struct OptionsPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} OptionsPage_ClassIf;
extern OptionsPage_ClassIf OptionsPage_ClassIf_tmpl;

typedef struct OptionsPage {
    union {
        OptionsPage_ClassIf* _;
        void* _is_OptionsPage;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string imgname;
    string title;
} OptionsPage;
extern ObjClassInfo OptionsPage_clsinfo;
#define OptionsPage(inst) ((OptionsPage*)(unused_noeval((inst) && &((inst)->_is_OptionsPage)), (inst)))
#define OptionsPageNone ((OptionsPage*)NULL)

typedef struct OptionsPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_OptionsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} OptionsPage_WeakRef;
#define OptionsPage_WeakRef(inst) ((OptionsPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_OptionsPage_WeakRef)), (inst)))

// bool optionspageMake(OptionsPage* self, Ihandle* list);
#define optionspageMake(self, list) (self)->_->make(OptionsPage(self), list)
// bool optionspageUpdate(OptionsPage* self);
#define optionspageUpdate(self) (self)->_->update(OptionsPage(self))


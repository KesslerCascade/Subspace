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
typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
saDeclarePtr(SettingsPage);
saDeclarePtr(SettingsPage_WeakRef);

typedef struct SettingsPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} SettingsPage_ClassIf;
extern SettingsPage_ClassIf SettingsPage_ClassIf_tmpl;

typedef struct SettingsPage {
    union {
        SettingsPage_ClassIf* _;
        void* _is_SettingsPage;
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
    bool visible;
} SettingsPage;
extern ObjClassInfo SettingsPage_clsinfo;
#define SettingsPage(inst) ((SettingsPage*)(unused_noeval((inst) && &((inst)->_is_SettingsPage)), (inst)))
#define SettingsPageNone ((SettingsPage*)NULL)

typedef struct SettingsPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SettingsPage_WeakRef;
#define SettingsPage_WeakRef(inst) ((SettingsPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SettingsPage_WeakRef)), (inst)))

// bool settingspageMake(SettingsPage* self, Ihandle* list);
#define settingspageMake(self, list) (self)->_->make(SettingsPage(self), list)
// bool settingspageUpdate(SettingsPage* self);
#define settingspageUpdate(self) (self)->_->update(SettingsPage(self))


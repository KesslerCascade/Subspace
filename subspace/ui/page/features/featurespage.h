#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/page/settingspage.h"
#include "gamemgr/gameinst.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct FeaturesPage FeaturesPage;
typedef struct FeaturesPage_WeakRef FeaturesPage_WeakRef;
saDeclarePtr(FeaturesPage);
saDeclarePtr(FeaturesPage_WeakRef);

typedef struct FeaturesPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} FeaturesPage_ClassIf;
extern FeaturesPage_ClassIf FeaturesPage_ClassIf_tmpl;

typedef struct FeaturesPage {
    union {
        FeaturesPage_ClassIf* _;
        void* _is_FeaturesPage;
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
    sa_Ihandle featpanels;
} FeaturesPage;
extern ObjClassInfo FeaturesPage_clsinfo;
#define FeaturesPage(inst) ((FeaturesPage*)(unused_noeval((inst) && &((inst)->_is_FeaturesPage)), (inst)))
#define FeaturesPageNone ((FeaturesPage*)NULL)

typedef struct FeaturesPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_FeaturesPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} FeaturesPage_WeakRef;
#define FeaturesPage_WeakRef(inst) ((FeaturesPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_FeaturesPage_WeakRef)), (inst)))

_objfactory_guaranteed FeaturesPage* FeaturesPage_create(SubspaceUI* ui);
// FeaturesPage* featurespageCreate(SubspaceUI* ui);
#define featurespageCreate(ui) FeaturesPage_create(SubspaceUI(ui))

// bool featurespageMake(FeaturesPage* self, Ihandle* list);
#define featurespageMake(self, list) (self)->_->make(FeaturesPage(self), list)
// bool featurespageUpdate(FeaturesPage* self);
#define featurespageUpdate(self) (self)->_->update(FeaturesPage(self))


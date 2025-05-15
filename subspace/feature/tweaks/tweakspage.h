#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/page/settingspage.h"
#include "gamemgr/gameinst.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct Tweaks Tweaks;
typedef struct Tweaks_WeakRef Tweaks_WeakRef;
typedef struct TweaksPage TweaksPage;
typedef struct TweaksPage_WeakRef TweaksPage_WeakRef;
saDeclarePtr(TweaksPage);
saDeclarePtr(TweaksPage_WeakRef);

typedef struct TweaksPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} TweaksPage_ClassIf;
extern TweaksPage_ClassIf TweaksPage_ClassIf_tmpl;

typedef struct TweaksPage {
    union {
        TweaksPage_ClassIf* _;
        void* _is_TweaksPage;
        void* _is_SettingsPage;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Weak(SubspaceFeature)* owner;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string imgname;
    string title;
    Tweaks* feature;
    string fpscustomtext;
    Ihandle* fpsradio;
    Ihandle* fps30;
    Ihandle* fps60;
    Ihandle* fps120;
    Ihandle* fpscustom;
    Ihandle* fpscustomval;
} TweaksPage;
extern ObjClassInfo TweaksPage_clsinfo;
#define TweaksPage(inst) ((TweaksPage*)(unused_noeval((inst) && &((inst)->_is_TweaksPage)), (inst)))
#define TweaksPageNone ((TweaksPage*)NULL)

typedef struct TweaksPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TweaksPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TweaksPage_WeakRef;
#define TweaksPage_WeakRef(inst) ((TweaksPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TweaksPage_WeakRef)), (inst)))

_objfactory_guaranteed TweaksPage* TweaksPage_create(Tweaks* feature, SubspaceUI* ui);
// TweaksPage* tweakspageCreate(Tweaks* feature, SubspaceUI* ui);
#define tweakspageCreate(feature, ui) TweaksPage_create(Tweaks(feature), SubspaceUI(ui))

// bool tweakspageMake(TweaksPage* self, Ihandle* list);
#define tweakspageMake(self, list) (self)->_->make(TweaksPage(self), list)
// bool tweakspageUpdate(TweaksPage* self);
#define tweakspageUpdate(self) (self)->_->update(TweaksPage(self))


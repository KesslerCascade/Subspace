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
typedef struct TimeWarp TimeWarp;
typedef struct TimeWarp_WeakRef TimeWarp_WeakRef;
typedef struct TimeWarpPage TimeWarpPage;
typedef struct TimeWarpPage_WeakRef TimeWarpPage_WeakRef;
saDeclarePtr(TimeWarpPage);
saDeclarePtr(TimeWarpPage_WeakRef);

typedef struct TimeWarpPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} TimeWarpPage_ClassIf;
extern TimeWarpPage_ClassIf TimeWarpPage_ClassIf_tmpl;

typedef struct TimeWarpPage {
    union {
        TimeWarpPage_ClassIf* _;
        void* _is_TimeWarpPage;
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
    TimeWarp* feature;
    Ihandle* maxwarp_val_num;
    Ihandle* maxwarp_val;
    Ihandle* doubletap_val_num;
    Ihandle* doubletap_val;
    Ihandle* allowslowmo;
    string maxwarptext;
    string doubletaptext;
} TimeWarpPage;
extern ObjClassInfo TimeWarpPage_clsinfo;
#define TimeWarpPage(inst) ((TimeWarpPage*)(unused_noeval((inst) && &((inst)->_is_TimeWarpPage)), (inst)))
#define TimeWarpPageNone ((TimeWarpPage*)NULL)

typedef struct TimeWarpPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TimeWarpPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TimeWarpPage_WeakRef;
#define TimeWarpPage_WeakRef(inst) ((TimeWarpPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TimeWarpPage_WeakRef)), (inst)))

_objfactory_guaranteed TimeWarpPage* TimeWarpPage_create(TimeWarp* feature, SubspaceUI* ui);
// TimeWarpPage* timewarppageCreate(TimeWarp* feature, SubspaceUI* ui);
#define timewarppageCreate(feature, ui) TimeWarpPage_create(TimeWarp(feature), SubspaceUI(ui))

// bool timewarppageMake(TimeWarpPage* self, Ihandle* list);
#define timewarppageMake(self, list) (self)->_->make(TimeWarpPage(self), list)
// bool timewarppageUpdate(TimeWarpPage* self);
#define timewarppageUpdate(self) (self)->_->update(TimeWarpPage(self))


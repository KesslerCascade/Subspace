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
typedef struct InfoBlock InfoBlock;
typedef struct InfoBlock_WeakRef InfoBlock_WeakRef;
typedef struct InfoBlockPage InfoBlockPage;
typedef struct InfoBlockPage_WeakRef InfoBlockPage_WeakRef;
saDeclarePtr(InfoBlockPage);
saDeclarePtr(InfoBlockPage_WeakRef);

typedef struct InfoBlockPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} InfoBlockPage_ClassIf;
extern InfoBlockPage_ClassIf InfoBlockPage_ClassIf_tmpl;

typedef struct InfoBlockPage {
    union {
        InfoBlockPage_ClassIf* _;
        void* _is_InfoBlockPage;
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
    InfoBlock* feature;
    sa_string order;
    sa_string orderdisabled;
    Ihandle* enabledlist;
    Ihandle* disabledlist;
    Ihandle* addbtn;
    Ihandle* upbtn;
    Ihandle* downbtn;
    Ihandle* removebtn;
} InfoBlockPage;
extern ObjClassInfo InfoBlockPage_clsinfo;
#define InfoBlockPage(inst) ((InfoBlockPage*)(unused_noeval((inst) && &((inst)->_is_InfoBlockPage)), (inst)))
#define InfoBlockPageNone ((InfoBlockPage*)NULL)

typedef struct InfoBlockPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_InfoBlockPage_WeakRef;
        void* _is_SettingsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} InfoBlockPage_WeakRef;
#define InfoBlockPage_WeakRef(inst) ((InfoBlockPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_InfoBlockPage_WeakRef)), (inst)))

_objfactory_guaranteed InfoBlockPage* InfoBlockPage_create(InfoBlock* feature, SubspaceUI* ui);
// InfoBlockPage* infoblockpageCreate(InfoBlock* feature, SubspaceUI* ui);
#define infoblockpageCreate(feature, ui) InfoBlockPage_create(InfoBlock(feature), SubspaceUI(ui))

void InfoBlockPage_save(_In_ InfoBlockPage* self);
// void infoblockpageSave(InfoBlockPage* self);
#define infoblockpageSave(self) InfoBlockPage_save(InfoBlockPage(self))

// bool infoblockpageMake(InfoBlockPage* self, Ihandle* list);
#define infoblockpageMake(self, list) (self)->_->make(InfoBlockPage(self), list)
// bool infoblockpageUpdate(InfoBlockPage* self);
#define infoblockpageUpdate(self) (self)->_->update(InfoBlockPage(self))


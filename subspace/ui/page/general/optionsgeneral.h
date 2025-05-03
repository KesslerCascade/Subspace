#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/page/optionspage.h"
#include "gamemgr/gameinst.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct GeneralPage GeneralPage;
typedef struct GeneralPage_WeakRef GeneralPage_WeakRef;
saDeclarePtr(GeneralPage);
saDeclarePtr(GeneralPage_WeakRef);

typedef struct GeneralPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} GeneralPage_ClassIf;
extern GeneralPage_ClassIf GeneralPage_ClassIf_tmpl;

typedef struct GeneralPage {
    union {
        GeneralPage_ClassIf* _;
        void* _is_GeneralPage;
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
    sa_string langids;
    sa_string langnames;
    string compatimg;
    string verstr;
    Ihandle* langselect;
    Ihandle* ftlloctext;
    Ihandle* ftlcompatimg;
    Ihandle* ftlcompatlabel;
    Ihandle* ftlver;
    GameInst* validateinst;
    bool vpending;        // validated instance should be saved to settings
} GeneralPage;
extern ObjClassInfo GeneralPage_clsinfo;
#define GeneralPage(inst) ((GeneralPage*)(unused_noeval((inst) && &((inst)->_is_GeneralPage)), (inst)))
#define GeneralPageNone ((GeneralPage*)NULL)

typedef struct GeneralPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GeneralPage_WeakRef;
        void* _is_OptionsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GeneralPage_WeakRef;
#define GeneralPage_WeakRef(inst) ((GeneralPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GeneralPage_WeakRef)), (inst)))

_objfactory_guaranteed GeneralPage* GeneralPage_create(SubspaceUI* ui);
// GeneralPage* generalpageCreate(SubspaceUI* ui);
#define generalpageCreate(ui) GeneralPage_create(SubspaceUI(ui))

// bool generalpageMake(GeneralPage* self, Ihandle* list);
#define generalpageMake(self, list) (self)->_->make(GeneralPage(self), list)
// bool generalpageUpdate(GeneralPage* self);
#define generalpageUpdate(self) (self)->_->update(GeneralPage(self))


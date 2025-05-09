#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/page/optionspage.h"
#include "gamemgr/gameinst.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct OptionsPage OptionsPage;
typedef struct OptionsPage_WeakRef OptionsPage_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SetupPage SetupPage;
typedef struct SetupPage_WeakRef SetupPage_WeakRef;
saDeclarePtr(SetupPage);
saDeclarePtr(SetupPage_WeakRef);

typedef struct SetupPage_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self, Ihandle* list);
    bool (*update)(_In_ void* self);
} SetupPage_ClassIf;
extern SetupPage_ClassIf SetupPage_ClassIf_tmpl;

typedef struct SetupPage {
    union {
        SetupPage_ClassIf* _;
        void* _is_SetupPage;
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
    bool visible;
    string featurereq;        // requires this feature to be enabled to be visible
    sa_string langids;
    sa_string langnames;
    string compatimg;
    string verstr;
    string overrideloc;
    Ihandle* langselect;
    Ihandle* ftlloctext;
    Ihandle* ftlcompatimg;
    Ihandle* ftlcompatlabel;
    Ihandle* ftlver;
    Ihandle* saveoverridecheck;
    Ihandle* saveoverrideusercheck;
    Ihandle* saveoverridetext;
    Ihandle* saveoverridehbox;
    GameInst* validateinst;
    bool vpending;        // validated instance should be saved to settings
} SetupPage;
extern ObjClassInfo SetupPage_clsinfo;
#define SetupPage(inst) ((SetupPage*)(unused_noeval((inst) && &((inst)->_is_SetupPage)), (inst)))
#define SetupPageNone ((SetupPage*)NULL)

typedef struct SetupPage_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SetupPage_WeakRef;
        void* _is_OptionsPage_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SetupPage_WeakRef;
#define SetupPage_WeakRef(inst) ((SetupPage_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SetupPage_WeakRef)), (inst)))

_objfactory_guaranteed SetupPage* SetupPage_create(SubspaceUI* ui);
// SetupPage* setuppageCreate(SubspaceUI* ui);
#define setuppageCreate(ui) SetupPage_create(SubspaceUI(ui))

// bool setuppageMake(SetupPage* self, Ihandle* list);
#define setuppageMake(self, list) (self)->_->make(SetupPage(self), list)
// bool setuppageUpdate(SetupPage* self);
#define setuppageUpdate(self) (self)->_->update(SetupPage(self))


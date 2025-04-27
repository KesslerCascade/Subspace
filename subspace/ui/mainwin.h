#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct MainWin MainWin;
typedef struct MainWin_WeakRef MainWin_WeakRef;
saDeclarePtr(MainWin);
saDeclarePtr(MainWin_WeakRef);

typedef struct MainWin_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*show)(_In_ void* self);
    void (*update)(_In_ void* self);
} MainWin_ClassIf;
extern MainWin_ClassIf MainWin_ClassIf_tmpl;

typedef struct MainWin {
    union {
        MainWin_ClassIf* _;
        void* _is_MainWin;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(SubspaceUI)* ui;
    Ihandle* win;
    uint32 activeInst;        // cookie of game instance that's being tracked through the UI
} MainWin;
extern ObjClassInfo MainWin_clsinfo;
#define MainWin(inst) ((MainWin*)(unused_noeval((inst) && &((inst)->_is_MainWin)), (inst)))
#define MainWinNone ((MainWin*)NULL)

typedef struct MainWin_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_MainWin_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} MainWin_WeakRef;
#define MainWin_WeakRef(inst) ((MainWin_WeakRef*)(unused_noeval((inst) && &((inst)->_is_MainWin_WeakRef)), (inst)))

_objfactory_guaranteed MainWin* MainWin_create(SubspaceUI* ui);
// MainWin* mainwinCreate(SubspaceUI* ui);
#define mainwinCreate(ui) MainWin_create(SubspaceUI(ui))

int MainWin_onClose(Ihandle* iupself);
// int mainwinOnClose(Ihandle* iupself);
#define mainwinOnClose(iupself) MainWin_onClose(iupself)

// void mainwinShow(MainWin* self);
#define mainwinShow(self) (self)->_->show(MainWin(self))
// void mainwinUpdate(MainWin* self);
#define mainwinUpdate(self) (self)->_->update(MainWin(self))


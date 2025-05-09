#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <iup.h>
#include "subspace.h"
#include "ui/page/optionspage.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct OptionsWin OptionsWin;
typedef struct OptionsWin_WeakRef OptionsWin_WeakRef;
saDeclarePtr(OptionsWin);
saDeclarePtr(OptionsWin_WeakRef);

#define OPTIONSWIN_WIDTH 500
#define OPTIONSWIN_HEIGHT 400

typedef struct OptionsWin_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*showPage)(_In_ void* self, int num);
    void (*showPageByList)(_In_ void* self, int listid);
    bool (*make)(_In_ void* self);
    void (*show)(_In_ void* self);
    void (*updateList)(_In_ void* self);
    bool (*updatePage)(_In_ void* self, _In_opt_ strref name);
    void (*updateAll)(_In_ void* self);
    void (*finish)(_In_ void* self);
} OptionsWin_ClassIf;
extern OptionsWin_ClassIf OptionsWin_ClassIf_tmpl;

typedef struct OptionsWin {
    union {
        OptionsWin_ClassIf* _;
        void* _is_OptionsWin;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    SubspaceUI* ui;
    Ihandle* win;
    Ihandle* pagelist;
    Ihandle* pagezbox;
    sa_OptionsPage pages;
    sa_int32 zboxmap;
} OptionsWin;
extern ObjClassInfo OptionsWin_clsinfo;
#define OptionsWin(inst) ((OptionsWin*)(unused_noeval((inst) && &((inst)->_is_OptionsWin)), (inst)))
#define OptionsWinNone ((OptionsWin*)NULL)

typedef struct OptionsWin_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_OptionsWin_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} OptionsWin_WeakRef;
#define OptionsWin_WeakRef(inst) ((OptionsWin_WeakRef*)(unused_noeval((inst) && &((inst)->_is_OptionsWin_WeakRef)), (inst)))

_objfactory_guaranteed OptionsWin* OptionsWin_create(SubspaceUI* ui);
// OptionsWin* optionswinCreate(SubspaceUI* ui);
#define optionswinCreate(ui) OptionsWin_create(SubspaceUI(ui))

int OptionsWin_onClose(Ihandle* ih);
// int optionswinOnClose(Ihandle* ih);
#define optionswinOnClose(ih) OptionsWin_onClose(ih)

// void optionswinShowPage(OptionsWin* self, int num);
#define optionswinShowPage(self, num) (self)->_->showPage(OptionsWin(self), num)
// void optionswinShowPageByList(OptionsWin* self, int listid);
#define optionswinShowPageByList(self, listid) (self)->_->showPageByList(OptionsWin(self), listid)
// bool optionswinMake(OptionsWin* self);
#define optionswinMake(self) (self)->_->make(OptionsWin(self))
// void optionswinShow(OptionsWin* self);
#define optionswinShow(self) (self)->_->show(OptionsWin(self))
// void optionswinUpdateList(OptionsWin* self);
#define optionswinUpdateList(self) (self)->_->updateList(OptionsWin(self))
// bool optionswinUpdatePage(OptionsWin* self, strref name);
#define optionswinUpdatePage(self, name) (self)->_->updatePage(OptionsWin(self), name)
// void optionswinUpdateAll(OptionsWin* self);
#define optionswinUpdateAll(self) (self)->_->updateAll(OptionsWin(self))
// void optionswinFinish(OptionsWin* self);
#define optionswinFinish(self) (self)->_->finish(OptionsWin(self))


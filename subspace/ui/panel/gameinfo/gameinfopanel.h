#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct GameInfoPanel GameInfoPanel;
typedef struct GameInfoPanel_WeakRef GameInfoPanel_WeakRef;
saDeclarePtr(GameInfoPanel);
saDeclarePtr(GameInfoPanel_WeakRef);

typedef struct GameInfoPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} GameInfoPanel_ClassIf;
extern GameInfoPanel_ClassIf GameInfoPanel_ClassIf_tmpl;

typedef struct GameInfoPanel {
    union {
        GameInfoPanel_ClassIf* _;
        void* _is_GameInfoPanel;
        void* _is_Panel;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(SubspaceUI)* ui;
    Ihandle* h;
    Ihandle* parent;
    strref name;
    string title;
} GameInfoPanel;
extern ObjClassInfo GameInfoPanel_clsinfo;
#define GameInfoPanel(inst) ((GameInfoPanel*)(unused_noeval((inst) && &((inst)->_is_GameInfoPanel)), (inst)))
#define GameInfoPanelNone ((GameInfoPanel*)NULL)

typedef struct GameInfoPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameInfoPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameInfoPanel_WeakRef;
#define GameInfoPanel_WeakRef(inst) ((GameInfoPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameInfoPanel_WeakRef)), (inst)))

_objfactory_guaranteed GameInfoPanel* GameInfoPanel_create(SubspaceUI* ui);
// GameInfoPanel* gameinfopanelCreate(SubspaceUI* ui);
#define gameinfopanelCreate(ui) GameInfoPanel_create(SubspaceUI(ui))

// bool gameinfopanelMake(GameInfoPanel* self);
#define gameinfopanelMake(self) (self)->_->make(GameInfoPanel(self))
// bool gameinfopanelUpdate(GameInfoPanel* self);
#define gameinfopanelUpdate(self) (self)->_->update(GameInfoPanel(self))
// intptr gameinfopanelCmp(GameInfoPanel* self, GameInfoPanel* other, uint32 flags);
#define gameinfopanelCmp(self, other, flags) (self)->_->cmp(GameInfoPanel(self), other, flags)


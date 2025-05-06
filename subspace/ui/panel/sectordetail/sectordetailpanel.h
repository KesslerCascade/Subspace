#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct SectorDetailPanel SectorDetailPanel;
typedef struct SectorDetailPanel_WeakRef SectorDetailPanel_WeakRef;
saDeclarePtr(SectorDetailPanel);
saDeclarePtr(SectorDetailPanel_WeakRef);

typedef struct SectorDetailPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} SectorDetailPanel_ClassIf;
extern SectorDetailPanel_ClassIf SectorDetailPanel_ClassIf_tmpl;

typedef struct SectorDetailPanel {
    union {
        SectorDetailPanel_ClassIf* _;
        void* _is_SectorDetailPanel;
        void* _is_Panel;
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
    string title;
} SectorDetailPanel;
extern ObjClassInfo SectorDetailPanel_clsinfo;
#define SectorDetailPanel(inst) ((SectorDetailPanel*)(unused_noeval((inst) && &((inst)->_is_SectorDetailPanel)), (inst)))
#define SectorDetailPanelNone ((SectorDetailPanel*)NULL)

typedef struct SectorDetailPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SectorDetailPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SectorDetailPanel_WeakRef;
#define SectorDetailPanel_WeakRef(inst) ((SectorDetailPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SectorDetailPanel_WeakRef)), (inst)))

_objfactory_guaranteed SectorDetailPanel* SectorDetailPanel_create(SubspaceUI* ui);
// SectorDetailPanel* sectordetailpanelCreate(SubspaceUI* ui);
#define sectordetailpanelCreate(ui) SectorDetailPanel_create(SubspaceUI(ui))

// bool sectordetailpanelMake(SectorDetailPanel* self);
#define sectordetailpanelMake(self) (self)->_->make(SectorDetailPanel(self))
// bool sectordetailpanelUpdate(SectorDetailPanel* self);
#define sectordetailpanelUpdate(self) (self)->_->update(SectorDetailPanel(self))
// intptr sectordetailpanelCmp(SectorDetailPanel* self, SectorDetailPanel* other, uint32 flags);
#define sectordetailpanelCmp(self, other, flags) (self)->_->cmp(SectorDetailPanel(self), other, flags)


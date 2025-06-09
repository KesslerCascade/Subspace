#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "ui/panel/panel.h"

typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct ScrapGraphPanel ScrapGraphPanel;
typedef struct ScrapGraphPanel_WeakRef ScrapGraphPanel_WeakRef;
saDeclarePtr(ScrapGraphPanel);
saDeclarePtr(ScrapGraphPanel_WeakRef);

typedef struct ScrapGraphPanel_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*make)(_In_ void* self);
    bool (*update)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} ScrapGraphPanel_ClassIf;
extern ScrapGraphPanel_ClassIf ScrapGraphPanel_ClassIf_tmpl;

typedef struct ScrapGraphPanel {
    union {
        ScrapGraphPanel_ClassIf* _;
        void* _is_ScrapGraphPanel;
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
} ScrapGraphPanel;
extern ObjClassInfo ScrapGraphPanel_clsinfo;
#define ScrapGraphPanel(inst) ((ScrapGraphPanel*)(unused_noeval((inst) && &((inst)->_is_ScrapGraphPanel)), (inst)))
#define ScrapGraphPanelNone ((ScrapGraphPanel*)NULL)

typedef struct ScrapGraphPanel_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ScrapGraphPanel_WeakRef;
        void* _is_Panel_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ScrapGraphPanel_WeakRef;
#define ScrapGraphPanel_WeakRef(inst) ((ScrapGraphPanel_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ScrapGraphPanel_WeakRef)), (inst)))

_objfactory_guaranteed ScrapGraphPanel* ScrapGraphPanel_create(SubspaceUI* ui);
// ScrapGraphPanel* scrapgraphpanelCreate(SubspaceUI* ui);
#define scrapgraphpanelCreate(ui) ScrapGraphPanel_create(SubspaceUI(ui))

// bool scrapgraphpanelMake(ScrapGraphPanel* self);
#define scrapgraphpanelMake(self) (self)->_->make(ScrapGraphPanel(self))
// bool scrapgraphpanelUpdate(ScrapGraphPanel* self);
#define scrapgraphpanelUpdate(self) (self)->_->update(ScrapGraphPanel(self))
// intptr scrapgraphpanelCmp(ScrapGraphPanel* self, ScrapGraphPanel* other, uint32 flags);
#define scrapgraphpanelCmp(self, other, flags) (self)->_->cmp(ScrapGraphPanel(self), other, flags)


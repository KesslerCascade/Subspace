#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "keybind.h"
#include "control/controlclient.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct KBMgr KBMgr;
typedef struct KBMgr_WeakRef KBMgr_WeakRef;
saDeclarePtr(KBMgr);
saDeclarePtr(KBMgr_WeakRef);

typedef struct KBMgr_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*bind)(_In_ void* self, _In_opt_ strref name, int key);
    void (*reg)(_In_ void* self, SubspaceFeature* owner, _In_opt_ strref name, int default_key);
    void (*sendAll)(_In_ void* self, ControlClient* client);
} KBMgr_ClassIf;
extern KBMgr_ClassIf KBMgr_ClassIf_tmpl;

typedef struct KBMgr {
    union {
        KBMgr_ClassIf* _;
        void* _is_KBMgr;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    RWLock kbmgrlock;
    hashtable binds;
} KBMgr;
extern ObjClassInfo KBMgr_clsinfo;
#define KBMgr(inst) ((KBMgr*)(unused_noeval((inst) && &((inst)->_is_KBMgr)), (inst)))
#define KBMgrNone ((KBMgr*)NULL)

typedef struct KBMgr_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_KBMgr_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} KBMgr_WeakRef;
#define KBMgr_WeakRef(inst) ((KBMgr_WeakRef*)(unused_noeval((inst) && &((inst)->_is_KBMgr_WeakRef)), (inst)))

_objfactory_guaranteed KBMgr* KBMgr_create(Subspace* subspace);
// KBMgr* kbmgrCreate(Subspace* subspace);
#define kbmgrCreate(subspace) KBMgr_create(subspace)

// bool kbmgrBind(KBMgr* self, strref name, int key);
#define kbmgrBind(self, name, key) (self)->_->bind(KBMgr(self), name, key)
// void kbmgrReg(KBMgr* self, SubspaceFeature* owner, strref name, int default_key);
#define kbmgrReg(self, owner, name, default_key) (self)->_->reg(KBMgr(self), SubspaceFeature(owner), name, default_key)
// void kbmgrSendAll(KBMgr* self, ControlClient* client);
#define kbmgrSendAll(self, client) (self)->_->sendAll(KBMgr(self), ControlClient(client))


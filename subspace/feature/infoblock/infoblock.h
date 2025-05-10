#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature/feature.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct InfoBlock InfoBlock;
typedef struct InfoBlock_WeakRef InfoBlock_WeakRef;
saDeclarePtr(InfoBlock);
saDeclarePtr(InfoBlock_WeakRef);

typedef struct InfoBlock_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SettingsPage* (*getSettingsPage)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
    void (*loadSettings)(_In_ void* self);
    void (*sendSetting)(_In_ void* self, ControlClient* client, _In_opt_ strref name);
    void (*sendAllSettings)(_In_ void* self, ControlClient* client);
} InfoBlock_ClassIf;
extern InfoBlock_ClassIf InfoBlock_ClassIf_tmpl;

typedef struct InfoBlock {
    union {
        InfoBlock_ClassIf* _;
        void* _is_InfoBlock;
        void* _is_SubspaceFeature;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    RWLock lock;
    string name;
    bool available;
    bool enabled;
    bool optional;        // Features that are expected to be unavailable, e.g. version-specific
    hashtable settings;        // Settings that are synchronized with the game client
} InfoBlock;
extern ObjClassInfo InfoBlock_clsinfo;
#define InfoBlock(inst) ((InfoBlock*)(unused_noeval((inst) && &((inst)->_is_InfoBlock)), (inst)))
#define InfoBlockNone ((InfoBlock*)NULL)

typedef struct InfoBlock_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_InfoBlock_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} InfoBlock_WeakRef;
#define InfoBlock_WeakRef(inst) ((InfoBlock_WeakRef*)(unused_noeval((inst) && &((inst)->_is_InfoBlock_WeakRef)), (inst)))

_objfactory_guaranteed InfoBlock* InfoBlock_create(Subspace* ss);
// InfoBlock* infoblockCreate(Subspace* ss);
#define infoblockCreate(ss) InfoBlock_create(ss)

// SettingsPage* infoblockGetSettingsPage(InfoBlock* self);
#define infoblockGetSettingsPage(self) (self)->_->getSettingsPage(InfoBlock(self))
// void infoblockEnable(InfoBlock* self, bool enabled);
#define infoblockEnable(self, enabled) (self)->_->enable(InfoBlock(self), enabled)
// void infoblockLoadSettings(InfoBlock* self);
#define infoblockLoadSettings(self) (self)->_->loadSettings(InfoBlock(self))
// void infoblockSendSetting(InfoBlock* self, ControlClient* client, strref name);
#define infoblockSendSetting(self, client, name) (self)->_->sendSetting(InfoBlock(self), ControlClient(client), name)
// void infoblockSendAllSettings(InfoBlock* self, ControlClient* client);
#define infoblockSendAllSettings(self, client) (self)->_->sendAllSettings(InfoBlock(self), ControlClient(client))


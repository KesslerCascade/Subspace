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
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct InfoBlock InfoBlock;
typedef struct InfoBlock_WeakRef InfoBlock_WeakRef;
saDeclarePtr(InfoBlock);
saDeclarePtr(InfoBlock_WeakRef);

typedef struct InfoBlock_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SettingsPage* (*getSettingsPage)(_In_ void* self);
    SettingsPage* (*createSettingsPage)(_In_ void* self, SubspaceUI* ui);
    bool (*isEnabled)(_In_ void* self);
    bool (*isAvailable)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
    void (*setAvailable)(_In_ void* self, bool available);
    void (*applyDefaultSettings)(_In_ void* self);
    void (*sendSetting)(_In_ void* self, ControlClient* client, _In_opt_ strref name);
    void (*sendAllSettings)(_In_ void* self, ControlClient* client);
    // update curinst when a settings changes
    void (*sendSettingCur)(_In_ void* self, _In_opt_ strref name);
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
    SSDNode* settings;        // Settings that are synchronized with the game client
    SettingsPage* page;
    bool pagecreated;
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
// SettingsPage* infoblockCreateSettingsPage(InfoBlock* self, SubspaceUI* ui);
#define infoblockCreateSettingsPage(self, ui) (self)->_->createSettingsPage(InfoBlock(self), SubspaceUI(ui))
// bool infoblockIsEnabled(InfoBlock* self);
#define infoblockIsEnabled(self) (self)->_->isEnabled(InfoBlock(self))
// bool infoblockIsAvailable(InfoBlock* self);
#define infoblockIsAvailable(self) (self)->_->isAvailable(InfoBlock(self))
// void infoblockEnable(InfoBlock* self, bool enabled);
#define infoblockEnable(self, enabled) (self)->_->enable(InfoBlock(self), enabled)
// void infoblockSetAvailable(InfoBlock* self, bool available);
#define infoblockSetAvailable(self, available) (self)->_->setAvailable(InfoBlock(self), available)
// void infoblockApplyDefaultSettings(InfoBlock* self);
#define infoblockApplyDefaultSettings(self) (self)->_->applyDefaultSettings(InfoBlock(self))
// void infoblockSendSetting(InfoBlock* self, ControlClient* client, strref name);
#define infoblockSendSetting(self, client, name) (self)->_->sendSetting(InfoBlock(self), ControlClient(client), name)
// void infoblockSendAllSettings(InfoBlock* self, ControlClient* client);
#define infoblockSendAllSettings(self, client) (self)->_->sendAllSettings(InfoBlock(self), ControlClient(client))
// void infoblockSendSettingCur(InfoBlock* self, strref name);
//
// update curinst when a settings changes
#define infoblockSendSettingCur(self, name) (self)->_->sendSettingCur(InfoBlock(self), name)


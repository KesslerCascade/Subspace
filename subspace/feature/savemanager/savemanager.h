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
typedef struct SaveManager SaveManager;
typedef struct SaveManager_WeakRef SaveManager_WeakRef;
saDeclarePtr(SaveManager);
saDeclarePtr(SaveManager_WeakRef);

typedef struct SaveManager_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SettingsPage* (*createSettingsPage)(_In_ void* self, SubspaceUI* ui);
    bool (*isEnabled)(_In_ void* self);
    bool (*isAvailable)(_In_ void* self);
    bool (*isLocked)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
    void (*setAvailable)(_In_ void* self, bool available);
    void (*applyDefaultSettings)(_In_ void* self);
    void (*sendSetting)(_In_ void* self, ControlClient* client, _In_opt_ strref name);
    void (*sendAllSettings)(_In_ void* self, ControlClient* client);
    // update curinst when a settings changes
    void (*sendSettingCur)(_In_ void* self, _In_opt_ strref name);
} SaveManager_ClassIf;
extern SaveManager_ClassIf SaveManager_ClassIf_tmpl;

typedef struct SaveManager {
    union {
        SaveManager_ClassIf* _;
        void* _is_SaveManager;
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
    bool hidden;
    bool optional;        // Features that are expected to be unavailable, e.g. version-specific
    bool locked;        // cannot be enabled or disabled currently
    SSDNode* settings;        // Settings that are synchronized with the game client
} SaveManager;
extern ObjClassInfo SaveManager_clsinfo;
#define SaveManager(inst) ((SaveManager*)(unused_noeval((inst) && &((inst)->_is_SaveManager)), (inst)))
#define SaveManagerNone ((SaveManager*)NULL)

typedef struct SaveManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SaveManager_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SaveManager_WeakRef;
#define SaveManager_WeakRef(inst) ((SaveManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SaveManager_WeakRef)), (inst)))

_objfactory_guaranteed SaveManager* SaveManager_create(Subspace* ss);
// SaveManager* savemanagerCreate(Subspace* ss);
#define savemanagerCreate(ss) SaveManager_create(ss)

// SettingsPage* savemanagerCreateSettingsPage(SaveManager* self, SubspaceUI* ui);
#define savemanagerCreateSettingsPage(self, ui) (self)->_->createSettingsPage(SaveManager(self), SubspaceUI(ui))
// bool savemanagerIsEnabled(SaveManager* self);
#define savemanagerIsEnabled(self) (self)->_->isEnabled(SaveManager(self))
// bool savemanagerIsAvailable(SaveManager* self);
#define savemanagerIsAvailable(self) (self)->_->isAvailable(SaveManager(self))
// bool savemanagerIsLocked(SaveManager* self);
#define savemanagerIsLocked(self) (self)->_->isLocked(SaveManager(self))
// void savemanagerEnable(SaveManager* self, bool enabled);
#define savemanagerEnable(self, enabled) (self)->_->enable(SaveManager(self), enabled)
// void savemanagerSetAvailable(SaveManager* self, bool available);
#define savemanagerSetAvailable(self, available) (self)->_->setAvailable(SaveManager(self), available)
// void savemanagerApplyDefaultSettings(SaveManager* self);
#define savemanagerApplyDefaultSettings(self) (self)->_->applyDefaultSettings(SaveManager(self))
// void savemanagerSendSetting(SaveManager* self, ControlClient* client, strref name);
#define savemanagerSendSetting(self, client, name) (self)->_->sendSetting(SaveManager(self), ControlClient(client), name)
// void savemanagerSendAllSettings(SaveManager* self, ControlClient* client);
#define savemanagerSendAllSettings(self, client) (self)->_->sendAllSettings(SaveManager(self), ControlClient(client))
// void savemanagerSendSettingCur(SaveManager* self, strref name);
//
// update curinst when a settings changes
#define savemanagerSendSettingCur(self, name) (self)->_->sendSettingCur(SaveManager(self), name)


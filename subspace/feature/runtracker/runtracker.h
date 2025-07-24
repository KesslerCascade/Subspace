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
typedef struct RunTracker RunTracker;
typedef struct RunTracker_WeakRef RunTracker_WeakRef;
saDeclarePtr(RunTracker);
saDeclarePtr(RunTracker_WeakRef);

typedef struct RunTracker_ClassIf {
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
    void (*updateLockState)(_In_ void* self);
} RunTracker_ClassIf;
extern RunTracker_ClassIf RunTracker_ClassIf_tmpl;

typedef struct RunTracker {
    union {
        RunTracker_ClassIf* _;
        void* _is_RunTracker;
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
} RunTracker;
extern ObjClassInfo RunTracker_clsinfo;
#define RunTracker(inst) ((RunTracker*)(unused_noeval((inst) && &((inst)->_is_RunTracker)), (inst)))
#define RunTrackerNone ((RunTracker*)NULL)

typedef struct RunTracker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_RunTracker_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} RunTracker_WeakRef;
#define RunTracker_WeakRef(inst) ((RunTracker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_RunTracker_WeakRef)), (inst)))

_objfactory_guaranteed RunTracker* RunTracker_create(Subspace* ss);
// RunTracker* runtrackerCreate(Subspace* ss);
#define runtrackerCreate(ss) RunTracker_create(ss)

// SettingsPage* runtrackerCreateSettingsPage(RunTracker* self, SubspaceUI* ui);
#define runtrackerCreateSettingsPage(self, ui) (self)->_->createSettingsPage(RunTracker(self), SubspaceUI(ui))
// bool runtrackerIsEnabled(RunTracker* self);
#define runtrackerIsEnabled(self) (self)->_->isEnabled(RunTracker(self))
// bool runtrackerIsAvailable(RunTracker* self);
#define runtrackerIsAvailable(self) (self)->_->isAvailable(RunTracker(self))
// bool runtrackerIsLocked(RunTracker* self);
#define runtrackerIsLocked(self) (self)->_->isLocked(RunTracker(self))
// void runtrackerEnable(RunTracker* self, bool enabled);
#define runtrackerEnable(self, enabled) (self)->_->enable(RunTracker(self), enabled)
// void runtrackerSetAvailable(RunTracker* self, bool available);
#define runtrackerSetAvailable(self, available) (self)->_->setAvailable(RunTracker(self), available)
// void runtrackerApplyDefaultSettings(RunTracker* self);
#define runtrackerApplyDefaultSettings(self) (self)->_->applyDefaultSettings(RunTracker(self))
// void runtrackerSendSetting(RunTracker* self, ControlClient* client, strref name);
#define runtrackerSendSetting(self, client, name) (self)->_->sendSetting(RunTracker(self), ControlClient(client), name)
// void runtrackerSendAllSettings(RunTracker* self, ControlClient* client);
#define runtrackerSendAllSettings(self, client) (self)->_->sendAllSettings(RunTracker(self), ControlClient(client))
// void runtrackerSendSettingCur(RunTracker* self, strref name);
//
// update curinst when a settings changes
#define runtrackerSendSettingCur(self, name) (self)->_->sendSettingCur(RunTracker(self), name)
// void runtrackerUpdateLockState(RunTracker* self);
#define runtrackerUpdateLockState(self) (self)->_->updateLockState(RunTracker(self))


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
typedef struct TimeWarp TimeWarp;
typedef struct TimeWarp_WeakRef TimeWarp_WeakRef;
saDeclarePtr(TimeWarp);
saDeclarePtr(TimeWarp_WeakRef);

typedef struct TimeWarp_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

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
} TimeWarp_ClassIf;
extern TimeWarp_ClassIf TimeWarp_ClassIf_tmpl;

typedef struct TimeWarp {
    union {
        TimeWarp_ClassIf* _;
        void* _is_TimeWarp;
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
} TimeWarp;
extern ObjClassInfo TimeWarp_clsinfo;
#define TimeWarp(inst) ((TimeWarp*)(unused_noeval((inst) && &((inst)->_is_TimeWarp)), (inst)))
#define TimeWarpNone ((TimeWarp*)NULL)

typedef struct TimeWarp_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TimeWarp_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TimeWarp_WeakRef;
#define TimeWarp_WeakRef(inst) ((TimeWarp_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TimeWarp_WeakRef)), (inst)))

_objfactory_guaranteed TimeWarp* TimeWarp_create(Subspace* ss);
// TimeWarp* timewarpCreate(Subspace* ss);
#define timewarpCreate(ss) TimeWarp_create(ss)

// SettingsPage* timewarpCreateSettingsPage(TimeWarp* self, SubspaceUI* ui);
#define timewarpCreateSettingsPage(self, ui) (self)->_->createSettingsPage(TimeWarp(self), SubspaceUI(ui))
// bool timewarpIsEnabled(TimeWarp* self);
#define timewarpIsEnabled(self) (self)->_->isEnabled(TimeWarp(self))
// bool timewarpIsAvailable(TimeWarp* self);
#define timewarpIsAvailable(self) (self)->_->isAvailable(TimeWarp(self))
// void timewarpEnable(TimeWarp* self, bool enabled);
#define timewarpEnable(self, enabled) (self)->_->enable(TimeWarp(self), enabled)
// void timewarpSetAvailable(TimeWarp* self, bool available);
#define timewarpSetAvailable(self, available) (self)->_->setAvailable(TimeWarp(self), available)
// void timewarpApplyDefaultSettings(TimeWarp* self);
#define timewarpApplyDefaultSettings(self) (self)->_->applyDefaultSettings(TimeWarp(self))
// void timewarpSendSetting(TimeWarp* self, ControlClient* client, strref name);
#define timewarpSendSetting(self, client, name) (self)->_->sendSetting(TimeWarp(self), ControlClient(client), name)
// void timewarpSendAllSettings(TimeWarp* self, ControlClient* client);
#define timewarpSendAllSettings(self, client) (self)->_->sendAllSettings(TimeWarp(self), ControlClient(client))
// void timewarpSendSettingCur(TimeWarp* self, strref name);
//
// update curinst when a settings changes
#define timewarpSendSettingCur(self, name) (self)->_->sendSettingCur(TimeWarp(self), name)


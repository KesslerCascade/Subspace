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
typedef struct Tweaks Tweaks;
typedef struct Tweaks_WeakRef Tweaks_WeakRef;
saDeclarePtr(Tweaks);
saDeclarePtr(Tweaks_WeakRef);

typedef struct Tweaks_ClassIf {
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
} Tweaks_ClassIf;
extern Tweaks_ClassIf Tweaks_ClassIf_tmpl;

typedef struct Tweaks {
    union {
        Tweaks_ClassIf* _;
        void* _is_Tweaks;
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
} Tweaks;
extern ObjClassInfo Tweaks_clsinfo;
#define Tweaks(inst) ((Tweaks*)(unused_noeval((inst) && &((inst)->_is_Tweaks)), (inst)))
#define TweaksNone ((Tweaks*)NULL)

typedef struct Tweaks_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Tweaks_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Tweaks_WeakRef;
#define Tweaks_WeakRef(inst) ((Tweaks_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Tweaks_WeakRef)), (inst)))

_objfactory_guaranteed Tweaks* Tweaks_create(Subspace* ss);
// Tweaks* tweaksCreate(Subspace* ss);
#define tweaksCreate(ss) Tweaks_create(ss)

// SettingsPage* tweaksCreateSettingsPage(Tweaks* self, SubspaceUI* ui);
#define tweaksCreateSettingsPage(self, ui) (self)->_->createSettingsPage(Tweaks(self), SubspaceUI(ui))
// bool tweaksIsEnabled(Tweaks* self);
#define tweaksIsEnabled(self) (self)->_->isEnabled(Tweaks(self))
// bool tweaksIsAvailable(Tweaks* self);
#define tweaksIsAvailable(self) (self)->_->isAvailable(Tweaks(self))
// void tweaksEnable(Tweaks* self, bool enabled);
#define tweaksEnable(self, enabled) (self)->_->enable(Tweaks(self), enabled)
// void tweaksSetAvailable(Tweaks* self, bool available);
#define tweaksSetAvailable(self, available) (self)->_->setAvailable(Tweaks(self), available)
// void tweaksApplyDefaultSettings(Tweaks* self);
#define tweaksApplyDefaultSettings(self) (self)->_->applyDefaultSettings(Tweaks(self))
// void tweaksSendSetting(Tweaks* self, ControlClient* client, strref name);
#define tweaksSendSetting(self, client, name) (self)->_->sendSetting(Tweaks(self), ControlClient(client), name)
// void tweaksSendAllSettings(Tweaks* self, ControlClient* client);
#define tweaksSendAllSettings(self, client) (self)->_->sendAllSettings(Tweaks(self), ControlClient(client))
// void tweaksSendSettingCur(Tweaks* self, strref name);
//
// update curinst when a settings changes
#define tweaksSendSettingCur(self, name) (self)->_->sendSettingCur(Tweaks(self), name)


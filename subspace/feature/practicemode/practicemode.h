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
typedef struct PracticeMode PracticeMode;
typedef struct PracticeMode_WeakRef PracticeMode_WeakRef;
saDeclarePtr(PracticeMode);
saDeclarePtr(PracticeMode_WeakRef);

typedef struct PracticeMode_ClassIf {
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
} PracticeMode_ClassIf;
extern PracticeMode_ClassIf PracticeMode_ClassIf_tmpl;

typedef struct PracticeMode {
    union {
        PracticeMode_ClassIf* _;
        void* _is_PracticeMode;
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
} PracticeMode;
extern ObjClassInfo PracticeMode_clsinfo;
#define PracticeMode(inst) ((PracticeMode*)(unused_noeval((inst) && &((inst)->_is_PracticeMode)), (inst)))
#define PracticeModeNone ((PracticeMode*)NULL)

typedef struct PracticeMode_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_PracticeMode_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} PracticeMode_WeakRef;
#define PracticeMode_WeakRef(inst) ((PracticeMode_WeakRef*)(unused_noeval((inst) && &((inst)->_is_PracticeMode_WeakRef)), (inst)))

_objfactory_guaranteed PracticeMode* PracticeMode_create(Subspace* ss);
// PracticeMode* practicemodeCreate(Subspace* ss);
#define practicemodeCreate(ss) PracticeMode_create(ss)

// SettingsPage* practicemodeCreateSettingsPage(PracticeMode* self, SubspaceUI* ui);
#define practicemodeCreateSettingsPage(self, ui) (self)->_->createSettingsPage(PracticeMode(self), SubspaceUI(ui))
// bool practicemodeIsEnabled(PracticeMode* self);
#define practicemodeIsEnabled(self) (self)->_->isEnabled(PracticeMode(self))
// bool practicemodeIsAvailable(PracticeMode* self);
#define practicemodeIsAvailable(self) (self)->_->isAvailable(PracticeMode(self))
// void practicemodeEnable(PracticeMode* self, bool enabled);
#define practicemodeEnable(self, enabled) (self)->_->enable(PracticeMode(self), enabled)
// void practicemodeSetAvailable(PracticeMode* self, bool available);
#define practicemodeSetAvailable(self, available) (self)->_->setAvailable(PracticeMode(self), available)
// void practicemodeApplyDefaultSettings(PracticeMode* self);
#define practicemodeApplyDefaultSettings(self) (self)->_->applyDefaultSettings(PracticeMode(self))
// void practicemodeSendSetting(PracticeMode* self, ControlClient* client, strref name);
#define practicemodeSendSetting(self, client, name) (self)->_->sendSetting(PracticeMode(self), ControlClient(client), name)
// void practicemodeSendAllSettings(PracticeMode* self, ControlClient* client);
#define practicemodeSendAllSettings(self, client) (self)->_->sendAllSettings(PracticeMode(self), ControlClient(client))
// void practicemodeSendSettingCur(PracticeMode* self, strref name);
//
// update curinst when a settings changes
#define practicemodeSendSettingCur(self, name) (self)->_->sendSettingCur(PracticeMode(self), name)


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
typedef struct FrameAdv FrameAdv;
typedef struct FrameAdv_WeakRef FrameAdv_WeakRef;
saDeclarePtr(FrameAdv);
saDeclarePtr(FrameAdv_WeakRef);

typedef struct FrameAdv_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SettingsPage* (*getSettingsPage)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
    void (*applyDefaultSettings)(_In_ void* self);
    void (*sendSetting)(_In_ void* self, ControlClient* client, _In_opt_ strref name);
    void (*sendAllSettings)(_In_ void* self, ControlClient* client);
    // update curinst when a settings changes
    void (*sendSettingCur)(_In_ void* self, _In_opt_ strref name);
} FrameAdv_ClassIf;
extern FrameAdv_ClassIf FrameAdv_ClassIf_tmpl;

typedef struct FrameAdv {
    union {
        FrameAdv_ClassIf* _;
        void* _is_FrameAdv;
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
} FrameAdv;
extern ObjClassInfo FrameAdv_clsinfo;
#define FrameAdv(inst) ((FrameAdv*)(unused_noeval((inst) && &((inst)->_is_FrameAdv)), (inst)))
#define FrameAdvNone ((FrameAdv*)NULL)

typedef struct FrameAdv_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_FrameAdv_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} FrameAdv_WeakRef;
#define FrameAdv_WeakRef(inst) ((FrameAdv_WeakRef*)(unused_noeval((inst) && &((inst)->_is_FrameAdv_WeakRef)), (inst)))

_objfactory_guaranteed FrameAdv* FrameAdv_create(Subspace* ss);
// FrameAdv* frameadvCreate(Subspace* ss);
#define frameadvCreate(ss) FrameAdv_create(ss)

// SettingsPage* frameadvGetSettingsPage(FrameAdv* self);
#define frameadvGetSettingsPage(self) (self)->_->getSettingsPage(FrameAdv(self))
// void frameadvEnable(FrameAdv* self, bool enabled);
#define frameadvEnable(self, enabled) (self)->_->enable(FrameAdv(self), enabled)
// void frameadvApplyDefaultSettings(FrameAdv* self);
#define frameadvApplyDefaultSettings(self) (self)->_->applyDefaultSettings(FrameAdv(self))
// void frameadvSendSetting(FrameAdv* self, ControlClient* client, strref name);
#define frameadvSendSetting(self, client, name) (self)->_->sendSetting(FrameAdv(self), ControlClient(client), name)
// void frameadvSendAllSettings(FrameAdv* self, ControlClient* client);
#define frameadvSendAllSettings(self, client) (self)->_->sendAllSettings(FrameAdv(self), ControlClient(client))
// void frameadvSendSettingCur(FrameAdv* self, strref name);
//
// update curinst when a settings changes
#define frameadvSendSettingCur(self, name) (self)->_->sendSettingCur(FrameAdv(self), name)


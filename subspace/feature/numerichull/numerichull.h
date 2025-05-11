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
typedef struct NumericHull NumericHull;
typedef struct NumericHull_WeakRef NumericHull_WeakRef;
saDeclarePtr(NumericHull);
saDeclarePtr(NumericHull_WeakRef);

typedef struct NumericHull_ClassIf {
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
} NumericHull_ClassIf;
extern NumericHull_ClassIf NumericHull_ClassIf_tmpl;

typedef struct NumericHull {
    union {
        NumericHull_ClassIf* _;
        void* _is_NumericHull;
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
} NumericHull;
extern ObjClassInfo NumericHull_clsinfo;
#define NumericHull(inst) ((NumericHull*)(unused_noeval((inst) && &((inst)->_is_NumericHull)), (inst)))
#define NumericHullNone ((NumericHull*)NULL)

typedef struct NumericHull_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_NumericHull_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} NumericHull_WeakRef;
#define NumericHull_WeakRef(inst) ((NumericHull_WeakRef*)(unused_noeval((inst) && &((inst)->_is_NumericHull_WeakRef)), (inst)))

_objfactory_guaranteed NumericHull* NumericHull_create(Subspace* ss);
// NumericHull* numerichullCreate(Subspace* ss);
#define numerichullCreate(ss) NumericHull_create(ss)

// SettingsPage* numerichullGetSettingsPage(NumericHull* self);
#define numerichullGetSettingsPage(self) (self)->_->getSettingsPage(NumericHull(self))
// void numerichullEnable(NumericHull* self, bool enabled);
#define numerichullEnable(self, enabled) (self)->_->enable(NumericHull(self), enabled)
// void numerichullApplyDefaultSettings(NumericHull* self);
#define numerichullApplyDefaultSettings(self) (self)->_->applyDefaultSettings(NumericHull(self))
// void numerichullSendSetting(NumericHull* self, ControlClient* client, strref name);
#define numerichullSendSetting(self, client, name) (self)->_->sendSetting(NumericHull(self), ControlClient(client), name)
// void numerichullSendAllSettings(NumericHull* self, ControlClient* client);
#define numerichullSendAllSettings(self, client) (self)->_->sendAllSettings(NumericHull(self), ControlClient(client))
// void numerichullSendSettingCur(NumericHull* self, strref name);
//
// update curinst when a settings changes
#define numerichullSendSettingCur(self, name) (self)->_->sendSettingCur(NumericHull(self), name)


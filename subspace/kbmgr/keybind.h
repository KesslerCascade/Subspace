#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature/feature.h"
#include "control/controlclient.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct KeyBind KeyBind;
typedef struct KeyBind_WeakRef KeyBind_WeakRef;
saDeclarePtr(KeyBind);
saDeclarePtr(KeyBind_WeakRef);

typedef struct KeyBind_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*send)(_In_ void* self, ControlClient* client);
} KeyBind_ClassIf;
extern KeyBind_ClassIf KeyBind_ClassIf_tmpl;

typedef struct KeyBind {
    union {
        KeyBind_ClassIf* _;
        void* _is_KeyBind;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Weak(SubspaceFeature)* owner;
    string name;
    int iupkey;
    int ftlkey;
} KeyBind;
extern ObjClassInfo KeyBind_clsinfo;
#define KeyBind(inst) ((KeyBind*)(unused_noeval((inst) && &((inst)->_is_KeyBind)), (inst)))
#define KeyBindNone ((KeyBind*)NULL)

typedef struct KeyBind_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_KeyBind_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} KeyBind_WeakRef;
#define KeyBind_WeakRef(inst) ((KeyBind_WeakRef*)(unused_noeval((inst) && &((inst)->_is_KeyBind_WeakRef)), (inst)))

_objfactory_guaranteed KeyBind* KeyBind_create(SubspaceFeature* owner, _In_opt_ strref name);
// KeyBind* keybindCreate(SubspaceFeature* owner, strref name);
#define keybindCreate(owner, name) KeyBind_create(SubspaceFeature(owner), name)

// void keybindSend(KeyBind* self, ControlClient* client);
#define keybindSend(self, client) (self)->_->send(KeyBind(self), ControlClient(client))


#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "feature/feature.h"
#include "subspace.h"
#include "process.h"
#include "proto.h"

typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct GameInst GameInst;
typedef struct GameInst_WeakRef GameInst_WeakRef;
saDeclarePtr(GameInst);
saDeclarePtr(GameInst_WeakRef);

typedef enum GameInstState {
    GI_Init = GAME_INIT,
    GI_Loading = GAME_LOADING,
    GI_Menu = GAME_MENU,
    GI_Run = GAME_RUN,
    GI_Practice = GAME_PRACTICE,
    GI_Launching,
    GI_Failed,
    GI_Exited,
    GI_Validated
} GameInstState;

typedef struct GameInst_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*launch)(_In_ void* self);
    void (*setState)(_In_ void* self, GameInstState state);
    void (*setStateLocked)(_In_ void* self, GameInstState state);
    GameInstState (*getState)(_In_ void* self);
    void (*onGameReady)(_In_ void* self, ControlClient* client);
} GameInst_ClassIf;
extern GameInst_ClassIf GameInst_ClassIf_tmpl;

typedef struct GameInst {
    union {
        GameInst_ClassIf* _;
        void* _is_GameInst;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    Weak(GameMgr)* mgr;
    uint32 cookie;        // unique cookie for game comms
    Weak(ControlClient)* client;
    ProcessHandle process;
    RWLock lock;
    string exepath;
    LaunchMode mode;
    GameInstState state;
    hashtable features;
    int32 ver[3];
    float loadPct;
    int failReason;
} GameInst;
extern ObjClassInfo GameInst_clsinfo;
#define GameInst(inst) ((GameInst*)(unused_noeval((inst) && &((inst)->_is_GameInst)), (inst)))
#define GameInstNone ((GameInst*)NULL)

typedef struct GameInst_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameInst_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameInst_WeakRef;
#define GameInst_WeakRef(inst) ((GameInst_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameInst_WeakRef)), (inst)))

_objfactory_guaranteed GameInst* GameInst_create(GameMgr* mgr, _In_opt_ strref exepath, LaunchMode mode);
// GameInst* ginstCreate(GameMgr* mgr, strref exepath, LaunchMode mode);
#define ginstCreate(mgr, exepath, mode) GameInst_create(GameMgr(mgr), exepath, mode)

_objfactory_guaranteed GameInst* GameInst_createForClient(GameMgr* mgr, ControlClient* client, uint32 cookie);
// GameInst* ginstCreateForClient(GameMgr* mgr, ControlClient* client, uint32 cookie);
//
// for dev mode clients
#define ginstCreateForClient(mgr, client, cookie) GameInst_createForClient(GameMgr(mgr), ControlClient(client), cookie)

// bool ginstLaunch(GameInst* self);
#define ginstLaunch(self) (self)->_->launch(GameInst(self))
// void ginstSetState(GameInst* self, GameInstState state);
#define ginstSetState(self, state) (self)->_->setState(GameInst(self), state)
// void ginstSetStateLocked(GameInst* self, GameInstState state);
#define ginstSetStateLocked(self, state) (self)->_->setStateLocked(GameInst(self), state)
// GameInstState ginstGetState(GameInst* self);
#define ginstGetState(self) (self)->_->getState(GameInst(self))
// void ginstOnGameReady(GameInst* self, ControlClient* client);
#define ginstOnGameReady(self, client) (self)->_->onGameReady(GameInst(self), ControlClient(client))


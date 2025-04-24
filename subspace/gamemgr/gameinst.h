#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"

typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct GameInst GameInst;
typedef struct GameInst_WeakRef GameInst_WeakRef;
saDeclarePtr(GameInst);
saDeclarePtr(GameInst_WeakRef);

#ifdef _PLATFORM_WIN
typedef void* ProcessHandle;
#endif

typedef enum GameInstLaunchMode {
    GI_PLAY,
    GI_VALIDATE
} GameInstLaunchMode;

typedef enum GameInstState {
    GI_Init,
    GI_Launching,
    GI_Loading,
    GI_Menu,
    GI_Run,
    GI_Practice,
    GI_Failed,
    GI_Exited
} GameInstState;

typedef struct GameInst_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*launch)(_In_ void* self);
    void (*setState)(_In_ void* self, GameInstState state);
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
    GameInstLaunchMode mode;
    GameInstState state;
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

_objfactory_guaranteed GameInst* GameInst_create(GameMgr* mgr, _In_opt_ strref exepath, GameInstLaunchMode mode);
// GameInst* ginstCreate(GameMgr* mgr, strref exepath, GameInstLaunchMode mode);
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


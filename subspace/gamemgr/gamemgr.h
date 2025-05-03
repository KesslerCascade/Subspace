#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "gameinst.h"
#include "process.h"
#include <cx/math.h>

typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct GameMgr GameMgr;
typedef struct GameMgr_WeakRef GameMgr_WeakRef;
saDeclarePtr(GameMgr);
saDeclarePtr(GameMgr_WeakRef);

typedef struct GameMgr_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*reg)(_In_ void* self, GameInst* inst);
    void (*unreg)(_In_ void* self, GameInst* inst);
    GameInst* (*acquireByCookie)(_In_ void* self, uint32 cookie);
    bool (*launchGame)(_In_ void* self, LaunchMode mode, GameInst** out);
    uint32 (*genCookie)(_In_ void* self);
} GameMgr_ClassIf;
extern GameMgr_ClassIf GameMgr_ClassIf_tmpl;

typedef struct GameMgr {
    union {
        GameMgr_ClassIf* _;
        void* _is_GameMgr;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    RWLock gmgrlock;
    ProcWatchState* pws;
    hashtable insts;        // hashed by cookie
    PcgState pcg;
} GameMgr;
extern ObjClassInfo GameMgr_clsinfo;
#define GameMgr(inst) ((GameMgr*)(unused_noeval((inst) && &((inst)->_is_GameMgr)), (inst)))
#define GameMgrNone ((GameMgr*)NULL)

typedef struct GameMgr_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameMgr_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameMgr_WeakRef;
#define GameMgr_WeakRef(inst) ((GameMgr_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameMgr_WeakRef)), (inst)))

_objfactory_guaranteed GameMgr* GameMgr_create(Subspace* subspace);
// GameMgr* gmgrCreate(Subspace* subspace);
#define gmgrCreate(subspace) GameMgr_create(subspace)

// void gmgrReg(GameMgr* self, GameInst* inst);
#define gmgrReg(self, inst) (self)->_->reg(GameMgr(self), GameInst(inst))
// void gmgrUnreg(GameMgr* self, GameInst* inst);
#define gmgrUnreg(self, inst) (self)->_->unreg(GameMgr(self), GameInst(inst))
// GameInst* gmgrAcquireByCookie(GameMgr* self, uint32 cookie);
#define gmgrAcquireByCookie(self, cookie) (self)->_->acquireByCookie(GameMgr(self), cookie)
// bool gmgrLaunchGame(GameMgr* self, LaunchMode mode, GameInst** out);
#define gmgrLaunchGame(self, mode, out) (self)->_->launchGame(GameMgr(self), mode, out)
// uint32 gmgrGenCookie(GameMgr* self);
#define gmgrGenCookie(self) (self)->_->genCookie(GameMgr(self))


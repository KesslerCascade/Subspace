#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"

typedef struct GameData GameData;
typedef struct GameData_WeakRef GameData_WeakRef;
saDeclarePtr(GameData);
saDeclarePtr(GameData_WeakRef);

typedef struct GameData {
    union {
        ObjIface* _;
        void* _is_GameData;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

} GameData;
extern ObjClassInfo GameData_clsinfo;
#define GameData(inst) ((GameData*)(unused_noeval((inst) && &((inst)->_is_GameData)), (inst)))
#define GameDataNone ((GameData*)NULL)

typedef struct GameData_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameData_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameData_WeakRef;
#define GameData_WeakRef(inst) ((GameData_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameData_WeakRef)), (inst)))



#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/resource/trfifo.h>
#include "subspace.h"

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRFifoNode TRFifoNode;
typedef struct sqlite3 sqlite3;
typedef struct Database Database;
typedef struct Database_WeakRef Database_WeakRef;
saDeclarePtr(Database);
saDeclarePtr(Database_WeakRef);

typedef struct Database_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*open)(_In_ void* self);
    // upgrade schema if necessary
    bool (*check)(_In_ void* self);
    void (*close)(_In_ void* self);
} Database_ClassIf;
extern Database_ClassIf Database_ClassIf_tmpl;

typedef struct Database {
    union {
        Database_ClassIf* _;
        void* _is_Database;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    sqlite3* db;
    TRFifo* fifo;        // for database multi-threading
} Database;
extern ObjClassInfo Database_clsinfo;
#define Database(inst) ((Database*)(unused_noeval((inst) && &((inst)->_is_Database)), (inst)))
#define DatabaseNone ((Database*)NULL)

typedef struct Database_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Database_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Database_WeakRef;
#define Database_WeakRef(inst) ((Database_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Database_WeakRef)), (inst)))

_objfactory_guaranteed Database* Database_create(Subspace* ss);
// Database* databaseCreate(Subspace* ss);
#define databaseCreate(ss) Database_create(ss)

// bool databaseOpen(Database* self);
#define databaseOpen(self) (self)->_->open(Database(self))
// bool databaseCheck(Database* self);
//
// upgrade schema if necessary
#define databaseCheck(self) (self)->_->check(Database(self))
// void databaseClose(Database* self);
#define databaseClose(self) (self)->_->close(Database(self))


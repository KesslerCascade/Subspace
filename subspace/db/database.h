#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "dbstmt.h"
#include "subspace.h"

typedef struct Database Database;
typedef struct Database_WeakRef Database_WeakRef;
typedef struct sqlite3_stmt sqlite3_stmt;
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
    DbStmt* (*prepare)(_In_ void* self, _In_opt_ strref sql);
    // get last auto-increment ID
    int64 (*lastId)(_In_ void* self);
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
// Database* dbCreate(Subspace* ss);
#define dbCreate(ss) Database_create(ss)

// bool dbOpen(Database* self);
#define dbOpen(self) (self)->_->open(Database(self))
// bool dbCheck(Database* self);
//
// upgrade schema if necessary
#define dbCheck(self) (self)->_->check(Database(self))
// void dbClose(Database* self);
#define dbClose(self) (self)->_->close(Database(self))
// DbStmt* dbPrepare(Database* self, strref sql);
#define dbPrepare(self, sql) (self)->_->prepare(Database(self), sql)
// int64 dbLastId(Database* self);
//
// get last auto-increment ID
#define dbLastId(self) (self)->_->lastId(Database(self))


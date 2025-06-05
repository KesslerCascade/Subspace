#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct Database Database;
typedef struct Database_WeakRef Database_WeakRef;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct DbStmt DbStmt;
typedef struct DbStmt_WeakRef DbStmt_WeakRef;
saDeclarePtr(DbStmt);
saDeclarePtr(DbStmt_WeakRef);

typedef struct DbStmt_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*bind)(_In_ void* self, int ncol, stvar val);
    bool (*exec)(_In_ void* self);
    void (*reset)(_In_ void* self);
    void (*clearBind)(_In_ void* self);
} DbStmt_ClassIf;
extern DbStmt_ClassIf DbStmt_ClassIf_tmpl;

typedef struct DbStmt {
    union {
        DbStmt_ClassIf* _;
        void* _is_DbStmt;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Database* db;
    sqlite3_stmt* stmt;
    sa_stvar row;
    bool error;
} DbStmt;
extern ObjClassInfo DbStmt_clsinfo;
#define DbStmt(inst) ((DbStmt*)(unused_noeval((inst) && &((inst)->_is_DbStmt)), (inst)))
#define DbStmtNone ((DbStmt*)NULL)

typedef struct DbStmt_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_DbStmt_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} DbStmt_WeakRef;
#define DbStmt_WeakRef(inst) ((DbStmt_WeakRef*)(unused_noeval((inst) && &((inst)->_is_DbStmt_WeakRef)), (inst)))

_objfactory_check DbStmt* DbStmt_create(Database* db, _In_opt_ strref sql);
// DbStmt* dbstmtCreate(Database* db, strref sql);
#define dbstmtCreate(db, sql) DbStmt_create(Database(db), sql)

// bool dbstmtBind(DbStmt* self, int ncol, stvar val);
#define dbstmtBind(self, ncol, val) (self)->_->bind(DbStmt(self), ncol, val)
// bool dbstmtExec(DbStmt* self);
#define dbstmtExec(self) (self)->_->exec(DbStmt(self))
// void dbstmtReset(DbStmt* self);
#define dbstmtReset(self) (self)->_->reset(DbStmt(self))
// void dbstmtClearBind(DbStmt* self);
#define dbstmtClearBind(self) (self)->_->clearBind(DbStmt(self))


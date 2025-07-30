#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "feature/feature.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct SettingsPage SettingsPage;
typedef struct SettingsPage_WeakRef SettingsPage_WeakRef;
typedef struct ControlClient ControlClient;
typedef struct ControlClient_WeakRef ControlClient_WeakRef;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct SaveManager SaveManager;
typedef struct SaveManager_WeakRef SaveManager_WeakRef;
typedef struct SaveManagerCopy SaveManagerCopy;
typedef struct SaveManagerCopy_WeakRef SaveManagerCopy_WeakRef;
saDeclarePtr(SaveManager);
saDeclarePtr(SaveManager_WeakRef);
saDeclarePtr(SaveManagerCopy);
saDeclarePtr(SaveManagerCopy_WeakRef);

typedef struct SaveManager_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    SettingsPage* (*createSettingsPage)(_In_ void* self, SubspaceUI* ui);
    bool (*isEnabled)(_In_ void* self);
    bool (*isAvailable)(_In_ void* self);
    bool (*isLocked)(_In_ void* self);
    void (*enable)(_In_ void* self, bool enabled);
    void (*setAvailable)(_In_ void* self, bool available);
    void (*applyDefaultSettings)(_In_ void* self);
    void (*sendSetting)(_In_ void* self, ControlClient* client, _In_opt_ strref name);
    void (*sendAllSettings)(_In_ void* self, ControlClient* client);
    // update curinst when a settings changes
    void (*sendSettingCur)(_In_ void* self, _In_opt_ strref name);
    bool (*autoSave)(_In_ void* self, RunInfo* run, _In_opt_ strref fname);
} SaveManager_ClassIf;
extern SaveManager_ClassIf SaveManager_ClassIf_tmpl;

typedef struct SaveManagerCopy_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
} SaveManagerCopy_ClassIf;
extern SaveManagerCopy_ClassIf SaveManagerCopy_ClassIf_tmpl;

typedef struct SaveManager {
    union {
        SaveManager_ClassIf* _;
        void* _is_SaveManager;
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
    bool hidden;
    bool optional;        // Features that are expected to be unavailable, e.g. version-specific
    bool locked;        // cannot be enabled or disabled currently
    SSDNode* settings;        // Settings that are synchronized with the game client
} SaveManager;
extern ObjClassInfo SaveManager_clsinfo;
#define SaveManager(inst) ((SaveManager*)(unused_noeval((inst) && &((inst)->_is_SaveManager)), (inst)))
#define SaveManagerNone ((SaveManager*)NULL)

typedef struct SaveManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SaveManager_WeakRef;
        void* _is_SubspaceFeature_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SaveManager_WeakRef;
#define SaveManager_WeakRef(inst) ((SaveManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SaveManager_WeakRef)), (inst)))

_objfactory_guaranteed SaveManager* SaveManager_create(Subspace* ss);
// SaveManager* savemanagerCreate(Subspace* ss);
#define savemanagerCreate(ss) SaveManager_create(ss)

// SettingsPage* savemanagerCreateSettingsPage(SaveManager* self, SubspaceUI* ui);
#define savemanagerCreateSettingsPage(self, ui) (self)->_->createSettingsPage(SaveManager(self), SubspaceUI(ui))
// bool savemanagerIsEnabled(SaveManager* self);
#define savemanagerIsEnabled(self) (self)->_->isEnabled(SaveManager(self))
// bool savemanagerIsAvailable(SaveManager* self);
#define savemanagerIsAvailable(self) (self)->_->isAvailable(SaveManager(self))
// bool savemanagerIsLocked(SaveManager* self);
#define savemanagerIsLocked(self) (self)->_->isLocked(SaveManager(self))
// void savemanagerEnable(SaveManager* self, bool enabled);
#define savemanagerEnable(self, enabled) (self)->_->enable(SaveManager(self), enabled)
// void savemanagerSetAvailable(SaveManager* self, bool available);
#define savemanagerSetAvailable(self, available) (self)->_->setAvailable(SaveManager(self), available)
// void savemanagerApplyDefaultSettings(SaveManager* self);
#define savemanagerApplyDefaultSettings(self) (self)->_->applyDefaultSettings(SaveManager(self))
// void savemanagerSendSetting(SaveManager* self, ControlClient* client, strref name);
#define savemanagerSendSetting(self, client, name) (self)->_->sendSetting(SaveManager(self), ControlClient(client), name)
// void savemanagerSendAllSettings(SaveManager* self, ControlClient* client);
#define savemanagerSendAllSettings(self, client) (self)->_->sendAllSettings(SaveManager(self), ControlClient(client))
// void savemanagerSendSettingCur(SaveManager* self, strref name);
//
// update curinst when a settings changes
#define savemanagerSendSettingCur(self, name) (self)->_->sendSettingCur(SaveManager(self), name)
// bool savemanagerAutoSave(SaveManager* self, RunInfo* run, strref fname);
#define savemanagerAutoSave(self, run, fname) (self)->_->autoSave(SaveManager(self), RunInfo(run), fname)

typedef struct SaveManagerCopy {
    union {
        SaveManagerCopy_ClassIf* _;
        void* _is_SaveManagerCopy;
        void* _is_ComplexTask;
        void* _is_Task;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    string name;        // task name to be shown in monitor output
    int64 last;        // the last time this task was moved between queues and/or run
    cchain oncomplete;        // functions that are called when this task has completed
    int64 nextrun;        // next time for this task to run when scheduled
    int64 lastprogress;        // timestamp of last progress change
    Weak(ComplexTaskQueue)* lastq;        // The last queue this task ran on before it was deferred
    sa_TaskRequires _requires;        // list of requirements that must be satisfied
    uint16 flags;        // flags to customize task behavior
    uint16 _intflags;        // internal flags reserved for use by the scheduler
    atomic(uint32) _advcount;        // number of times this task has been advanced
    SaveManager* smgr;
    RunInfo* run;
    int64 runid;
    int64 savepoint;
    int64 timestamp;
    string fname;
    string destfn;
} SaveManagerCopy;
extern ObjClassInfo SaveManagerCopy_clsinfo;
#define SaveManagerCopy(inst) ((SaveManagerCopy*)(unused_noeval((inst) && &((inst)->_is_SaveManagerCopy)), (inst)))
#define SaveManagerCopyNone ((SaveManagerCopy*)NULL)

typedef struct SaveManagerCopy_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SaveManagerCopy_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SaveManagerCopy_WeakRef;
#define SaveManagerCopy_WeakRef(inst) ((SaveManagerCopy_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SaveManagerCopy_WeakRef)), (inst)))

_objfactory_guaranteed SaveManagerCopy* SaveManagerCopy_create(SaveManager* smgr, RunInfo* run, _In_opt_ strref fname, _In_opt_ strref destfn, int64 runid, int64 savepoint, int64 timestamp);
// SaveManagerCopy* savemanagercopyCreate(SaveManager* smgr, RunInfo* run, strref fname, strref destfn, int64 runid, int64 savepoint, int64 timestamp);
#define savemanagercopyCreate(smgr, run, fname, destfn, runid, savepoint, timestamp) SaveManagerCopy_create(SaveManager(smgr), RunInfo(run), fname, destfn, runid, savepoint, timestamp)

// void savemanagercopyRequireTask(SaveManagerCopy* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define savemanagercopyRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void savemanagercopyRequireTaskTimeout(SaveManagerCopy* self, Task* dep, bool failok, int64 timeout);
#define savemanagercopyRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void savemanagercopyRequireResource(SaveManagerCopy* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define savemanagercopyRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void savemanagercopyRequireResourceTimeout(SaveManagerCopy* self, TaskResource* res, int64 timeout);
#define savemanagercopyRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void savemanagercopyRequireGate(SaveManagerCopy* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define savemanagercopyRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void savemanagercopyRequireGateTimeout(SaveManagerCopy* self, TRGate* gate, int64 timeout);
#define savemanagercopyRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void savemanagercopyRequire(SaveManagerCopy* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define savemanagercopyRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool savemanagercopyAdvance(SaveManagerCopy* self);
//
// advance a deferred task to run as soon as possible
#define savemanagercopyAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 savemanagercopyCheckRequires(SaveManagerCopy* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define savemanagercopyCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void savemanagercopyCancelRequires(SaveManagerCopy* self);
//
// cascade a task cancellation to any requirements
#define savemanagercopyCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool savemanagercopyAcquireRequires(SaveManagerCopy* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define savemanagercopyAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool savemanagercopyReleaseRequires(SaveManagerCopy* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define savemanagercopyReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool savemanagercopy_setState(SaveManagerCopy* self, uint32 newstate);
#define savemanagercopy_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 savemanagercopyRun(SaveManagerCopy* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define savemanagercopyRun(self, tq, worker, tcon) (self)->_->run(SaveManagerCopy(self), TaskQueue(tq), TQWorker(worker), tcon)
// void savemanagercopyRunCancelled(SaveManagerCopy* self, TaskQueue* tq, TQWorker* worker);
#define savemanagercopyRunCancelled(self, tq, worker) (self)->_->runCancelled(SaveManagerCopy(self), TaskQueue(tq), TQWorker(worker))
// bool savemanagercopyCancel(SaveManagerCopy* self);
#define savemanagercopyCancel(self) (self)->_->cancel(SaveManagerCopy(self))
// bool savemanagercopyReset(SaveManagerCopy* self);
#define savemanagercopyReset(self) (self)->_->reset(SaveManagerCopy(self))
// bool savemanagercopyWait(SaveManagerCopy* self, int64 timeout);
#define savemanagercopyWait(self, timeout) (self)->_->wait(SaveManagerCopy(self), timeout)
// intptr savemanagercopyCmp(SaveManagerCopy* self, SaveManagerCopy* other, uint32 flags);
#define savemanagercopyCmp(self, other, flags) (self)->_->cmp(SaveManagerCopy(self), other, flags)
// uint32 savemanagercopyHash(SaveManagerCopy* self, uint32 flags);
#define savemanagercopyHash(self, flags) (self)->_->hash(SaveManagerCopy(self), flags)


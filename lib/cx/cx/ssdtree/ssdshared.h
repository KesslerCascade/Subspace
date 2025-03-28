#pragma once

#include <cx/thread/rwlock.h>
#include <cx/meta/block.h>
#include <cx/container/sarray.h>

#if DEBUG_LEVEL > 1
#define SSD_LOCK_DEBUG 1
#endif

typedef struct SSDTree SSDTree;
typedef struct SSDNode SSDNode;
typedef SSDNode *(*SSDNodeFactory)(SSDTree *info);

enum SSD_FLAGS_ENUM {
    SSD_CaseInsensitive = 0x0001,       // keys in hashtables are case-insensitive
};

typedef enum SSD_CREATE_TYPE_ENUM {
    SSD_Create_None = 0,
    SSD_Create_Hashtable,
    SSD_Create_Array,
    SSD_Create_Single,

    SSD_Create_Count
} SSDCreateType;

typedef struct SSDLockDebug {
    int64 time;
    const char *file;
    int32 line;
} SSDLockDebug;
saDeclare(SSDLockDebug);

typedef struct SSDLockState
{
    union {
        bool _is_SSDLockState;
        bool init;                      // lock state structure initialized
    };
    bool rdlock;                        // read lock held by current thread
    bool wrlock;                        // write lock held by current thread
#ifdef SSD_LOCK_DEBUG
    SSDLockDebug dbg;
#endif
} SSDLockState;

enum SSD_LOCK_STATE_ENUM {
    _ssdCurrentLockState = 0
};

// Initializes a lock structure
#ifdef SSD_LOCK_DEBUG
SSDLockState *__ssdLockStateInit(_Out_ SSDLockState *lstate, _In_z_ const char *fn, int lnum);
#define _ssdLockStateInit(lstate) __ssdLockStateInit(lstate, __FILE__, __LINE__)
#else
SSDLockState *_ssdLockStateInit(_Out_ SSDLockState *lstate);
#endif

// bool ssdLockRead(SSDNode *root)
//
// Starts a locked operation in read mode
#ifdef SSD_LOCK_DEBUG
#define ssdLockRead(root) _ssdLockRead(SSDNode(root), (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState, __FILE__, __LINE__)
#define _ssdManualLockRead(root, lstate) _ssdLockRead(SSDNode(root), lstate, __FILE__, __LINE__)
bool _ssdLockRead(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate, _In_z_ const char *fn, int lnum);
#else
#define ssdLockRead(root) _ssdLockRead(SSDNode(root), _ssdCurrentLockState)
#define _ssdManualLockRead(root, lstate) _ssdLockRead(SSDNode(root), lstate)
bool _ssdLockRead(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate);
#endif

// bool ssdLockWrite(SSDNode *root)
//
// Starts a operation in write mode or upgrades one from read to write
// NOTE, upgrading the lock drops it briefly, do not assume that no one else
// got the write lock in between! State may be changed between dropping the
// read lock and getting the write lock.
#ifdef SSD_LOCK_DEBUG
#define ssdLockWrite(root) _ssdLockWrite(SSDNode(root), (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState, __FILE__, __LINE__)
#define _ssdManualLockWrite(root, lstate) _ssdLockWrite(SSDNode(root), lstate, __FILE__, __LINE__)
bool _ssdLockWrite(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate, _In_z_ const char *fn, int lnum);
#else
#define ssdLockWrite(root) _ssdLockWrite(SSDNode(root), _ssdCurrentLockState)
#define _ssdManualLockWrite(root, lstate) _ssdLockWrite(SSDNode(root), lstate)
bool _ssdLockWrite(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate);
#endif

// Unlocks the lock, potentially for re-use laster
#define ssdUnlock(root) _ssdUnlock(SSDNode(root), (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)
bool _ssdUnlock(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate);

// Ends a locked operation; destroys the lock
#define ssdLockEnd(root) _ssdLockEnd(SSDNode(root), (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)
bool _ssdLockEnd(_Inout_ SSDNode *root, _Inout_ SSDLockState *lstate);

// Wraps a locked transaction. Should be used around a group of SSD transactions that
// should be executed together.
#define ssdLockedTransaction(root) _blkStart                                                    \
    _inhibitReturn                                                                              \
    _blkFull(SSDLockState _ssdTransientLockState = { 0 }, (root),                               \
        _ssdLockEnd(SSDNode(root), &_ssdTransientLockState))                                    \
    _blkBefore(SSDLockState *_ssdCurrentLockStateShadow = (SSDLockState*)_ssdCurrentLockState)  \
    _blkBefore(SSDLockState *_ssdCurrentLockState = _ssdCurrentLockStateShadow ?                \
        _ssdCurrentLockStateShadow : _ssdLockStateInit(&_ssdTransientLockState))                \
    _blkEnd

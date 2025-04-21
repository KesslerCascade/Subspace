#pragma once

// Settings module

// This is a specialized SSDTree backed by a json file in a VFS.
// Changes to the tree are automatically flushed to disk by a background thread.
// Also included is the optional ability to bind specific setting paths to static/global variables.

#include <cx/ssdtree/ssdtree.h>
#include <cx/stype/stconvert.h>
#include <cx/stype/stvar.h>

typedef struct VFS VFS;

// currently supported types for settings:
// bool
// all int/uint types except uint64
// float (32 and 64)
// string
typedef struct SetsBindSpec {
    string name;
    intptr offset;
    stvar deftyp;   // default value AND type
} SetsBindSpec;

SSDNode* setsOpen(VFS* vfs, strref path, int64 flush_interval);

// Does NOT take an sarray, but rather a static array with a NULL name/ptr at the end.
// Multiple calls will stack bindings; replacing only those that have the same name.
bool setsBind(SSDNode* sets, SetsBindSpec* bindings, void* base);
void setsUnbindAll(SSDNode* sets);

// loads and saves settings into a variable without actually binding it
bool setsImport(SSDNode* sets, SetsBindSpec* bindings, void* base);
bool setsExport(SSDNode* sets, SetsBindSpec* bindings, void* base);

// Acquires an object, caller must call objRelease on the subsetting!
#define setsGetSub(sets, path) ssdSubtree(sets, path, SSD_Create_Hashtable)

#define setsGet(sets, path, type, out, def) ssdCopyOutD(sets, path, type, out, def)
#define setsSet(sets, path, type, val)      ssdSet(sets, path, true, stvar(type, val))
#define setsRemove(sets, path)              ssdRemove(sets, path)

// forces a save to disk at next flush interval
void setsSetDirty(SSDNode* sets);

// forces a full rescan of bound variables at next flush interval
void setsCheckBound(SSDNode* sets);

bool setsFlush(SSDNode* sets);
bool setsClose(SSDNode** sets);

#pragma once

#include <cx/cx.h>

// ---------- Platform-specific filesystem operations ----------

CX_C_BEGIN

// Convert a platform-specific path into a CX path
void pathFromPlatform(_Inout_ string *out, _In_opt_ strref platformpath);
// Convert a CX normalized path into a platform-specific path
void pathToPlatform(_Inout_ string *out, _In_opt_ strref path);

// Get / set current directory
void fsCurDir(_Inout_ string *out);
bool fsSetCurDir(_In_opt_ strref cur);

// Make a relative path into an absolute one, using the OS's current directory
bool pathMakeAbsolute(_Inout_ string *out, _In_opt_ strref path);

// Get running executable name
void fsExe(_Inout_ string *out);
void fsExeDir(_Inout_ string *out);

typedef enum FSPathStatEnum {
    FS_Nonexistent,
    FS_Directory,
    FS_File
} FSPathStat;

typedef struct FSStat {
    uint64 size;
    int64 created;
    int64 modified;
    int64 accessed;
} FSStat;

_Success_(return != FS_Nonexistent)
FSPathStat fsStat(_In_opt_ strref path, _Out_opt_ FSStat *stat);

_meta_inline bool fsExist(_In_opt_ strref path)
{
    return fsStat(path, NULL) != FS_Nonexistent;
}
_meta_inline bool fsIsDir(_In_opt_ strref path)
{
    return fsStat(path, NULL) == FS_Directory;
}
_meta_inline bool fsIsFile(_In_opt_ strref path)
{
    return fsStat(path, NULL) == FS_File;
}

bool fsSetTimes(_In_opt_ strref path, int64 modified, int64 accessed);

bool fsCreateDir(_In_opt_ strref path);
bool fsCreateAll(_In_opt_ strref path);
bool fsRemoveDir(_In_opt_ strref path);
bool fsDelete(_In_opt_ strref path);
bool fsRename(_In_opt_ strref from, _In_opt_ strref to);

typedef struct FSSearch FSSearch;
typedef struct FSSearchIter {
    string name;
    int type;
    FSStat stat;
    // private
    void *_search;
} FSSearchIter;

bool fsSearchInit(_Out_ FSSearchIter *iter, _In_opt_ strref path, _In_opt_ strref pattern, bool stat);
bool fsSearchNext(_Inout_ FSSearchIter *iter);
void fsSearchFinish(_Inout_ FSSearchIter *iter);
_meta_inline bool fsSearchValid(_In_ FSSearchIter *iter) { return iter->name; }

CX_C_END

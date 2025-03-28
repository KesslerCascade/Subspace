#pragma once

#include <cx/cx.h>
#include <cx/container/sarray.h>

// ---------- Platform-indepedent path manipulation ----------
// All pathnames are UTF-8!

// CX pathnames take the form of:
//     namespace:/path/to/file
//
// The meaning of namespace depends on the context in which the path is used.
//
// Windows filesystem: Each drive letter is its own namespace, unc namespace is used for UNC paths
// Unix filesystem: None, namespace is ignored
// VFS: Identifies a VFS namespace (if that namespace is allowed by the view)

CX_C_BEGIN

// Eliminate redundant path elements, convert backslashes, etc
void pathNormalize(_Inout_ string *path);

// Same as pathNormalize, but outputs an sarray of strings
// returns true if it was an absolute path, false for relative
bool pathDecompose(_Inout_ string *ns, _In_ sa_string *components, _In_opt_ strref path);

// Recomposes a path
bool pathCompose(_Inout_ string *out, _In_opt_ strref ns, _In_ sa_string components);

// Is this an absolute or relative path?
bool pathIsAbsolute(_In_opt_ strref path);

// Get parent directory
bool pathParent(_Inout_ string *out, _In_opt_ strref path);

// Get filename
bool pathFilename(_Inout_ string *out, _In_opt_ strref path);

// Split a full path into a namespace/path components
bool pathSplitNS(_Inout_ string *nspart, _Inout_ string *pathpart, _In_opt_ strref path);

// For convenience, just string concats with the path separator
bool _pathJoin(_Inout_ string *out, int n, _In_ strref* elements);
#define pathJoin(out, ...) _pathJoin(out, count_macro_args(__VA_ARGS__), (strref[]){ __VA_ARGS__ })

// Add filename extension
void pathAddExt(_Inout_ string *out, _In_opt_ strref path, _In_opt_ strref ext);
bool pathRemoveExt(_Inout_ string *out, _In_opt_ strref path);
bool pathGetExt(_Inout_ string *out, _In_opt_ strref path);
void pathSetExt(_Inout_ string *out, _In_opt_ strref path, _In_opt_ strref ext);

// Path wildcard matching
enum PathMatchFlags {
    PATH_LeadingDir       = 1,      // Ignore /* after successful match
    PATH_IgnorePath       = 2,      // match slashes as regular character
    PATH_Smart            = 4,      // smart path matching for intuitive patterns
                                    // patterns that begin with / are treated as full pathnames,
                                    //   the leading / is ignored and they match entire subdirs
                                    //   like PATH_LeadingDir
                                    // patterns that do NOT begin with / are matched only against
                                    //   the filename component
    PATH_CaseInsensitive  = 8,      // Ignore case differences
};
bool pathMatch(_In_opt_ strref path, _In_opt_ strref pattern, uint32 flags);

CX_C_END

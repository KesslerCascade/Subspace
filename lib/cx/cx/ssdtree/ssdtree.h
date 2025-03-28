#pragma once

#include <cx/ssdtree/node/ssdnode.h>
#include <cx/string/strbase.h>
#include <cx/container/sarray.h>

// Semi-Structured data tree

_Ret_valid_
SSDNode *_ssdCreateRoot(int crtype, _In_opt_ SSDTree *tree, uint32 flags);
// Creates a new semi-structured tree with a hashtable as its root.
// Returns a reference-counted object that must be disposed of with objRelease
#define ssdCreateHashtable(...) _ssdCreateRoot(SSD_Create_Hashtable, NULL, opt_flags(__VA_ARGS__))
// Creates a new semi-structured tree with an array as its root.
// Returns a reference-counted object that must be disposed of with objRelease
#define ssdCreateArray(...) _ssdCreateRoot(SSD_Create_Array, NULL, opt_flags(__VA_ARGS__))
// Creates a new semi-structured tree with a single value as its root.
// The supplied value must be one of the supported types!
// (int, float, string, boolean, or stvNone)
// Returns a reference-counted object that must be disposed of with objRelease
#define ssdCreateSingle(...) _ssdCreateRoot(SSD_Create_Single, NULL, opt_flags(__VA_ARGS__))
#define ssdCreateCustom(crtype, tree) _ssdCreateRoot(crtype, tree, 0)

_Ret_opt_valid_
SSDNode *_ssdClone(_In_ SSDNode *root, _In_opt_ SSDTree *desttree, _Inout_opt_ SSDLockState *lstate);
// SSDNode *ssdClone(SSDNode *root, SSDTree *desttree);
// Creates a deep clone of root. If desttree is not NULL, associates the clone with that tree -- the
// resulting node will be an isolated branch not yet attached to the rest of the tree but part of the
// same "forest".
// Otherwise creates a new tree.
#define ssdClone(root, desttree) _ssdClone(root, desttree, (SSDLockState*)_ssdCurrentLockState)

_Ret_opt_valid_
SSDNode *_ssdSubtree(_In_ SSDNode *root, _In_opt_ strref path, SSDCreateType create, _Inout_opt_ SSDLockState *lstate);
// SSDNode *ssdSubtree(SSDNode *root, strref path, int create);
// Returns a node representing a subtree
// If this node does not exist and the create parameter is any value other than SSD_Create_None,
// a node of the specified type is created at the given path.
// The caller inherits an object reference that must be freed with objRelease()
#define ssdSubtree(root, path, create) _ssdSubtree(root, path, create, (SSDLockState*)_ssdCurrentLockState)

_Ret_opt_valid_
SSDNode *_ssdSubtreeB(_In_ SSDNode *root, _In_opt_ strref path, _Inout_opt_ SSDLockState *lstate);
// SSDNode *ssdSubtreeB(SSDNode *root, strref path);
// Borrowed version of ssdSubtree that does not acquire an object reference.
// This version cannot be used standalone and must be part of a locked transaction.
// This version is intended for reading and cannot create nodes.
#define ssdSubtreeB(root, path) _ssdSubtreeB(root, path, (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)

// For convenience, the following functions take a PATH to be traversed.
// The syntax for a path consists of a series of names separated by '/',
// and/or array indices enclosed in []
//
// For example:
//
//     bucket/paints[2]/yellow
//
// Represents a path to a value with the key 'yellow', which is the third hashtable in an array of hashtables
// that is named 'paints' inside of an hashtable called 'bucket' inside of the unnamed root hashtable. To make
// it easier to understand, see the corresponding JSON representation:
//
// {
//     "bucket": {
//         "paints": [
//             {},
//             {}
//             { "yellow": "THIS VALUE HERE" }
//         ]
//     }
// }
//
// This path is for a structure that uses an array as its root:
//
//     [1]/mango
//
// Or this JSON:
//
// [
//     {},
//     { "mango": "THIS VALUE HERE" }
// ]
// 
// If automatic path traversal is not desired, the SSDNode interface can instead be directly used.

_Success_(return)
bool _ssdGet(_In_ SSDNode *root, _In_opt_ strref path, _Out_ stvar *out, _Inout_opt_ SSDLockState *lstate);
// bool ssdGet(SSDNode *root, strref path, stvar *out);
// CAUTION: The output stvar MUST be destroyed with stDestroy or it could cause a memory leak if the
// value is a string or if a hashtable or array exists at that name. To avoid the hassle of memory
// management, ssdPtr can be used instead.
#define ssdGet(root, path, out) _ssdGet(root, path, out, (SSDLockState*)_ssdCurrentLockState)

_Success_(return)
_meta_inline bool _ssdGetD(_In_ SSDNode *root, _In_opt_ strref path, _Out_ stvar *out, stvar def, _Inout_opt_ SSDLockState *lstate)
{
    if (_ssdGet(root, path, out, lstate))
        return true;

    stvarCopy(out, def);
    return false;
}
// bool ssdGetD(SSDNode *root, strref path, stvar *out, stvar def);
// A variant of ssdGet that fills in a default if the value does not exist. The function still
// returns false in that case.
#define ssdGetD(root, path, out, def) _ssdGetD(root, path, out, def, (SSDLockState*)_ssdCurrentLockState)

_Success_(return)
bool _ssdCopyOut(_In_ SSDNode *root, _In_opt_ strref path, stype valtype, _stCopyDest_Anno_(valtype) stgeneric *val, _Inout_opt_ SSDLockState *lstate);
// bool ssdCopyOut(SSDNode *root, strref path, stype valtype, stgeneric *val);
// Variant of ssdGet that copies the value out of the tree to an arbitrary destination, converting
// it in the process
#define ssdCopyOut(root, path, vtype, val_copy_out) _ssdCopyOut(root, path, stCheckedPtrArg(vtype, val_copy_out), (SSDLockState*)_ssdCurrentLockState)

_Success_(return)
bool _ssdCopyOutD(_In_ SSDNode *root, _In_opt_ strref path, stype valtype, _stCopyDest_Anno_(valtype) stgeneric *val, stgeneric def, _Inout_opt_ SSDLockState *lstate);
// bool ssdCopyOutD(SSDNode *root, strref path, stype valtype, stgeneric *val, stgeneric def);
// Variant of ssdGet that copies the value out of the tree to an arbitrary destination, converting
// it in the process. Copies the default if the value is not found or can't be converted.
#define ssdCopyOutD(root, path, vtype, val_copy_out, def) _ssdCopyOutD(root, path, stCheckedPtrArg(vtype, val_copy_out), stArg(vtype, def), (SSDLockState*)_ssdCurrentLockState)

bool _ssdSet(_Inout_ SSDNode *root, _In_opt_ strref path, bool createpath, stvar val, _Inout_opt_ SSDLockState *lstate);
// bool ssdSet(SSDNode *root, strref path, bool createpath, stvar val);
// If createpath is true, the path to the value is automatically created if necessary
#define ssdSet(root, path, createpath, val) _ssdSet(root, path, createpath, val, (SSDLockState*)_ssdCurrentLockState)

bool _ssdSetC(_Inout_ SSDNode *root, _In_opt_ strref path, bool createpath, _Pre_notnull_ _Post_invalid_ stvar *val, _Inout_opt_ SSDLockState *lstate);
// bool ssdSetC(SSDNode *root, strref path, bool createpath, stvar *val)
// Consumes the given value even on failure.
// If createpath is true, the path to the value is automatically created if necessary
#define ssdSetC(root, path, createpath, val) _ssdSetC(root, path, createpath, val, (SSDLockState*)_ssdCurrentLockState)

bool _ssdRemove(_Inout_ SSDNode *root, _In_opt_ strref path, _Inout_opt_ SSDLockState *lstate);
// bool ssdRemove(SSDNode *root, strref path);
#define ssdRemove(root, path) _ssdRemove(root, path, (SSDLockState*)_ssdCurrentLockState)

stvar *_ssdPtr(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ SSDLockState *lstate);
// stvar *ssdPtr(SSDNode *root, strref path);
// Unlike the other ssdtree functions, ssdPtr MUST be used within a locked transaction..
// That is because the pointer returned by this function is only guaranteed to be valid so long as
// the read or write lock is held.
#define ssdPtr(root, path) _ssdPtr(root, path, (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)

_Ret_opt_valid_
_meta_inline strref _ssdStrRef(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ SSDLockState *lstate)
{
    stvar *temp = _ssdPtr(root, path, lstate);
    return stvarIs(temp, string) ? temp->data.st_string : NULL;
}
// strref ssdStrRef(SSDNode *root, strref path);
// Gets a string -- lock is required as this will return a pointer directly to the string inside
// the internal storage!
#define ssdStrRef(root, path) _ssdStrRef(root, path, (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)

_Must_inspect_result_
_Ret_opt_valid_
_meta_inline ObjInst *_ssdObjInstPtr(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ SSDLockState *lstate)
{
    stvar *temp = _ssdPtr(root, path, lstate);
    return stvarIs(temp, object) ? temp->data.st_object : NULL;
}
// ObjInst *ssdObjInstPtr(SSDNode *root, strref path)
// Gets an object instance -- lock is required as this will return a pointer directly to the object
// inside the internal storage!
#define ssdObjInstPtr(root, path) _ssdObjInstPtr(root, path, (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)

// ClassName *ssdObjPtr(SSDNode *root, strref path, [ClassName])
#define ssdObjPtr(root, path, clsname) objDynCast(clsname, ssdObjInstPtr(root, path))

// convenience functions to get various types inline with a default
#define ssdval_spec(type) \
    _meta_inline stTypeDef(type) _ssdVal_##type(_In_ SSDNode *root, _In_opt_ strref path, stTypeDef(type) def, _Inout_opt_ SSDLockState *lstate)  \
    {                                                                                                                   \
        stTypeDef(type) out;                                                                                            \
        _ssdCopyOutD(root, path, stCheckedPtrArg(type, &out), stArg(type, def), lstate);                                \
        return out;                                                                                                     \
    }

ssdval_spec(bool)
ssdval_spec(int8)
ssdval_spec(int16)
ssdval_spec(int32)
ssdval_spec(int64)
ssdval_spec(uint8)
ssdval_spec(uint16)
ssdval_spec(uint32)
ssdval_spec(uint64)
ssdval_spec(float32)
ssdval_spec(float64)
#define ssdVal(type, root, path, def) \
    _ssdVal_##type(root, path, def, (SSDLockState*)_ssdCurrentLockState)

_meta_inline bool _ssdStringOut(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ string *out, _Inout_opt_ SSDLockState *lstate)
{
    strDestroy(out);
    return _ssdCopyOut(root, path, stCheckedPtrArg(string, out), lstate);
}
// bool ssdStringOut(SSDNode *root, strref path, string *out)
#define ssdStringOut(root, path, out) _ssdStringOut(root, path, out, (SSDLockState*)_ssdCurrentLockState)

_meta_inline bool _ssdStringOutD(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ string *out, _In_opt_ strref def, _Inout_opt_ SSDLockState *lstate)
{
    strDestroy(out);
    return _ssdCopyOutD(root, path, stCheckedPtrArg(string, out), stArg(strref, def), lstate);
}
// bool ssdStringOutD(SSDNode *root, strref path, string *out, strref def)
#define ssdStringOutD(root, path, out, def) _ssdStringOutD(root, path, out, def, (SSDLockState*)_ssdCurrentLockState)

// out must be an initialized array or a pointer to NULL.
bool _ssdExportArray(_In_ SSDNode *root, _In_opt_ strref path, _Inout_ sa_stvar *out, _Inout_opt_ SSDLockState *lstate);
// bool ssdExportArray(SSDNode *root, strref path, sa_stvar *out)
#define ssdExportArray(root, path, out) _ssdExportArray(root, path, out, (SSDLockState*)_ssdCurrentLockState)
bool _ssdImportArray(_Inout_ SSDNode *root, _In_opt_ strref path, _In_ sa_stvar arr, _Inout_opt_ SSDLockState *lstate);
// bool ssdImportArray(SSDNode *root, strref path, sa_stvar arr)
#define ssdImportArray(root, path, arr) _ssdImportArray(root, path, arr, (SSDLockState*)_ssdCurrentLockState)

bool _ssdExportTypedArray(_In_ SSDNode *root, _In_opt_ strref path, stype elemtype, _Inout_ sahandle out, bool strict, _Inout_opt_ SSDLockState *lstate);
bool _ssdImportTypedArray(_Inout_ SSDNode *root, _In_opt_ strref path, stype elemtype, _In_ sa_ref arr, _Inout_opt_ SSDLockState *lstate);

// bool ssdExportTypedArray(SSDNode *root, strref path, stype elemtype, sahandle out, bool strict)
// out must be an initialized array or a pointer to NULL.
// If strict is set, will cause the export to fail if any items of the wrong type are encountered,
// otherwise they are simply skipped over.
#define ssdExportTypedArray(root, path, type, out, strict) _ssdExportTypedArray(root, path, stType(type), SAHANDLE(out), strict, (SSDLockState*)_ssdCurrentLockState)
// bool ssdImportTypedArray(SSDNode *root, strref path, stype elemtype, sa_ref arr)
#define ssdImportTypedArray(root, path, type, arr) _ssdImportTypedArray(root, path, stType(type), SAREF(arr), (SSDLockState*)_ssdCurrentLockState)

int32 _ssdCount(_In_ SSDNode *root, _In_opt_ strref path, bool arrayonly, _Inout_opt_ SSDLockState *lstate);
// int32 ssdCount(SSDNode * root, strref path, bool arrayonly)
// Returns the number of children of the given path.
// If arrayonly is true, ssdCount will return 0 for hashtable nodes.
#define ssdCount(root, path, arrayonly) _ssdCount(root, path, arrayonly, (SSDLockState*)_ssdCurrentLockState)

_Ret_opt_valid_
stvar *_ssdIndex(_In_ SSDNode *root, _In_opt_ strref path, int32 idx, _Inout_ SSDLockState *lstate);
// stvar *ssdIndex(SSDNode *root, strref path, int32 idx)
// Gets a pointer to a given array index.
// Like ssdPtr this requires lock state to be passed.
#define ssdIndex(root, path, idx) _ssdIndex(root, path, idx, (SSDLockState*)&_ssdCurrentLockState->_is_SSDLockState)

bool _ssdAppend(_Inout_ SSDNode *root, _In_opt_ strref path, bool createpath, stvar val, _Inout_opt_ SSDLockState *lstate);
// bool ssdAppend(SSDNode *root, strref path, bool createpath, stvar val)
// Similar to SSDSet, but appends the value to an array at the given path, creating the array
// if necessary and createpath is true.
#define ssdAppend(root, path, createpath, val) _ssdAppend(root, path, createpath, val, (SSDLockState*)_ssdCurrentLockState)

bool _ssdGraft(_Inout_ SSDNode *dest, _In_opt_ strref destpath, _Inout_opt_ SSDLockState *dest_lstate, _In_ SSDNode *src, _In_opt_ strref srcpath, _Inout_opt_ SSDLockState *src_lstate);
// bool ssdGraft(SSDNode *dest, strref destpath, SSDNode *src, strref srcpath)
// Grafts a subtree of the source tree onto the dest tree.
// This deep copies the tree and associates the resulting nodes with the destination tree (creating them
// using its customized node classes if needed).
// If this is used within a locked transaction, it must be one for the DESTINATION tree. The source tree
// is always locked in read-only mode using a transient lock and must NOT be currently locked.
#define ssdGraft(dest, destpath, src, srcpath) _ssdGraft(dest, destpath, (SSDLockState*)_ssdCurrentLockState, src, srcpath, NULL)

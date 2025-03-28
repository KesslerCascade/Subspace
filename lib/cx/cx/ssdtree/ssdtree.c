#include "ssdtree.h"
#include <cx/container.h>
#include <cx/string.h>
#include "node/ssdarraynode.h"
#include "node/ssdhashnode.h"
#include "node/ssdsinglenode.h"

#ifdef SSD_LOCK_DEBUG
#include <cx/format.h>
#include <cx/time.h>
_Use_decl_annotations_
SSDLockState *__ssdLockStateInit(SSDLockState *lstate, const char *fn, int lnum)
#else
_Use_decl_annotations_
SSDLockState *_ssdLockStateInit(SSDLockState *lstate)
#endif
{
#ifdef SSD_LOCK_DEBUG
    lstate->dbg.time = clockTimer();
    lstate->dbg.file = fn;
    lstate->dbg.line = lnum;
#endif
    lstate->init = true;
    lstate->rdlock = lstate->wrlock = false;
    return lstate;
}

#ifdef SSD_LOCK_DEBUG
_Use_decl_annotations_
bool _ssdLockRead(SSDNode *root, SSDLockState *lstate, const char *fn, int lnum)
#else
_Use_decl_annotations_
bool _ssdLockRead(SSDNode *root, SSDLockState *lstate)
#endif
{
    if (!root || !lstate)
        return false;
    if (!lstate->init)
#ifdef SSD_LOCK_DEBUG
        __ssdLockStateInit(lstate, fn, lnum);
#else
        _ssdLockStateInit(lstate);
#endif

    if (lstate->rdlock || lstate->wrlock)
        return true;

    rwlockAcquireRead(&root->tree->lock);
    lstate->rdlock = true;
#ifdef SSD_LOCK_DEBUG
    mutexAcquire(&root->tree->dbg.mtx);
    saPush(&root->tree->dbg.readlocks, opaque, lstate->dbg);
    mutexRelease(&root->tree->dbg.mtx);
#endif

    return true;
}

#ifdef SSD_LOCK_DEBUG
_Use_decl_annotations_
bool _ssdLockWrite(SSDNode *root, SSDLockState *lstate, const char *fn, int lnum)
#else
_Use_decl_annotations_
bool _ssdLockWrite(SSDNode *root, SSDLockState *lstate)
#endif
{
    if (!root || !lstate)
        return false;
    if (!lstate->init)
#ifdef SSD_LOCK_DEBUG
        __ssdLockStateInit(lstate, fn, lnum);
#else
        _ssdLockStateInit(lstate);
#endif

    if (lstate->wrlock)
        return true;

    if (lstate->rdlock) {
        rwlockReleaseRead(&root->tree->lock);
#ifdef SSD_LOCK_DEBUG
        mutexAcquire(&root->tree->dbg.mtx);
        saFindRemove(&root->tree->dbg.readlocks, opaque, lstate->dbg);
        mutexRelease(&root->tree->dbg.mtx);
#endif
        lstate->rdlock = false;
    }

    rwlockAcquireWrite(&root->tree->lock);
#ifdef SSD_LOCK_DEBUG
    mutexAcquire(&root->tree->dbg.mtx);
    saPush(&root->tree->dbg.writelocks, opaque, lstate->dbg);
    mutexRelease(&root->tree->dbg.mtx);
#endif
    lstate->wrlock = true;

    return true;
}

_Use_decl_annotations_
bool _ssdUnlock(SSDNode *root, SSDLockState *lstate)
{
    if (!root || !lstate || !lstate->init)
        return false;

    _Analysis_assume_lock_held_(root->tree->lock);

    if (lstate->wrlock)
        rwlockReleaseWrite(&root->tree->lock);
    else if (lstate->rdlock)
        rwlockReleaseRead(&root->tree->lock);

#ifdef SSD_LOCK_DEBUG
    mutexAcquire(&root->tree->dbg.mtx);
    if (lstate->wrlock)
        saFindRemove(&root->tree->dbg.writelocks, opaque, lstate->dbg);
    else if (lstate->rdlock)
        saFindRemove(&root->tree->dbg.readlocks, opaque, lstate->dbg);
    mutexRelease(&root->tree->dbg.mtx);
#endif

    lstate->wrlock = lstate->rdlock = false;

    return true;
}

_Use_decl_annotations_
bool _ssdLockEnd(SSDNode *root, SSDLockState *lstate)
{
    if (!lstate)
        return false;
    bool ret = _ssdUnlock(root, lstate);
    lstate->init = false;
    return ret;
}

_Use_decl_annotations_
SSDNode *_ssdCreateRoot(int crtype, SSDTree *tree, uint32 flags)
{
    bool created = false;

    if (!tree) {
        created = true;
        tree = ssdtreeCreate(flags);
    }

    SSDNode *ret = ssdtreeCreateNode(tree, crtype);

    if (created)
        objRelease(&tree);

    return ret;
}

_Ret_opt_valid_
static SSDNode *getChild(_In_ SSDNode *node, _In_opt_ strref name, int checktype, int create, _Inout_ SSDLockState *_ssdCurrentLockState)
{

    ssdLockRead(node);
    bool havewrite;
    SSDNode *ret = NULL;

    do {
        havewrite = _ssdCurrentLockState->wrlock;

        stvar *val = ssdnodePtr(node, SSD_ByName, name, _ssdCurrentLockState);
        SSDNode *child = val ? (stEq(val->type, stType(object)) ? objDynCast(SSDNode, val->data.st_object) : NULL) : NULL;

        // check if existing child is the correct type for the context
        if (child && checktype == SSD_Create_Hashtable && !ssdnodeIsHashtable(child))
            child = NULL;
        if (child && checktype == SSD_Create_Array && !ssdnodeIsArray(child))
            child = NULL;

        // there's a valid child there with the correct type, just return it
        if (child)
            return child;

        // didn't exist (or was the wrong type), so create it
        if (create != SSD_Create_None) {
            ssdLockWrite(node);

            // check again with write lock held, since we do drop the lock
            // briefly, another thread may have created it
            if (!havewrite)
                continue;

            ret = ssdtreeCreateNode(node->tree, create);
            if (!ret)
                return NULL;

            // create or overwrite
            ssdnodeSetC(node, SSD_ByName, name, &stvar(object, ret), _ssdCurrentLockState);
            // return a borrowed reference to the node that was just inserted into the tree
            return ret;
        }
    } while ((create != SSD_Create_None) && !havewrite);    // loop if we're in create mode but didn't have the write lock

    return ret;
}

_Success_(return)
static bool ssdResolvePath(_In_ SSDNode *root, _In_opt_ strref path, _Out_ SSDNode **nodeout, _Inout_ string *nameout, bool create, _Inout_ SSDLockState *_ssdCurrentLockState)
{
    SSDNode *node = root;
    string name = 0;
    int arrstate = 0;
    bool ret = false;
    bool first = true;

    ssdLockRead(root);

    foreach(string, it, path) {
        for (uint32 i = 0; i < it.len; ++i) {
            uint8 ch = it.bytes[i];
            if (arrstate == 1 && !((ch >= '0' && ch <= '9') || ch == ']'))
                // array index parse error
                goto out;

            // name can start with [#] for index of root
            if (!first && (ch == '/' || ch == '[')) {
                // hit a parse point, we need to move down a level

                // check if existing child is the correct type for the context
                int childtype = 0;
                if (ch == '/')
                    childtype = SSD_Create_Hashtable;
                if (ch == '[')
                    childtype = SSD_Create_Array;

                SSDNode *child = getChild(node, name, childtype, create ? childtype : SSD_Create_None, _ssdCurrentLockState);
                if (!child) {
                    // failed to recurse, did not create
                    goto out;
                }

                // rotate child into 'current' node
                node = child;
                strClear(&name);
            }

            if (ch == '/') {
                arrstate = 0;
            } else if (ch == '[') {
                arrstate = 1;
            } else if (ch == ']') {
                if (arrstate != 1)
                    goto out;               // parse error!
                arrstate = 2;
            } else {
                if (arrstate == 2)
                    goto out;               // parse error, raw text can't come directly after ']'!
                strSetChar(&name, strEnd, ch);
            }

            first = false;
        }
    }

    ret = true;

out:
    if (ret) {
        *nodeout = node;
        strDup(nameout, name);
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
bool _ssdGet(SSDNode *root, strref path, stvar *out, SSDLockState *_ssdCurrentLockState)
{
    string name = 0;
    bool ret = false;

    ssdLockedTransaction(root)
    {
        SSDNode *node = NULL;
        if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState))
            ret = ssdnodeGet(node, SSD_ByName, name, out, _ssdCurrentLockState);
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
stvar *_ssdPtr(SSDNode *root, strref path, SSDLockState *_ssdCurrentLockState)
{
    SSDNode *node = NULL;
    string name = 0;
    stvar *ret = NULL;

    if (!devVerifyMsg(_ssdCurrentLockState, "ssdPtr must be used within a locked transaction"))
        return NULL;

    if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState))
        ret = ssdnodePtr(node, SSD_ByName, name, _ssdCurrentLockState);

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
bool _ssdSet(SSDNode *root, strref path, bool createpath, stvar val, SSDLockState *_ssdCurrentLockState)
{
    string name = 0;
    bool ret = false;

    ssdLockedTransaction(root)
    {
        // we know we're going to need the write lock for the duration regardless
        ssdLockWrite(root);

        SSDNode *node = NULL;
        if (ssdResolvePath(root, path, &node, &name, createpath, _ssdCurrentLockState)) {
            ret = ssdnodeSet(node, SSD_ByName, name, val, _ssdCurrentLockState);
        }
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
bool _ssdSetC(SSDNode *root, strref path, bool createpath, stvar *val, SSDLockState *_ssdCurrentLockState)
{
    string name = 0;
    bool ret = false;

    ssdLockedTransaction(root)
    {
        // we know we're going to need the write lock for the duration regardless
        ssdLockWrite(root);

        SSDNode *node = NULL;
        if (ssdResolvePath(root, path, &node, &name, createpath, _ssdCurrentLockState)) {
            ret = ssdnodeSetC(node, SSD_ByName, name, val, _ssdCurrentLockState);
        } else {
            stvarDestroy(val);
        }
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
bool _ssdRemove(SSDNode *root, strref path, SSDLockState *_ssdCurrentLockState)
{
    string name = 0;
    bool ret = false;

    ssdLockedTransaction(root)
    {
        // this is likely going to need the write lock, even if it technically
        // doesn't 100% of the time (i.e. if the key doesn't exist)
        ssdLockWrite(root);

        SSDNode *node = NULL;
        if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState)) {
            ret = ssdnodeRemove(node, SSD_ByName, name, _ssdCurrentLockState);
        }
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
SSDNode *_ssdSubtree(SSDNode *root, strref path, SSDCreateType create, SSDLockState *_ssdCurrentLockState)
{
    SSDNode *ret = NULL;
    string name = 0;

    ssdLockedTransaction(root)
    {
        SSDNode *node = NULL;

        if (ssdResolvePath(root, path, &node, &name, create != SSD_Create_None, _ssdCurrentLockState)) {
            ret = objAcquire(getChild(node, name, create, create, _ssdCurrentLockState));
        }
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
SSDNode *_ssdSubtreeB(SSDNode *root, strref path, SSDLockState *_ssdCurrentLockState)
{
    if (!devVerifyMsg(_ssdCurrentLockState, "ssdSubtreeB must be used within a locked transaction"))
        return NULL;

    SSDNode *ret = NULL;
    SSDNode *node = NULL;
    string name = 0;

    ssdLockRead(root);
    if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState)) {
        stvar *val = ssdnodePtr(node, SSD_ByName, name, _ssdCurrentLockState);
        ret = val ? (stEq(val->type, stType(object)) ? objDynCast(SSDNode, val->data.st_object) : NULL) : NULL;
    }

    strDestroy(&name);
    return ret;
}

_Use_decl_annotations_
bool _ssdCopyOut(SSDNode *root, strref path, stype valtype, stgeneric *val, SSDLockState *_ssdCurrentLockState)
{
    bool ret = false;
    string name = 0;

    ssdLockedTransaction(root)
    {
        SSDNode *node = NULL;
        stvar *temp = NULL;
        if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState))
            temp = ssdnodePtr(node, SSD_ByName, name, _ssdCurrentLockState);

        if (temp)
            ret = _stConvert(valtype, val, temp->type, NULL, temp->data, 0);
    }
    strDestroy(&name);

    return ret;
}

_Use_decl_annotations_
bool _ssdCopyOutD(SSDNode *root, strref path, stype valtype, stgeneric *val, stgeneric def, SSDLockState *_ssdCurrentLockState)
{
    bool ret = false;
    string name = 0;

    ssdLockedTransaction(root)
    {
        SSDNode *node = NULL;
        stvar *temp = NULL;
        if (ssdResolvePath(root, path, &node, &name, false, _ssdCurrentLockState))
            temp = ssdnodePtr(node, SSD_ByName, name, _ssdCurrentLockState);

        if (temp)
            ret = _stConvert(valtype, val, temp->type, NULL, temp->data, 0);
    }
    strDestroy(&name);

    if (!ret)
        _stCopy(valtype, NULL, val, def, 0);

    return ret;
}

_Use_decl_annotations_
bool _ssdExportArray(SSDNode *root, strref path, sa_stvar *out, SSDLockState *_ssdCurrentLockState)
{
    bool ret = false;

    ssdLockedTransaction(root)
    {
        ssdLockRead(root);
        SSDArrayNode *node = ssdObjPtr(root, path, SSDArrayNode);
        if (node) {
            saDestroy(out);
            saClone(out, node->storage);
            ret = true;
        }
    }

    return ret;
}

_Use_decl_annotations_
bool _ssdExportTypedArray(SSDNode *root, strref path, stype elemtype, sahandle out, bool strict, SSDLockState *_ssdCurrentLockState)
{
    bool ret = false;

    ssdLockedTransaction(root)
    {
        ssdLockRead(root);
        SSDArrayNode *node = ssdObjPtr(root, path, SSDArrayNode);
        if (node) {
            saClear(out);
            for (int i = 0, sz = saSize(node->storage); i < sz; ++i) {
                if (!stEq(node->storage.a[i].type, elemtype)) {
                    // is the same type as the output array?
                    if (strict)
                        goto out;
                    continue;               // just skip over
                }

                _saPushPtr(out, elemtype, stStoredPtr(elemtype, &node->storage.a[i].data), 0);
            }
            ret = true;
        }
out:
        continue;
    }

    if (!ret)
        saClear(out);

    return ret;
}

_Use_decl_annotations_
bool _ssdImportArray(SSDNode *root, strref path, sa_stvar arr, SSDLockState *_ssdCurrentLockState)
{
    devAssert(stEq(saElemType(arr), stType(stvar)));            // double check this is the right kind of array

    bool ret = false;

    ssdLockedTransaction(root)
    {
        ssdLockWrite(root);
        SSDNode *stree = ssdSubtree(root, path, SSD_Create_Array);
        SSDArrayNode *node = objDynCast(SSDArrayNode, stree);
        if (node) {
            saDestroy(&node->storage);
            saClone(&node->storage, arr);
            ssdnodeUpdateModified(node);
            ret = true;
        }
        objRelease(&stree);
    }

    return ret;
}

_Use_decl_annotations_
bool _ssdImportTypedArray(SSDNode *root, strref path, stype elemtype, sa_ref arr, SSDLockState *_ssdCurrentLockState)
{
    devAssert(stEq(saElemType(arr), elemtype));             // double check this is the right kind of array

    bool ret = false;

    ssdLockedTransaction(root)
    {
        stvar val = { .type = elemtype };

        ssdLockWrite(root);
        SSDNode *stree = ssdSubtree(root, path, SSD_Create_Array);
        SSDArrayNode *node = objDynCast(SSDArrayNode, stree);
        if (node) {
            saClear(&node->storage);
            for (int i = 0, sz = saSize(arr); i < sz; ++i) {
                val.data = stStored(elemtype, (void *)((uintptr)arr.a + (uintptr)i * saElemSize(arr)));       // low-budget ELEMPTR
                saPush(&node->storage, stvar, val);
            }
            ssdnodeUpdateModified(node);
            ret = true;
        }
        objRelease(&stree);
    }

    return ret;
}

_Ret_opt_valid_
static SSDNode *ssdCloneNode(_In_ SSDNode *src, _Inout_opt_ SSDLockState *srclstate, _In_ SSDTree *desttree, _Inout_opt_ SSDLockState *destlstate)
{
    SSDNode *dnode;
    if (ssdnodeIsHashtable(src))
        dnode = ssdtreeCreateNode(desttree, SSD_Create_Hashtable);
    else if (ssdnodeIsArray(src))
        dnode = ssdtreeCreateNode(desttree, SSD_Create_Array);
    else
        return NULL;

    SSDIterator *iter = ssdnode_iterLocked(src, srclstate);
    for (; ssditeratorValid(iter); ssditeratorNext(iter)) {
        int32 srcidx = 0;
        strref srcname = 0;
        stvar *val = NULL;
        stvar valcopy;
        if (!ssditeratorIterOut(iter, &srcidx, &srcname, &val))
            break;

        valcopy        = *val;
        SSDNode* child = stvarObj(SSDNode, val);
        if (child) {
            // if this is a node, clone it first
            valcopy.data.st_object = (ObjInst *)ssdCloneNode(child, srclstate, desttree, destlstate);
        }

        ssdnodeSet(dnode, srcidx, srcname, valcopy, destlstate);

        if (child) {
            // don't hang onto the node we cloned above
            objRelease(&valcopy.data.st_object);
        }
    }
    objRelease(&iter);

    return dnode;
}

_Use_decl_annotations_
SSDNode *_ssdClone(SSDNode *root, SSDTree *desttree, SSDLockState *_ssdCurrentLockState)
{
    SSDNode *ret = NULL;
    bool createdtree = false;
    if (!desttree) {
        desttree = ssdtreeCreate(0);
        createdtree = true;
    }

    ssdLockedTransaction(root)
    {
        // lock source tree while we're working on it
        ssdLockRead(root);

        // manually create lock state for destination, inherent state already used for source
        SSDLockState *destlock;
        SSDLockState transient_destlock;
        _ssdLockStateInit(&transient_destlock);

        // if the source tree is the same as the destination, they should use the same lock
        destlock = (root->tree == desttree) ? _ssdCurrentLockState : &transient_destlock;

        ret = ssdCloneNode(root, _ssdCurrentLockState, desttree, destlock);
        if (ret)
            _ssdLockEnd(ret, &transient_destlock);
    }

    if (createdtree) {
        objRelease(&desttree);
    }

    return ret;
}

_Use_decl_annotations_
bool _ssdGraft(SSDNode *dest, strref destpath, SSDLockState *dest_lstate, SSDNode *src, strref srcpath, SSDLockState *src_lstate)
{
    // have to do this the old fashioned way; _ssdCurrentLockState only works for one at a time
    SSDLockState transient_lock_state_dest = { 0 };
    if (!dest_lstate) dest_lstate = &transient_lock_state_dest;
    SSDLockState transient_lock_state_src = { 0 };
    if (!src_lstate) src_lstate = &transient_lock_state_src;

    bool ret = false;

    _ssdManualLockRead(src, src_lstate);
    _ssdManualLockWrite(dest, dest_lstate);

    SSDNode *stree;
    SSDNode *dpnode = NULL;
    string dname = 0;

    if (!strEmpty(srcpath))
        stree = _ssdSubtreeB(src, srcpath, src_lstate);
    else
        stree = src;

    if (!stree)
        goto out;

    if (!ssdResolvePath(dest, destpath, &dpnode, &dname, true, dest_lstate))
        goto out;

    SSDNode *dnode = ssdCloneNode(stree, src_lstate, dest->tree, dest_lstate);
    if (dnode) {
        ret = ssdnodeSet(dpnode, SSD_ByName, dname, stvar(object, dnode), dest_lstate);
        objRelease(&dnode);
    }

out:
    strDestroy(&dname);
    if (transient_lock_state_src.init)
        _ssdLockEnd(src, &transient_lock_state_src);
    if (transient_lock_state_dest.init)
        _ssdLockEnd(dest, &transient_lock_state_dest);

    return ret;
}

_Use_decl_annotations_
int32 _ssdCount(SSDNode *root, strref path, bool arrayonly, SSDLockState *_ssdCurrentLockState)
{
    int32 ret = 0;

    ssdLockedTransaction(root)
    {
        SSDNode *node;
        if (strEmpty(path))
            node = root;
        else
            node = ssdSubtreeB(root, path);

        if (node && (!arrayonly || ssdnodeIsArray(node)))
            ret = ssdnodeCount(node, _ssdCurrentLockState);
    }

    return ret;
}

_Use_decl_annotations_
stvar *_ssdIndex(SSDNode *root, strref path, int32 idx, SSDLockState *_ssdCurrentLockState)
{
    if (!devVerifyMsg(_ssdCurrentLockState, "ssdIndex must be used within a locked transaction"))
        return NULL;                            // lock parameter is mandatory

    SSDNode *node;
    if (strEmpty(path))
        node = root;
    else
        node = ssdSubtreeB(root, path);

    if (node)
        return ssdnodePtr(node, idx, NULL, _ssdCurrentLockState);

    return NULL;
}

_Use_decl_annotations_
bool _ssdAppend(SSDNode *root, strref path, bool createpath, stvar val, SSDLockState *_ssdCurrentLockState)
{
    bool ret = false;

    ssdLockedTransaction(root)
    {
        SSDNode *node = NULL;
        if (strEmpty(path)) {
            node = objAcquire(root);
        } else {
            node = ssdSubtree(root, path, createpath ? SSD_Create_Array : SSD_Create_None);
        }

        SSDArrayNode *arrn = objDynCast(SSDArrayNode, node);
        if (arrn)
            ret = ssdarraynodeAppend(arrn, val, _ssdCurrentLockState);
        objRelease(&node);
    }

    return ret;
}

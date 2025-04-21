#include "settings_private.h"
#include <cx/fs.h>
#include <cx/serialize/jsonparse.h>
#include <cx/serialize/sbfile.h>
#include <cx/ssdtree/ssdnodes.h>
#include <cx/string.h>
#include "settings.h"

SSDNode* setsOpen(VFS* vfs, strref path, int64 flush_interval)
{
    SettingsTree* tree = settingstreeCreate();
    StreamBuffer* sb   = NULL;
    SSDNode* ret       = NULL;
    VFSFile* file      = NULL;

    if (flush_interval > 0)
        tree->interval = flush_interval;

    tree->vfs = objAcquire(vfs);
    strDup(&tree->filename, path);

    file = vfsOpen(tree->vfs, tree->filename, FS_Read);
    if (!file)
        goto out;

    sb = sbufCreate(256);
    if (!sbufFilePRegisterPull(sb, file, false))
        goto out;
    ret = jsonParseTreeCustom(sb, SSDTree(tree));

    // Sanity check, for settings we require the root to be a hashtable using our derived class.
    // If it's not, because somebody replaced the json with one that has an array at the root or
    // something, nuke it from orbit.
    if (!objDynCast(SettingsHashNode, ret))
        objRelease(&ret);

out:
    sbufRelease(&sb);

    // if we failed to open or parse the file, create an empty root node instead
    if (!ret)
        ret = ssdCreateCustom(SSD_Create_Hashtable, SSDTree(tree));

    // Make sure saved time is newer than any modified times from loading the tree
    tree->saved = clockTimer();

    // fire up settings thread to monitor it
    _setsThreadCheck();
    _setsThreadWatch(ret);

    vfsClose(file);
    objRelease(&tree);

    return ret;
}

bool setsBind(SSDNode* sets, SetsBindSpec* bindings, void* base)
{
    if (!sets || !bindings)
        return false;

    string pname = 0;
    string sname = 0;
    bool ret     = true;

    ssdLockedTransaction(sets)
    {
        for (SetsBindSpec* cur = bindings; cur->name; cur++) {
            SSDNode* parent;
            int32 slash = strFindR(cur->name, strEnd, _S"/");
            if (slash != -1) {
                strSubStr(&pname, cur->name, 0, slash);
                strSubStr(&sname, cur->name, slash + 1, strEnd);
                parent = ssdSubtree(sets, pname, SSD_Create_Hashtable);
            } else {
                strDup(&sname, cur->name);
                parent = objAcquire(sets);
            }

            if (!parent)
                continue;

            SettingsHashNode* shn = objDynCast(SettingsHashNode, parent);
            if (shn)
                ret &= settingshashnodeBind(shn,
                                            sname,
                                            cur->deftyp.type,
                                            ((uint8*)base) + cur->offset,
                                            cur->deftyp.data,
                                            _ssdCurrentLockState);
            else
                ret = false;

            objRelease(&parent);
        }
    }

    strDestroy(&pname);
    strDestroy(&sname);
    return ret;
}

void setsUnbindAll(SSDNode* sets)
{
    // Root node being a hashtable is required for settings;
    // our tree factory override should take care of that
    ssdLockedTransaction(sets)
    {
        SettingsHashNode* node = objDynCast(SettingsHashNode, sets);
        if (node)
            settingshashnodeUnbindAll(node, _ssdCurrentLockState);
    }
}

// TODO: loads and saves settings into a variable without actually binding it
bool setsImport(SSDNode* sets, SetsBindSpec* bindings, void* base)
{
    return false;
}
bool setsExport(SSDNode* sets, SetsBindSpec* bindings, void* base)
{
    return false;
}

void setsSetDirty(SSDNode* sets)
{
    SettingsTree* tree = sets ? objDynCast(SettingsTree, sets->tree) : NULL;
    if (!tree)
        return;

    ssdLockedTransaction(sets)
    {
        ssdLockWrite(sets);
        // this will cause setsFlush to always think that the in-memory copy is newer than the disk
        // copy
        tree->saved = 0;
    }
}

// forces a full rescan of bound variables at next flush interval
void setsCheckBound(SSDNode* sets)
{
    SettingsTree* tree = sets ? objDynCast(SettingsTree, sets->tree) : NULL;
    if (!tree)
        return;

    ssdLockedTransaction(sets)
    {
        ssdLockWrite(sets);
        tree->checkbound = true;
    }
}

static bool flushAll(SSDNode* sets, SettingsTree* tree, bool checkbound,
                     SSDLockState* _ssdCurrentLockState)
{
    bool ret = false;
    ssdLockRead(sets);

    if (!tree->vfs) {
        ssdUnlock(sets);
        return false;
    }

    // last chance to check bound variables for changes
    SettingsHashNode* node = objDynCast(SettingsHashNode, sets);
    if (node && checkbound) {
        settingshashnodeCheckAll(node, _ssdCurrentLockState);
        // ok to update this with only read lock held
        tree->checkbound = false;
    }

    // see if anything actually changed
    if (tree->modified <= tree->saved)
        return false;

    // will need the write lock for this
    ssdLockWrite(sets);
    tree->saved = clockTimer();
    return _setsWriteTree(sets, tree, _ssdCurrentLockState);
    return ret;
}

bool setsFlush(SSDNode* sets)
{
    SettingsTree* tree = sets ? objDynCast(SettingsTree, sets->tree) : NULL;
    if (!tree)
        return false;

    ssdLockedTransaction(sets)
    {
        flushAll(sets, tree, tree->checkbound, _ssdCurrentLockState);
    }

    return true;
}

bool setsClose(SSDNode** psets)
{
    SSDNode* sets      = psets ? *psets : NULL;
    SettingsTree* tree = sets ? objDynCast(SettingsTree, sets->tree) : NULL;
    if (!tree)
        return false;

    _setsThreadForget(sets);
    ssdLockedTransaction(sets)
    {
        ssdLockWrite(sets);

        // force check of bound variables at close
        flushAll(sets, tree, true, _ssdCurrentLockState);

        // ensure nobody can hold onto a reference and save this again
        objRelease(&tree->vfs);
    }

    objRelease(psets);
    return true;
}

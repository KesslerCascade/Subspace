// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gamedata/gamedata.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <pkgfs/pkgfs.h>
#include "gamedata/loadtext.h"

_objfactory_guaranteed GameData* GameData_create()
{
    GameData* self;
    self = objInstCreate(GameData);

    self->loaded = trgateCreate(_S"Game Data Loaded");

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool GameData_init(_In_ GameData* self)
{
    // Autogen begins -----
    rwlockInit(&self->lock);
    htInit(&self->text, string, string, 16);
    return true;
    // Autogen ends -------
}

GameDataState GameData_state(_In_ GameData* self)
{
    GameDataState ret;
    withReadLock (&self->lock) {
        ret = self->_state;
    }

    return ret;
}

bool GameData_load(_In_ GameData* self, Subspace* ss, VFS* parentvfs, _In_opt_ strref ftldir)
{
    bool ret = false;
    GameDataState gds;
    string fname     = 0;
    VFSFile* datfile = NULL;
    PkgFile* pkg     = NULL;
    PkgFS* pkgfs     = NULL;

    withWriteLock (&self->lock) {
        gds = self->_state;
        if (gds == GD_Init)
            self->_state = GD_Loading;
    }
    if (gds != GD_Init)
        goto out;

    // go ahead and try to mount VFS
    self->ftldat = vfsCreate(VFS_ReadOnly);
    pathJoin(&fname, ftldir, FTLDAT_FILENAME);
    datfile = vfsOpen(parentvfs, fname, FS_Read);
    if (!datfile)
        goto out;

    pkg = pkgfileOpen(datfile);
    if (!pkg)
        goto out;

    pkgfs = pkgfsCreate(pkg);
    if (vfsMountProvider(self->ftldat, pkgfs, _S"/"))
        ret = true;

    GameDataLoader* gdl = gamedataloaderCreate(ss, self);
    tqRun(ss->workq, &gdl);

out:
    if (!ret) {
        withWriteLock (&self->lock) {
            self->_state = GD_Error;
            trgateOpen(self->loaded);
        }

        if (datfile)
            vfsClose(datfile);
    }
    objRelease(&pkgfs);
    objRelease(&pkg);
    strDestroy(&fname);

    return ret;
}

void GameData_wait(_In_ GameData* self, ComplexTask* task)
{
    withReadLock (&self->lock) {
        if (self->_state == GD_Loading) {
            ctaskRequireGate(task, self->loaded);
        }
    }
}

strref GameData_getTextD(_In_ GameData* self, _In_opt_ strref key, _In_opt_ strref def)
{
    strref ret = def;

    withReadLock (&self->lock) {
        if (self->_state == GD_Available) {
            htelem elem;

            elem = htFind(self->text, strref, key, none, NULL);
            if (elem)
                ret = hteVal(self->text, strref, elem);
        }
    }

    return ret;
}

strref GameData_getText(_In_ GameData* self, _In_opt_ strref key)
{
    return GameData_getTextD(self, key, key);
}

void GameData_destroy(_In_ GameData* self)
{
    // Autogen begins -----
    objRelease(&self->ftldat);
    rwlockDestroy(&self->lock);
    objRelease(&self->loaded);
    htDestroy(&self->text);
    // Autogen ends -------
}

_objfactory_guaranteed GameDataLoader* GameDataLoader_create(Subspace* ss, GameData* data)
{
    GameDataLoader* self;
    self = objInstCreate(GameDataLoader);

    self->ss   = ss;
    self->data = objAcquire(data);

    objInstInit(self);

    ComplexTask* task;

    task = ComplexTask(loadtextCreate(ss, data));
    ctaskRequireTask(self, task, false);
    tqRun(ss->workq, &task);

    return self;
}

uint32 GameDataLoader_run(_In_ GameDataLoader* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                          _Inout_ TaskControl* tcon)
{
    // By the time this is called; all sub-tasks have completed successfully

    withWriteLock (&self->data->lock) {
        self->data->_state = GD_Available;
        trgateOpen(self->data->loaded);
    }

    subspaceUpdateUI(self->ss);

    return TASK_Result_Success;
}

void GameDataLoader_destroy(_In_ GameDataLoader* self)
{
    // Autogen begins -----
    objRelease(&self->data);
    // Autogen ends -------
}

// Autogen begins -----
// clang-format off
#include "gamedata/gamedata.auto.inc"
// clang-format on
// Autogen ends -------

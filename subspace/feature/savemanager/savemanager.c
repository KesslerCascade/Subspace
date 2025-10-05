// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/savemanager/savemanager.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <cx/utils.h>
#include "db/database.h"
#include "feature/featureregistry.h"
#include "feature/savemanager/savemanagerpage.h"
#include "run/runinfo.h"
#include "util/pathfilter.h"
#include "util/pathsubst.h"

static LazyInitState savefilevars_init_state;
static hashtable savefilevars;

static void savefilevars_init(void* dummy)
{
    htInit(&savefilevars, string, string, 20);
    htInsert(&savefilevars, string, _S"year", string, _S"${0uint(4):year;0000}");
    htInsert(&savefilevars, string, _S"month", string, _S"${0uint(2):month;00}");
    htInsert(&savefilevars, string, _S"day", string, _S"${0uint(2):day;00}");
    htInsert(&savefilevars, string, _S"hour", string, _S"${0uint(2):hour;00}");
    htInsert(&savefilevars, string, _S"minute", string, _S"${0uint(2):minute;00}");
    htInsert(&savefilevars, string, _S"second", string, _S"${0uint(2):second;00}");
    htInsert(&savefilevars, string, _S"shipname", string, _S"${string:shipname;The Nameless One}");
    htInsert(&savefilevars, string, _S"shiptype", string, _S"${string:shiptype;Unknown A}");
    htInsert(&savefilevars, string, _S"beacon", string, _S"${string:beacon;0}");
    htInsert(&savefilevars, string, _S"sector", string, _S"${string:sector;0}");
    htInsert(&savefilevars, string, _S"event", string, _S"${string:event;}");
    htInsert(&savefilevars, string, _S"othership", string, _S"${string:othership;}");
}

_objfactory_guaranteed SaveManager* SaveManager_create(Subspace* ss)
{
    SaveManager* self;
    self = objInstCreate(SaveManager);

    self->ss = ss;
    strDup(&self->name, _S"SaveManager");

    objInstInit(self);
    return self;
}

extern SettingsPage* SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* SaveManager_createSettingsPage(_In_ SaveManager* self, SubspaceUI* ui)
{
    return SettingsPage(savemanagerpageCreate(self, ui));
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void SaveManager_applyDefaultSettings(_In_ SaveManager* self)
{
    // feature registry isn't available yet, so graab this from settings
    if (!(ssdVal(bool, self->ss->settings, _S"feature/RunTracker/available", false) &&
          ssdVal(bool, self->ss->settings, _S"feature/RunTracker/enabled", false))) {
        self->enabled = false;
        self->locked  = true;
    }

    return;
}

bool SaveManager_autoSave(_In_ SaveManager* self, RunInfo* run, _In_opt_ strref fname)
{
    bool ret = false;

    lazyInit(&savefilevars_init_state, savefilevars_init, NULL);

    // build out destination filename
    string destfn = 0;
    string temp = 0, fmt = 0;
    hashtable ivars, svars;
    int64 runid, savepoint;
    int64 now = clockWall();

    // create format string by substituting in variables
    ssdStringOutD(self->settings, _S"savename", &temp, SAVENAME_DEFAULT);
    pathSubst(&fmt, temp, savefilevars);

    // hash tables for integer and string vars
    htInit(&ivars, string, uint32, 10);
    htInit(&svars, string, string, 10);
    withReadLock (&run->lock) {
        TimeParts tp;
        timeDecompose(&tp, toLocalTime(now));
        htInsert(&ivars, string, _S"year", uint32, tp.year);
        htInsert(&ivars, string, _S"month", uint32, tp.month);
        htInsert(&ivars, string, _S"day", uint32, tp.day);
        htInsert(&ivars, string, _S"hour", uint32, tp.hour);
        htInsert(&ivars, string, _S"minute", uint32, tp.minute);
        htInsert(&ivars, string, _S"second", uint32, tp.second);

        runid     = run->runid;
        savepoint = run->savepoint;
        spointFormat(&temp, run->savepoint);
        htInsert(&svars, string, _S"beacon", string, temp);
        spointFormat(&temp, run->sectorpoint);
        htInsert(&svars, string, _S"sector", string, temp);

        htInsert(&svars, string, _S"event", string, run->initialEvent);
        htInsert(&svars, string, _S"othership", string, run->otherShip);

        htInsert(&svars, string, _S"shipname", string, run->shipName);

        strConcat(&temp, _S"ship_shortname_", run->shipType);
        htInsert(&svars, string, _S"shiptype", strref, langGetD(self->ss, temp, run->shipType));

        strFormat(&temp, fmt, stvar(hashtable, ivars), stvar(hashtable, svars));
        pathFilter(&destfn, temp);
        pathSetExt(&destfn, destfn, _S"sav");
    }

    SaveManagerCopy* cpy = savemanagercopyCreate(self, run, fname, destfn, runid, savepoint, now);
    tqRun(self->ss->workq, &cpy);

    htDestroy(&ivars);
    htDestroy(&svars);
    strDestroy(&temp);
    strDestroy(&fmt);
    strDestroy(&destfn);

    return ret;
}

// -------- SaveManagerCopy --------

_objfactory_guaranteed SaveManagerCopy*
SaveManagerCopy_create(SaveManager* smgr, RunInfo* run, _In_opt_ strref fname,
                       _In_opt_ strref destfn, int64 runid, int64 savepoint, int64 timestamp)
{
    SaveManagerCopy* self;
    self = objInstCreate(SaveManagerCopy);

    self->smgr      = objAcquire(smgr);
    self->runid     = runid;
    self->savepoint = savepoint;
    self->timestamp = timestamp;
    self->run       = objAcquire(run);
    strDup(&self->fname, fname);
    strDup(&self->destfn, destfn);

    objInstInit(self);
    return self;
}

uint32 SaveManagerCopy_run(_In_ SaveManagerCopy* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                           _Inout_ TaskControl* tcon)
{
    Subspace* ss = self->smgr->ss;

    string destpathreal = 0;
    string rundir       = 0;
    DbStmt* stmt        = NULL;
    runinfoGetRunDir(self->run, &rundir);
    pathJoin(&rundir, RUNDIR_FILENAME, rundir);
    if (!vfsIsDir(ss->fs, rundir) && !vfsCreateAll(ss->fs, rundir))
        goto out;
    if (!vfsGetFSPath(&destpathreal, ss->fs, rundir))
        goto out;

    pathJoin(&destpathreal, destpathreal, self->destfn);

    // check if we already have a save file for this savepoint in the database
    stmt = dbstmtCreate(ss->db, _S"SELECT time FROM saves WHERE runid=? AND savepoint=?");
    if (!stmt)
        goto out;
    dbstmtBind(stmt, 1, stvar(int64, self->runid));
    dbstmtBind(stmt, 2, stvar(int64, self->savepoint));
    dbstmtExec(stmt);
    if (saSize(stmt->row) > 0)
        goto out;   // already have one
    objRelease(&stmt);

    // copy file into place
    if (!vfsCopy(ss->rootfs, self->fname, destpathreal))
        goto out;

    // successfully copied the file; update it in the DB
    stmt = dbstmtCreate(ss->db,
                        _S
                        "INSERT INTO saves (runid, savepoint, time, filename) VALUES (?, ?, ?, ?)");
    if (!stmt)
        goto out;
    dbstmtBind(stmt, 1, stvar(int64, self->runid));
    dbstmtBind(stmt, 2, stvar(int64, self->savepoint));
    dbstmtBind(stmt, 3, stvar(int64, self->timestamp));
    dbstmtBind(stmt, 4, stvar(string, self->destfn));
    dbstmtExec(stmt);

out:
    objRelease(&stmt);
    strDestroy(&destpathreal);
    strDestroy(&rundir);

    // either way delete the temporary subspace save
    vfsDelete(ss->rootfs, self->fname);
    return TASK_Result_Success;
}

void SaveManagerCopy_destroy(_In_ SaveManagerCopy* self)
{
    // Autogen begins -----
    objRelease(&self->smgr);
    objRelease(&self->run);
    strDestroy(&self->fname);
    strDestroy(&self->destfn);
    // Autogen ends -------
}

// Autogen begins -----
#include "feature/savemanager/savemanager.auto.inc"
// Autogen ends -------

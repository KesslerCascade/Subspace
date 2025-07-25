// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "runinfo.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <cx/time.h>
#include <cx/utils/compare.h>
#include "db/database.h"
#include "feature/featureregistry.h"
#include "ui/subspaceui.h"
#include "hulltracker.h"
#include "logent.h"
#include "logrelay.h"
#include "scraptracker.h"

_objfactory_guaranteed RunInfo* RunInfo_create(Subspace* ss)
{
    RunInfo* self;
    self = objInstCreate(RunInfo);

    self->ss    = ss;
    self->runid = -1;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool RunInfo_init(_In_ RunInfo* self)
{
    // Autogen begins -----
    rwlockInit(&self->lock);
    saInit(&self->sectors, object, 1, SA_Sorted);
    return true;
    // Autogen ends -------
}

static void RunInfo_newUntracked(RunInfo* self, int seed, strref shipType, strref shipName,
                                 int difficulty)
{
    withWriteLock (&self->lock) {
        self->runid = -1;
        self->seed  = seed;
        strDup(&self->shipType, shipType);
        strDup(&self->shipName, shipName);
        self->difficulty  = difficulty;
        self->startTime   = clockWall();
        self->modified    = self->startTime;
        self->damageTaken = -1;   // will never have a value for this in untracked runs
    }
}

static void RunInfo_newTracked(RunInfo* self, int seed, strref shipType, strref shipName,
                               int difficulty)
{
    int64 now = clockWall();

    withWriteLock (&self->lock) {
        self->seed = seed;
        strDup(&self->shipType, shipType);
        strDup(&self->shipName, shipName);
        self->difficulty  = difficulty;
        self->sectorpoint = 256;
        self->startTime   = now;
        self->modified    = now;
    }

    DbStmt*
        stmt = dbPrepare(self->ss->db,
                         _S"INSERT INTO runs (seed, shiptype, shipname, difficulty, start) VALUES "
                          "(?, ?, ?, ?, ?)");
    dbstmtBind(stmt, 1, stvar(int32, seed));
    dbstmtBind(stmt, 2, stvar(strref, shipType));
    dbstmtBind(stmt, 3, stvar(strref, shipName));
    dbstmtBind(stmt, 4, stvar(int32, difficulty));
    dbstmtBind(stmt, 5, stvar(int64, now));
    if (dbstmtExec(stmt)) {
        withWriteLock (&self->lock) {
            self->runid     = dbLastId(self->ss->db);
            self->recording = true;
        }
        logFmt(Info,
               _S"Tracking new run [${int}]: ${string} (${string})",
               stvar(int64, self->runid),
               stvar(strref, shipName),
               stvar(strref, shipType));
    } else {
        logStr(Error, _S"Could not track run due to database error");
    }
    objRelease(&stmt);
}

static void loadSectors(Database* db, int64 runid, sa_SectorInfo* out)
{
    DbStmt* stmt = dbPrepare(db,
                             _S"SELECT sectorpoint, time, type, seed FROM sectors WHERE runid = ?");
    if (!stmt)
        return;

    saInit(out, object, 1, SA_Sorted);
    dbstmtBind(stmt, 1, stvar(int64, runid));

    while (dbstmtExec(stmt) && saSize(stmt->row) == 4) {
        SectorInfo* nsec = sectorinfoCreate();
        stConvert(int64, &nsec->sectorpoint, stvar, stmt->row.a[0]);
        stConvert(int64, &nsec->time, stvar, stmt->row.a[1]);
        stConvert(string, &nsec->type, stvar, stmt->row.a[2]);
        stConvert(int32, &nsec->seed, stvar, stmt->row.a[3]);
        saPushC(out, object, &nsec);
    }

    objRelease(&stmt);
}

static void RunInfo_findOrCreateTracked(RunInfo* self, int seed, strref shipType, strref shipName,
                                        int difficulty, int beacons)
{
    // try to find this run if it exists
    DbStmt* stmt = dbPrepare(
        self->ss->db,
        _S
        "SELECT runid, start, savepoint, sectorpoint, beacons_explored, ships_defeated, scrap_collected, crew_hired, scrap_actual, damage_taken, savepath "
        "FROM runs "
        "WHERE seed = ? AND shiptype = ? AND shipname = ? AND difficulty = ? AND beacons_explored <= ? AND result = 0 AND end = 0");
    if (!stmt)
        return;

    dbstmtBind(stmt, 1, stvar(int32, seed));
    dbstmtBind(stmt, 2, stvar(strref, shipType));
    dbstmtBind(stmt, 3, stvar(strref, shipName));
    dbstmtBind(stmt, 4, stvar(int32, difficulty));
    dbstmtBind(stmt, 5, stvar(int32, beacons));
    if (!dbstmtExec(stmt))
        logStr(Error, _S"Database error trying to find run!");

    if (saSize(stmt->row) == 11) {
        // found one, load it
        sa_SectorInfo loadsectors = saInitNone;
        loadSectors(self->ss->db, stmt->row.a[0].data.st_int64, &loadsectors);

        withWriteLock (&self->lock) {
            self->seed = seed;
            strDup(&self->shipType, shipType);
            strDup(&self->shipName, shipName);
            self->difficulty = difficulty;
            self->modified   = clockWall();

            stConvert(int64, &self->runid, stvar, stmt->row.a[0]);
            stConvert(int64, &self->startTime, stvar, stmt->row.a[1]);
            stConvert(int64, &self->savepoint, stvar, stmt->row.a[2]);
            stConvert(int64, &self->sectorpoint, stvar, stmt->row.a[3]);
            stConvert(int32, &self->beaconsExplored, stvar, stmt->row.a[4]);
            stConvert(int32, &self->shipsDefeated, stvar, stmt->row.a[5]);
            stConvert(int32, &self->scrapCollected, stvar, stmt->row.a[6]);
            stConvert(int32, &self->crewHired, stvar, stmt->row.a[7]);
            stConvert(int32, &self->scrapActual, stvar, stmt->row.a[8]);
            stConvert(int32, &self->damageTaken, stvar, stmt->row.a[9]);
            stConvert(string, &self->savePath, stvar, stmt->row.a[10]);

            saDestroy(&self->sectors);
            self->sectors = loadsectors;

            self->recording = true;
        }

        logFmt(Info,
               _S"Loaded run [${int}]: ${string} (${string})",
               stvar(int64, self->runid),
               stvar(strref, shipName),
               stvar(strref, shipType));
    } else {
        // did not find a matching run; create a new one
        logFmt(Info, _S"Did not find a matching run with seed ${int}", stvar(int32, seed));
        RunInfo_newTracked(self, seed, shipType, shipName, difficulty);
    }
    objRelease(&stmt);
}

bool RunInfo_loadHistoric(_In_ RunInfo* self, int64 runid)
{
    bool ret     = false;
    DbStmt* stmt = dbPrepare(
        self->ss->db,
        _S
        "SELECT start, end, savepoint, sectorpoint, seed, shiptype, shipname, difficulty, result, beacons_explored, ships_defeated, "
        "scrap_collected, crew_hired, scrap_actual, damage_taken, savepath "
        "FROM runs WHERE runid = ?");
    if (!stmt)
        return false;

    dbstmtBind(stmt, 1, stvar(int64, runid));
    if (!dbstmtExec(stmt))
        logStr(Error, _S"Database error trying to load run!");

    if (saSize(stmt->row) == 16) {
        // found one, load it
        sa_SectorInfo loadsectors = saInitNone;
        loadSectors(self->ss->db, runid, &loadsectors);

        withWriteLock (&self->lock) {
            self->modified = clockWall();
            self->runid    = runid;

            stConvert(int64, &self->startTime, stvar, stmt->row.a[0]);
            stConvert(int64, &self->endTime, stvar, stmt->row.a[1]);
            stConvert(int64, &self->savepoint, stvar, stmt->row.a[2]);
            stConvert(int64, &self->sectorpoint, stvar, stmt->row.a[3]);
            stConvert(int32, &self->seed, stvar, stmt->row.a[4]);
            stConvert(string, &self->shipType, stvar, stmt->row.a[5]);
            stConvert(string, &self->shipName, stvar, stmt->row.a[6]);
            stConvert(int32, &self->difficulty, stvar, stmt->row.a[7]);
            stConvert(int32, &self->result, stvar, stmt->row.a[8]);
            stConvert(int32, &self->beaconsExplored, stvar, stmt->row.a[9]);
            stConvert(int32, &self->shipsDefeated, stvar, stmt->row.a[10]);
            stConvert(int32, &self->scrapCollected, stvar, stmt->row.a[11]);
            stConvert(int32, &self->crewHired, stvar, stmt->row.a[12]);
            stConvert(int32, &self->scrapActual, stvar, stmt->row.a[13]);
            stConvert(int32, &self->damageTaken, stvar, stmt->row.a[14]);
            stConvert(string, &self->savePath, stvar, stmt->row.a[15]);

            saDestroy(&self->sectors);
            self->sectors = loadsectors;

            self->recording = true;
        }

        logFmt(Info,
               _S"Loaded run [${int}]: ${string} (${string})",
               stvar(int64, self->runid),
               stvar(strref, self->shipName),
               stvar(strref, self->shipType));

        ret = true;
    }

    objRelease(&stmt);
    return ret;
}

void RunInfo_newGame(_In_ RunInfo* self, int seed, _In_opt_ strref shipType,
                     _In_opt_ strref shipName, int difficulty)
{
    if (fregIsEnabled(self->ss->freg, _S"RunTracker"))
        RunInfo_newTracked(self, seed, shipType, shipName, difficulty);
    else
        RunInfo_newUntracked(self, seed, shipType, shipName, difficulty);

    // when starting a new game, the run is automatically focused by the UI
    subspaceSetRun(self->ss, self);
}

void RunInfo_loadGame(_In_ RunInfo* self, int seed, _In_opt_ strref shipType,
                      _In_opt_ strref shipName, int difficulty, int beacons)
{
    bool usetracker = fregIsEnabled(self->ss->freg, _S"RunTracker");

    if (usetracker)
        RunInfo_findOrCreateTracked(self, seed, shipType, shipName, difficulty, beacons);
    else
        RunInfo_newUntracked(self, seed, shipType, shipName, difficulty);

    // when loading a saved game, the run is automatically focused by the UI
    subspaceSetRun(self->ss, self);
    if (usetracker)
        runinfoReplayLog(self, false, 0, 0);
}

void RunInfo_finish(_In_ RunInfo* self, RunResult result)
{
    RunResult oldresult;
    int64 now = clockWall();
    withReadLock (&self->lock) {
        oldresult = self->result;
    }
    if (oldresult != RUN_Active)
        return;   // can't finish a run that's already been finished!

    if (self->recording) {
        DbStmt* stmt = dbPrepare(self->ss->db, _S"UPDATE runs SET result=?, end=? WHERE runid=?");
        if (stmt) {
            dbstmtBind(stmt, 1, stvar(int32, result));
            dbstmtBind(stmt, 2, stvar(int64, now));
            dbstmtBind(stmt, 3, stvar(int64, self->runid));
            dbstmtExec(stmt);
        }
        objRelease(&stmt);
    }

    withWriteLock (&self->lock) {
        self->result    = result;
        self->endTime   = now;
        self->recording = false;
        self->modified  = clockWall();
    }

    if (self->focused)
        ssuiUpdateMain(self->ss->ui, _S"gameinfo");
}

void RunInfo_enterSector(_In_ RunInfo* self, int num, int seed, _In_opt_ strref type, bool secret)
{
    int extra    = 0;
    bool already = false;

    withReadLock (&self->lock) {
        // check if we already have this sector (i.e. loading a game)
        foreach (sarray, idx, SectorInfo*, sec, self->sectors) {
            if (SPOINT_NUM(sec->sectorpoint) == num && sec->seed == seed) {
                already = true;
                break;
            }

            // if we already have a sector with this number but a different seed (i.e. crystal
            // sector or other secret in mods), record the highest extra
            if (SPOINT_NUM(sec->sectorpoint) == num)
                extra = max(extra, SPOINT_EXTRA(sec->sectorpoint) + 1);
        }
    }
    if (already)
        return;

    SectorInfo* nsec  = sectorinfoCreate();
    nsec->sectorpoint = SPOINT(num, extra);
    nsec->seed        = seed;
    nsec->time        = clockWall();
    strDup(&nsec->type, type);

    if (self->recording) {
        DbStmt* stmt = dbPrepare(
            self->ss->db,
            _S "INSERT INTO sectors (runid, sectorpoint, time, type, seed) VALUES (?, ?, ?, ?, ?)");
        if (stmt) {
            dbstmtBind(stmt, 1, stvar(int64, self->runid));
            dbstmtBind(stmt, 2, stvar(int64, nsec->sectorpoint));
            dbstmtBind(stmt, 3, stvar(int64, nsec->time));
            dbstmtBind(stmt, 4, stvar(strref, nsec->type));
            dbstmtBind(stmt, 5, stvar(int32, nsec->seed));
            dbstmtExec(stmt);
            objRelease(&stmt);
        } else {
            logStr(Error, _S"Failed to insert into sectors table");
        }

        if (nsec->sectorpoint > self->sectorpoint) {
            stmt = dbPrepare(self->ss->db, _S"UPDATE runs SET sectorpoint=? WHERE runid=?");
            if (stmt) {
                dbstmtBind(stmt, 1, stvar(int64, nsec->sectorpoint));
                dbstmtBind(stmt, 2, stvar(int64, self->runid));
                dbstmtExec(stmt);
                objRelease(&stmt);
            }
        }
    }

    withWriteLock (&self->lock) {
        if (nsec->sectorpoint > self->sectorpoint) {
            self->sectorpoint = nsec->sectorpoint;
        }
        saPushC(&self->sectors, object, &nsec);
        self->modified = clockWall();
    }

    if (self->focused)
        ssuiUpdateMain(self->ss->ui, _S"gameinfo");
}

SectorInfo* RunInfo_getSector(_In_ RunInfo* self, int64 sectorpoint)
{
    SectorInfo* ret = NULL;

    withReadLock (&self->lock) {
        foreach (sarray, i, SectorInfo*, sec, self->sectors) {
            if (sec->sectorpoint == sectorpoint) {
                ret = objAcquire(sec);
                break;
            }
        }
    }

    return ret;
}

HullTracker* RunInfo_getHull(_In_ RunInfo* self)
{
    HullTracker* ret = NULL;
    withReadLock (&self->lock) {
        ret = objAcquire(self->hull);
    }

    return ret;
}

ScrapTracker* RunInfo_getScrap(_In_ RunInfo* self)
{
    ScrapTracker* ret = NULL;
    withReadLock (&self->lock) {
        ret = objAcquire(self->scrap);
    }

    return ret;
}

void RunInfo_updateStats(_In_ RunInfo* self, int ships, int beacons, int scrap, int crew)
{
    if (self->recording) {
        string sql   = 0;
        int nb       = 0;
        DbStmt* stmt = NULL;

        // build SQL statement
        strDup(&sql, _S"UPDATE runs SET ");
        if (ships >= 0) {
            strAppend(&sql, _S"ships_defeated=?");
            nb++;
        }

        if (beacons >= 0) {
            if (nb > 0)
                strAppend(&sql, _S", ");
            strAppend(&sql, _S"beacons_explored=?");
            nb++;
        }

        if (scrap >= 0) {
            if (nb > 0)
                strAppend(&sql, _S", ");
            strAppend(&sql, _S"scrap_collected=?");
            nb++;
        }

        if (crew >= 0) {
            if (nb > 0)
                strAppend(&sql, _S", ");
            strAppend(&sql, _S"crew_hired=?");
            nb++;
        }
        strAppend(&sql, _S" WHERE runid = ?");

        stmt = dbPrepare(self->ss->db, sql);

        int b = 1;
        if (ships >= 0)
            dbstmtBind(stmt, b++, stvar(int32, ships));
        if (beacons >= 0)
            dbstmtBind(stmt, b++, stvar(int32, beacons));

        if (scrap >= 0)
            dbstmtBind(stmt, b++, stvar(int32, scrap));

        if (crew >= 0)
            dbstmtBind(stmt, b++, stvar(int32, crew));

        dbstmtBind(stmt, b++, stvar(int64, self->runid));
        dbstmtExec(stmt);

        strDestroy(&sql);
        objRelease(&stmt);
    }

    withWriteLock (&self->lock) {
        if (ships >= 0)
            self->shipsDefeated = ships;
        if (beacons >= 0)
            self->beaconsExplored = beacons;
        if (scrap >= 0)
            self->scrapCollected = scrap;
        if (crew >= 0)
            self->crewHired = crew;

        self->modified = clockWall();
    }

    if (self->focused)
        ssuiUpdateMain(self->ss->ui, _S"gameinfo");
}

int32 RunInfo_score(_In_ RunInfo* self)
{
    float diffScale = (self->difficulty == 2) ? 1.5f : (self->difficulty == 1 ? 1.25f : 1.0f);
    return (int32)((self->beaconsExplored * 10 + self->shipsDefeated * 20 + self->scrapCollected) *
                   diffScale);
}

void RunInfo_runLog(_In_ RunInfo* self, int sector, int beacons, int64 time, float64 gametime,
                    _In_opt_ strref id, stvar params[LOG_MAX_PARAMS])
{
    // normalize sectorpoint / savepoint
    int64 savepoint, sectorpoint;
    bool recording, focused;
    withReadLock (&self->lock) {
        savepoint   = max(SPOINT(beacons, 0), self->savepoint);
        sectorpoint = max(SPOINT(sector, 0), self->sectorpoint);
        // remember this now because runinfoProcessLog may change it for run-ending events
        recording   = self->recording;
        focused     = self->focused;
    }

    LogEnt* ent = logentCreate(sectorpoint, savepoint, time, gametime, id, params);
    if (!ent)
        return;

    LogEntSpec* spec = ent->spec;

    // relay to interested UI elements
    if (focused)
        logrelaySend(self->ss->runlog, ent, false);

    // do any top-level processing for the run info itself
    runinfoProcessLog(self, ent);

    string temp = 0;
    // save in database
    if (recording) {
        string sql = ent->spec->combat ? _S"INSERT INTO combatlog " : _S"INSERT INTO log ";
        strAppend(&sql, _S"(runid, savepoint, sectorpoint, time, gametime, id");
        for (int i = 0; i < spec->numParams; i++) {
            strFormat(&temp, _S", param${int}", stvar(int32, i + 1));
            strAppend(&sql, temp);
        }
        strAppend(&sql, _S") VALUES (?, ?, ?, ?, ?, ?");
        for (int i = 0; i < spec->numParams; i++) {
            strAppend(&sql, _S", ?");
        }
        strAppend(&sql, _S")");

        DbStmt* stmt = dbPrepare(self->ss->db, sql);
        strDestroy(&sql);
        if (!stmt)
            return;

        dbstmtBind(stmt, 1, stvar(int64, self->runid));
        dbstmtBind(stmt, 2, stvar(int64, savepoint));
        dbstmtBind(stmt, 3, stvar(int64, sectorpoint));
        dbstmtBind(stmt, 4, stvar(int64, time));
        dbstmtBind(stmt, 5, stvar(float64, gametime));
        dbstmtBind(stmt, 6, stvar(strref, id));
        for (int i = 0; i < spec->numParams; i++) {
            dbstmtBind(stmt, 7 + i, params[i]);
        }

        if (!dbstmtExec(stmt))
            logStr(DevVerbose, _S"Failed to insert log entry");

        objRelease(&ent);
        objRelease(&stmt);
    }
}

void RunInfo_processLog(_In_ RunInfo* self, LogEnt* ent)
{
    if (ent->spec == &Log_Scrap) {
        runinfoProcessScrap(self,
                            ent->rawparams.a[0].data.st_strref,
                            ent->rawparams.a[1].data.st_int32,
                            ent->rawparams.a[2].data.st_int32);
    } else if (ent->spec == &Log_HullDamage) {
        runinfoProcessHullDamage(self,
                                 ent->rawparams.a[0].data.st_strref,
                                 ent->rawparams.a[1].data.st_int32);
    } else if (ent->spec == &Log_Victory) {
        RunInfo_finish(self, RUN_Victory);
    } else if (ent->spec == &Log_Defeat) {
        RunInfo_finish(self, RUN_Defeat);
    } else if (ent->spec == &Log_Abandon) {
        RunInfo_finish(self, RUN_Abandoned);
    } else if (ent->spec == &Log_Ship) {
        runinfoProcessShip(self, ent->rawparams.a[0].data.st_strref);
    }
}

void RunInfo_processScrap(_In_ RunInfo* self, _In_opt_ strref src, int amount, int rawamount)
{
    if (strEq(src, _S"Event") && amount > 0) {
        // for income from an event, record it as actual scrap
        int32 scrapActual;
        withWriteLock (&self->lock) {
            self->scrapActual += amount;
            self->modified = clockWall();
            scrapActual    = self->scrapActual;
        }

        if (self->recording) {
            DbStmt* stmt = dbPrepare(self->ss->db,
                                     _S"UPDATE runs SET scrap_actual=? WHERE runid=?");
            if (stmt) {
                dbstmtBind(stmt, 1, stvar(int32, scrapActual));
                dbstmtBind(stmt, 2, stvar(int64, self->runid));
                dbstmtExec(stmt);
            }
            objRelease(&stmt);
        }

        if (self->focused)
            ssuiUpdateMain(self->ss->ui, _S"gameinfo");
    }
}

void RunInfo_processHullDamage(_In_ RunInfo* self, _In_opt_ strref src, int amount)
{
    if (amount > 0) {
        int32 damageTaken;
        withWriteLock (&self->lock) {
            self->damageTaken += amount;
            self->modified = clockWall();
            damageTaken    = self->damageTaken;
        }

        if (self->recording) {
            DbStmt* stmt = dbPrepare(self->ss->db,
                                     _S"UPDATE runs SET damage_taken=? WHERE runid=?");
            if (stmt) {
                dbstmtBind(stmt, 1, stvar(int32, damageTaken));
                dbstmtBind(stmt, 2, stvar(int64, self->runid));
                dbstmtExec(stmt);
            }
            objRelease(&stmt);
        }

        if (self->focused)
            ssuiUpdateMain(self->ss->ui, _S"gameinfo");
    }
}

void RunInfo_processShip(_In_ RunInfo* self, _In_opt_ strref name)
{
    int64 runid, savepoint;
    withWriteLock (&self->lock) {
        if (!self->recording || self->updatedBeaconShip)
            break;
        self->updatedBeaconShip = true;
        runid                   = self->runid;
        savepoint               = self->savepoint;
    }

    DbStmt* stmt = dbPrepare(self->ss->db,
                             _S"UPDATE beacons SET other_ship=? WHERE runid=? AND savepoint=?");
    if (stmt) {
        dbstmtBind(stmt, 1, stvar(strref, name));
        dbstmtBind(stmt, 2, stvar(int64, runid));
        dbstmtBind(stmt, 3, stvar(int64, savepoint));
        dbstmtExec(stmt);
    }
    objRelease(&stmt);
}

void RunInfo_replayLog(_In_ RunInfo* self, bool combat, int64 savepoint, int64 sectorpoint)
{
    // run this as a separate task so that it can wait for data if needed
    LogReplay* replay = logreplayCreate(self->ss, self, combat, savepoint, sectorpoint);
    tqRun(self->ss->workq, &replay);
}

void RunInfo_beacon(_In_ RunInfo* self, int sector, int beacons, int visit, int x, int y,
                    int64 time, _In_opt_ strref event)
{
    int64 savepoint, sectorpoint, runid;
    bool recording;
    withWriteLock (&self->lock) {
        savepoint   = max(SPOINT(beacons, 0), self->savepoint + 1);
        sectorpoint = max(SPOINT(sector, 0), self->sectorpoint);
        runid       = self->runid;
        recording   = self->recording;

        self->savepoint         = savepoint;   // we've always jumped or waited to get here
        self->updatedBeaconShip = false;
    }

    if (recording) {
        DbStmt* stmt = dbPrepare(self->ss->db, _S"UPDATE runs SET savepoint=? WHERE runid=?");
        if (stmt) {
            dbstmtBind(stmt, 1, stvar(int32, savepoint));
            dbstmtBind(stmt, 2, stvar(int64, runid));
            dbstmtExec(stmt);
        }
        objRelease(&stmt);

        stmt = dbPrepare(
            self->ss->db,
            _S
            "INSERT INTO beacons (runid, savepoint, sectorpoint, visit, x, y, time, initial_event) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        if (stmt) {
            dbstmtBind(stmt, 1, stvar(int64, runid));
            dbstmtBind(stmt, 2, stvar(int64, savepoint));
            dbstmtBind(stmt, 3, stvar(int64, sectorpoint));
            dbstmtBind(stmt, 4, stvar(int32, visit));
            dbstmtBind(stmt, 5, stvar(int32, x));
            dbstmtBind(stmt, 6, stvar(int32, y));
            dbstmtBind(stmt, 7, stvar(int64, time));
            dbstmtBind(stmt, 8, stvar(strref, event));
            dbstmtExec(stmt);
        }
        objRelease(&stmt);
    }
}

void RunInfo_setFocused(_In_ RunInfo* self, bool focused)
{
    withWriteLock (&self->lock) {
        if (self->focused != focused) {
            if (focused) {
                self->hull  = hulltrackerCreate(self);
                self->scrap = scraptrackerCreate(self);
            } else {
                objRelease(&self->hull);
                objRelease(&self->scrap);
            }
            self->focused = focused;
        }
    }
}

bool RunInfo_isFocused(_In_ RunInfo* self)
{
    bool ret;
    withReadLock (&self->lock) {
        ret = self->focused;
    }

    return ret;
}

bool RunInfo_isRecording(_In_ RunInfo* self)
{
    bool ret;
    withReadLock (&self->lock) {
        ret = self->recording;
    }

    return ret;
}

void RunInfo_destroy(_In_ RunInfo* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    strDestroy(&self->shipType);
    strDestroy(&self->shipName);
    strDestroy(&self->savePath);
    saDestroy(&self->sectors);
    objRelease(&self->hull);
    objRelease(&self->scrap);
    // Autogen ends -------
}

_objfactory_guaranteed LogReplay*
LogReplay_create(Subspace* ss, RunInfo* run, bool combat, int64 savepoint, int64 sectorpoint)
{
    LogReplay* self;
    self = objInstCreate(LogReplay);

    self->ss          = ss;
    self->run         = objAcquire(run);
    self->combat      = combat;
    self->savepoint   = savepoint;
    self->sectorpoint = sectorpoint;

    objInstInit(self);

    // defer the replay until game data is finished loading
    GameData* data = subspaceData(ss);
    if (data) {
        gamedataWait(data, self);
        objRelease(&data);
    }

    return self;
}

uint32 LogReplay_run(_In_ LogReplay* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                     _Inout_ TaskControl* tcon)
{
    string sql = self->combat ? _S
        "SELECT savepoint, sectorpoint, time, gametime, id, param1, param2, param3, param4 FROM combatlog WHERE runid = ?" :
                                _S
        "SELECT savepoint, sectorpoint, time, gametime, id, param1, param2, param3, param4 FROM log WHERE runid = ?";

    if (self->savepoint > 0)
        strAppend(&sql, _S" AMD savepoint = ?");
    if (self->sectorpoint > 0)
        strAppend(&sql, _S" AND sectorpoint = ?");

    strAppend(&sql, _S" ORDER BY savepoint, time, rowid");

    DbStmt* stmt = dbPrepare(self->ss->db, sql);
    if (!stmt)
        goto out;

    RunInfo* run = self->run;

    int nb = 1;
    dbstmtBind(stmt, nb++, stvar(int64, run->runid));
    if (self->savepoint > 0)
        dbstmtBind(stmt, nb++, stvar(int64, self->savepoint));
    if (self->sectorpoint > 0)
        dbstmtBind(stmt, nb++, stvar(int64, self->sectorpoint));

    LogRelay* runlog = self->ss->runlog;
    logrelayReset(runlog);

    while (dbstmtExec(stmt) && saSize(stmt->row) == 9) {
        int64 savepoint              = 0;
        int64 sectorpoint            = 0;
        int64 time                   = 0;
        float64 gametime             = 0;
        string id                    = 0;
        stvar params[LOG_MAX_PARAMS] = { 0 };

        stConvert(int64, &savepoint, stvar, stmt->row.a[0]);
        stConvert(int64, &sectorpoint, stvar, stmt->row.a[1]);
        stConvert(int64, &time, stvar, stmt->row.a[2]);
        stConvert(float64, &gametime, stvar, stmt->row.a[3]);
        stConvert(string, &id, stvar, stmt->row.a[4]);
        params[0] = stmt->row.a[5];
        params[1] = stmt->row.a[6];
        params[2] = stmt->row.a[7];
        params[3] = stmt->row.a[8];

        LogEnt* nent = logentCreate(sectorpoint, savepoint, time, gametime, id, params);
        if (nent) {
            logrelaySend(runlog, nent, true);
            objRelease(&nent);
        }

        strDestroy(&id);
    }

    logrelayReplayComplete(runlog);

out:
    strDestroy(&sql);
    objRelease(&stmt);

    return TASK_Result_Success;
}

void LogReplay_destroy(_In_ LogReplay* self)
{
    // Autogen begins -----
    objRelease(&self->run);
    // Autogen ends -------
}

// Autogen begins -----
#include "runinfo.auto.inc"
// Autogen ends -------

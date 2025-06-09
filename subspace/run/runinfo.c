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
#include <cx/time.h>
#include <cx/utils/compare.h>
#include "db/database.h"
#include "feature/featureregistry.h"
#include "ui/subspaceui.h"

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

void RunInfo_destroy(_In_ RunInfo* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    strDestroy(&self->shipType);
    strDestroy(&self->shipName);
    strDestroy(&self->savePath);
    saDestroy(&self->sectors);
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
        self->difficulty = difficulty;
    }
}

static void RunInfo_newTracked(RunInfo* self, int seed, strref shipType, strref shipName,
                               int difficulty)
{
    withWriteLock (&self->lock) {
        self->seed = seed;
        strDup(&self->shipType, shipType);
        strDup(&self->shipName, shipName);
        self->difficulty  = difficulty;
        self->sectorpoint = 256;
        self->startTime   = clockWall();

        DbStmt* stmt =
            dbPrepare(self->ss->db,
                      _S"INSERT INTO runs (seed, shiptype, shipname, difficulty, start) VALUES "
                       "(?, ?, ?, ?, ?)");
        dbstmtBind(stmt, 1, stvar(int32, seed));
        dbstmtBind(stmt, 2, stvar(strref, shipType));
        dbstmtBind(stmt, 3, stvar(strref, shipName));
        dbstmtBind(stmt, 4, stvar(int32, difficulty));
        dbstmtBind(stmt, 5, stvar(int64, self->startTime));
        if (dbstmtExec(stmt)) {
            self->runid     = dbLastId(self->ss->db);
            self->recording = true;
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
}

// MUST HAVE WRITE LOCK HELD!
static void RunInfo_loadSectors(RunInfo* self)
{
    DbStmt* stmt = dbPrepare(self->ss->db,
                             _S"SELECT sectorpoint, time, type, seed FROM sectors WHERE runid = ?");
    if (!stmt)
        return;

    dbstmtBind(stmt, 1, stvar(int64, self->runid));

    while (dbstmtExec(stmt) && saSize(stmt->row) == 4) {
        SectorInfo* nsec = sectorinfoCreate();
        stConvert(int64, &nsec->sectorpoint, stvar, stmt->row.a[0]);
        stConvert(int64, &nsec->time, stvar, stmt->row.a[1]);
        stConvert(string, &nsec->type, stvar, stmt->row.a[2]);
        stConvert(int32, &nsec->seed, stvar, stmt->row.a[3]);
        saPushC(&self->sectors, object, &nsec);
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
        withWriteLock (&self->lock) {
            self->seed = seed;
            strDup(&self->shipType, shipType);
            strDup(&self->shipName, shipName);
            self->difficulty = difficulty;

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

            RunInfo_loadSectors(self);
            self->recording = true;

            logFmt(Info,
                   _S"Loaded run [${int}]: ${string} (${string})",
                   stvar(int64, self->runid),
                   stvar(strref, shipName),
                   stvar(strref, shipType));
        }
    } else {
        // did not find a matching run; create a new one
        logFmt(Info, _S"Did not find a matching run with seed ${int}", stvar(int32, seed));
        RunInfo_newTracked(self, seed, shipType, shipName, difficulty);
    }
    objRelease(&stmt);
}

void RunInfo_newGame(_In_ RunInfo* self, int seed, _In_opt_ strref shipType,
                     _In_opt_ strref shipName, int difficulty)
{
    if (fregIsEnabled(self->ss->freg, _S"RunTracker"))
        RunInfo_newTracked(self, seed, shipType, shipName, difficulty);
    else
        RunInfo_newUntracked(self, seed, shipType, shipName, difficulty);
}

void RunInfo_loadGame(_In_ RunInfo* self, int seed, _In_opt_ strref shipType,
                      _In_opt_ strref shipName, int difficulty, int beacons)
{
    if (fregIsEnabled(self->ss->freg, _S"RunTracker"))
        RunInfo_findOrCreateTracked(self, seed, shipType, shipName, difficulty, beacons);
    else
        RunInfo_newUntracked(self, seed, shipType, shipName, difficulty);
}

void RunInfo_abandon(_In_ RunInfo* self)
{
    withWriteLock (&self->lock) {
        if (self->result == RUN_Active) {
            self->result = RUN_Abandoned;

            if (self->recording) {
                // TODO: Record in database
            }
        }
    }
}

void RunInfo_enterSector(_In_ RunInfo* self, int num, int seed, _In_opt_ strref type, bool secret)
{
    int extra = 0;

    withWriteLock (&self->lock) {
        // check if we already have this sector (i.e. loading a game)
        foreach (sarray, idx, SectorInfo*, sec, self->sectors) {
            if (SPOINT_NUM(sec->sectorpoint) == num && sec->seed == seed)
                goto out;

            // if we already have a sector with this number but a different seed (i.e. crystal
            // sector or other secret in mods), record the highest extra
            if (SPOINT_NUM(sec->sectorpoint) == num)
                extra = max(extra, SPOINT_EXTRA(sec->sectorpoint) + 1);
        }

        SectorInfo* nsec  = sectorinfoCreate();
        nsec->sectorpoint = SPOINT(num, extra);
        nsec->seed        = seed;
        nsec->time        = clockWall();
        strDup(&nsec->type, type);

        if (self->recording) {
            DbStmt* stmt = dbPrepare(
                self->ss->db,
                _S
                "INSERT INTO sectors (runid, sectorpoint, time, type, seed) VALUES (?, ?, ?, ?, ?)");
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
        }

        if (nsec->sectorpoint > self->sectorpoint) {
            self->sectorpoint = nsec->sectorpoint;

            if (self->recording) {
                DbStmt* stmt = dbPrepare(self->ss->db,
                                         _S"UPDATE runs SET sectorpoint=? WHERE runid=?");
                if (stmt) {
                    dbstmtBind(stmt, 1, stvar(int64, nsec->sectorpoint));
                    dbstmtBind(stmt, 2, stvar(int64, self->runid));
                    dbstmtExec(stmt);
                }
            }
        }

        saPushC(&self->sectors, object, &nsec);
        ssuiUpdateMain(self->ss->ui, _S"gameinfo");

out:
        break;
    }
}

void RunInfo_updateStats(_In_ RunInfo* self, int ships, int beacons, int scrap, int crew)
{
    // build SQL statement
    string sql   = 0;
    int nb       = 0;
    DbStmt* stmt = NULL;

    if (self->recording) {
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
    }

    withWriteLock (&self->lock) {
        int b = 1;
        if (ships >= 0) {
            self->shipsDefeated = ships;
            if (self->recording)
                dbstmtBind(stmt, b++, stvar(int32, ships));
        }

        if (beacons >= 0) {
            self->beaconsExplored = beacons;
            if (self->recording)
                dbstmtBind(stmt, b++, stvar(int32, beacons));
        }

        if (scrap >= 0) {
            self->scrapCollected = scrap;
            if (self->recording)
                dbstmtBind(stmt, b++, stvar(int32, scrap));
        }

        if (crew >= 0) {
            self->crewHired = crew;
            if (self->recording)
                dbstmtBind(stmt, b++, stvar(int32, crew));
        }

        if (self->recording) {
            dbstmtBind(stmt, b++, stvar(int64, self->runid));
            dbstmtExec(stmt);
        }

        ssuiUpdateMain(self->ss->ui, _S"gameinfo");
    }

    strDestroy(&sql);
    objRelease(&stmt);
}

int32 RunInfo_score(_In_ RunInfo* self)
{
    float diffScale = (self->difficulty == 2) ? 1.5f : (self->difficulty == 1 ? 1.25f : 1.0f);
    return (int32)((self->beaconsExplored * 10 + self->shipsDefeated * 20 + self->scrapCollected) *
                   diffScale);
}

// Autogen begins -----
#include "runinfo.auto.inc"
// Autogen ends -------

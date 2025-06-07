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
#include "db/database.h"
#include "feature/featureregistry.h"

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
    // Autogen ends -------
}

static void RunInfo_newUntracked(RunInfo* self, int seed, strref shipType, strref shipName,
                                 int difficulty)
{
    self->runid = -1;
    self->seed  = seed;
    strDup(&self->shipType, shipType);
    strDup(&self->shipName, shipName);
    self->difficulty = difficulty;
}

static void RunInfo_newTracked(RunInfo* self, int seed, strref shipType, strref shipName,
                               int difficulty)
{
    self->seed = seed;
    strDup(&self->shipType, shipType);
    strDup(&self->shipName, shipName);
    self->difficulty = difficulty;

    DbStmt*
        stmt = dbPrepare(self->ss->db,
                         _S"INSERT INTO runs (seed, shiptype, shipname, difficulty, start) VALUES "
                          "(?, ?, ?, ?, ?)");
    dbstmtBind(stmt, 1, stvar(int32, seed));
    dbstmtBind(stmt, 2, stvar(strref, shipType));
    dbstmtBind(stmt, 3, stvar(strref, shipName));
    dbstmtBind(stmt, 4, stvar(int32, difficulty));
    dbstmtBind(stmt, 5, stvar(int64, clockWall()));
    if (dbstmtExec(stmt)) {
        self->runid = dbLastId(self->ss->db);
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

static void RunInfo_findOrCreateTracked(RunInfo* self, int seed, strref shipType, strref shipName,
                                        int difficulty, int beacons)
{
    // try to find this run if it exists
    DbStmt* stmt = dbPrepare(
        self->ss->db,
        _S
        "SELECT runid, start, savepoint, beacons_explored, ships_defeated, scrap_collected, crew_hired, scrap_actual, damage_taken, savepath "
        "FROM runs "
        "WHERE seed = ? AND shiptype = ? AND shipname = ? AND difficulty = ? AND beacons_explored <= ? AND result = 0 AND end = 0");
    dbstmtBind(stmt, 1, stvar(int32, seed));
    dbstmtBind(stmt, 2, stvar(strref, shipType));
    dbstmtBind(stmt, 3, stvar(strref, shipName));
    dbstmtBind(stmt, 4, stvar(int32, difficulty));
    dbstmtBind(stmt, 5, stvar(int32, beacons));
    if (!dbstmtExec(stmt))
        logStr(Error, _S"Database error trying to find run!");

    if (saSize(stmt->row) > 0) {
        // found one, load it
        if (saSize(stmt->row) == 10) {
            stConvert(int64, &self->runid, stvar, stmt->row.a[0]);
            stConvert(int64, &self->startTime, stvar, stmt->row.a[1]);
            stConvert(int64, &self->savepoint, stvar, stmt->row.a[2]);
            stConvert(int32, &self->beaconsExplored, stvar, stmt->row.a[3]);
            stConvert(int32, &self->shipsDefeated, stvar, stmt->row.a[4]);
            stConvert(int32, &self->scrapCollected, stvar, stmt->row.a[5]);
            stConvert(int32, &self->crewHired, stvar, stmt->row.a[6]);
            stConvert(int32, &self->scrapActual, stvar, stmt->row.a[7]);
            stConvert(int32, &self->damageTaken, stvar, stmt->row.a[8]);
            stConvert(string, &self->savePath, stvar, stmt->row.a[9]);
            self->recording = true;

            logFmt(Info,
                   _S"Loaded run [${int}]: ${string} (${string})",
                   stvar(int64, self->runid),
                   stvar(strref, shipName),
                   stvar(strref, shipType));
        } else {
            logStr(Error, _S"Incorrect number of columns returned when loading run!");
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

// Autogen begins -----
#include "runinfo.auto.inc"
// Autogen ends -------

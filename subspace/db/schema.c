#include "schema.h"
#include <cx/log.h>
#include <sqlite3.h>

static bool dbCreateRuns(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE runs ("
                     "runid INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "seed INTEGER NOT NULL,"
                     "shiptype TEXT NOT NULL,"
                     "shipname TEXT NOT NULL,"
                     "difficulty INTEGER NOT NULL,"
                     "result INTEGER NOT NULL DEFAULT 0,"
                     "start INTEGER NOT NULL DEFAULT 0,"
                     "end INTEGER NOT NULL DEFAULT 0,"
                     "savepoint INTEGER NOT NULL DEFAULT 0,"
                     "sectorpoint INTEGER NOT NULL DEFAULT 256,"
                     "beacons_explored INTEGER NOT NULL DEFAULT 0,"
                     "ships_defeated INTEGER NOT NULL DEFAULT 0,"
                     "scrap_collected INTEGER NOT NULL DEFAULT 0,"
                     "crew_hired INTEGER NOT NULL DEFAULT 0,"
                     "scrap_actual INTEGER NOT NULL DEFAULT 0,"
                     "damage_taken INTEGER NOT NULL DEFAULT 0,"
                     "savepath TEXT"
                     ")",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX runs_shiptype ON runs (shiptype)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX runs_seed ON runs (seed)", NULL, NULL, NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX runs_result ON runs (result)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX runs_start ON runs (start)", NULL, NULL, NULL) != SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create runs table");

    return ret;
}

static bool dbCreateSectors(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE sectors ("
                     "runid INTEGER NOT NULL,"
                     "sectorpoint INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "type TEXT NOT NULL,"
                     "seed INTEGER NOT NULL,"
                     "PRIMARY KEY (runid, sectorpoint))",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX sectors_runid ON sectors (runid)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create sectors table");

    return ret;
}

static bool dbCreateBeacons(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE beacons ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectorpoint INTEGER NOT NULL,"
                     "visit INTEGER NOT NULL,"
                     "x INTEGER NOT NULL,"
                     "y INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "initial_event TEXT,"
                     "other_ship TEXT,"
                     "PRIMARY KEY (runid, savepoint))",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX beacons_runid ON beacons (runid)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create beacons table");

    return ret;
}

static bool dbUpgradeV1Beacons(sqlite3* db)
{
    bool ret = true;

    // the beacons table isn't actually used in V1 at all; so we can simply drop it

    if (sqlite3_exec(db, "DROP TABLE beacons", NULL, NULL, NULL) != SQLITE_OK)
        ret = false;

    return ret && dbCreateBeacons(db);
}

static bool dbCreateSaves(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE saves ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectorpoint INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "filename TEXT NOT NULL,"
                     "PRIMARY KEY (runid, savepoint))",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX saves_runid ON saves (runid)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create saves table");

    return ret;
}

static bool dbCreateLog(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE log ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectorpoint INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "id TEXT NOT NULL,"
                     "param1,"
                     "param2,"
                     "param3,"
                     "param4"
                     ")",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX log_runid ON log (runid)", NULL, NULL, NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX log_runid_savepoint ON log (runid, savepoint)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX log_runid_sectorpoint ON log (runid, sectorpoint)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX log_id ON log (id)", NULL, NULL, NULL) != SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create log table");

    return ret;
}

// copy and paste this because it's identical for now, but may change later
static bool dbCreateCombatLog(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE combatlog ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectorpoint INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "id TEXT NOT NULL,"
                     "param1,"
                     "param2,"
                     "param3,"
                     "param4"
                     ")",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX combatlog_runid ON combatlog (runid)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX combatlog_runid_savepoint ON combatlog (runid, savepoint)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create combatlog table");

    return ret;
}

bool dbCreateSchema(sqlite3* db)
{
    bool ret = false;

    if (!dbCreateRuns(db) || !dbCreateSectors(db) || !dbCreateBeacons(db) || !dbCreateSaves(db) ||
        !dbCreateLog(db) || !dbCreateCombatLog(db))
        return false;

    if (sqlite3_exec(db, "CREATE TABLE subspace (ver INT)", NULL, NULL, NULL) != SQLITE_OK)
        return false;

    sqlite3_stmt* s;
    if (sqlite3_prepare_v2(db, "INSERT INTO subspace VALUES (?)", -1, &s, NULL) != SQLITE_OK)
        return false;

    sqlite3_bind_int(s, 1, DB_CURRENT_SCHEMA_VER);

    if (sqlite3_step(s) == SQLITE_DONE)
        ret = true;

    sqlite3_finalize(s);

    return ret;
}

static bool dbUpgradeV2RunLog(sqlite3* db)
{
    bool ret = true;

    // schema versions before V2 didn't have the Start runlog entry, but it can be synthesized from
    // the run data
    if (sqlite3_exec(
            db,
            "INSERT INTO log SELECT runid, 256 AS savepoint, 256 as sectorpoint, start AS time, "
            "'Start' AS id, "
            "shiptype AS param1, shipname AS param2, seed AS param3, difficulty AS "
            "param4 FROM runs",
            NULL,
            NULL,
            NULL) != SQLITE_OK)
        ret = false;

    // schema versions before V2 didn't have the Sector runlog entry, but it can be synthesized from
    // the sector data
    if (sqlite3_exec(
            db,
            "INSERT INTO log SELECT l.runid, min(l.savepoint) as savepoint, l.sectorpoint, s.time, "
            "'Sector' AS id, s.type AS param1, s.seed AS param2, NULL as param3, NULL as param4 FROM log l "
            "JOIN sectors s ON (s.runid = l.runid AND s.sectorpoint = l.sectorpoint) "
            "WHERE l.savepoint > 0 "
            "GROUP BY l.runid, l.sectorpoint",
            NULL,
            NULL,
            NULL) != SQLITE_OK)
        ret = false;

    return ret;
}

static bool updateVer(sqlite3* db, int newver)
{
    bool ret = false;

    sqlite3_stmt* s;
    if (sqlite3_prepare_v2(db, "UPDATE subspace SET ver=?", -1, &s, NULL) != SQLITE_OK)
        return false;

    sqlite3_bind_int(s, 1, newver);

    if (sqlite3_step(s) == SQLITE_DONE)
        ret = true;

    sqlite3_finalize(s);
    return ret;
}

static bool dbUpgradeFromV0(sqlite3* db, int* destver)
{
    // version 0 only has subspace table; create the rest
    if (!dbCreateRuns(db) || !dbCreateSectors(db) || !dbCreateBeacons(db) || !dbCreateSaves(db) ||
        !dbCreateLog(db) || !dbCreateCombatLog(db))
        return false;

    // special case; this function upgrades stright to the current version and does not step through
    // the rest
    *destver = DB_CURRENT_SCHEMA_VER;

    return true;
}

static bool dbUpgradeFromV1(sqlite3* db, int* destver)
{
    return dbUpgradeV1Beacons(db);
}

static bool dbUpgradeFromV2(sqlite3* db, int* destver)
{
    return dbUpgradeV2RunLog(db);
}

static bool dbUpgradeOnce(sqlite3* db, int* ver)
{
    bool ret    = true;
    int origver = *ver;
    int destver = origver + 1;

    switch (*ver) {
    case 0:
        ret = dbUpgradeFromV0(db, &destver);
        break;
    case 1:
        ret = dbUpgradeFromV1(db, &destver);
        break;
    case 2:
        ret = dbUpgradeFromV2(db, &destver);
        break;
    default:
        ret = false;
    }

    if (ret) {
        ret = updateVer(db, destver);
    }

    if (ret) {
        logFmt(Notice,
               _S"Upgraded database schema from version ${int} to ${int}",
               stvar(int32, origver),
               stvar(int32, destver));
        *ver = destver;
        return true;
    } else {
        logFmt(Error, _S"Failed to upgrade database schema to version ${int}", stvar(int32, destver));
        return false;
    }
}

bool dbUpgradeSchema(sqlite3* db, int from)
{
    while (from < DB_CURRENT_SCHEMA_VER) {
        if (!dbUpgradeOnce(db, &from))
            return false;
    }
    return true;
}

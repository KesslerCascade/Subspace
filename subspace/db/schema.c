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
                     "result INTEGER NOT NULL DEFAULT 0,"
                     "start INTEGER NOT NULL DEFAULT 0,"
                     "end INTEGER NOT NULL DEFAULT 0,"
                     "savepoint INTEGER NOT NULL DEFAULT 0,"
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
                     "sectornum INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "type TEXT NOT NULL,"
                     "seed INTEGER NOT NULL,"
                     "PRIMARY KEY (runid, sectornum))",
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
                     "sectornum INTEGER NOT NULL,"
                     "beaconidx INTEGER NOT NULL,"
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

static bool dbCreateSaves(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE saves ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectornum INTEGER NOT NULL,"
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

static bool dbCreateEvents(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE events ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectornum INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "event TEXT NOT NULL,"
                     "param1,"
                     "param2,"
                     "param3,"
                     "param4"
                     ")",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX events_runid ON events (runid)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX events_runid_savepoint ON events (runid, savepoint)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX events_runid_sectornum ON events (runid, sectornum)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db, "CREATE INDEX events_event ON events (event)", NULL, NULL, NULL) !=
        SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create events table");

    return ret;
}

// copy and paste this because it's identical for now, but may change later
static bool dbCreateCombatEvents(sqlite3* db)
{
    bool ret = true;

    if (sqlite3_exec(db,
                     "CREATE TABLE combatevents ("
                     "runid INTEGER NOT NULL,"
                     "savepoint INTEGER NOT NULL,"
                     "sectornum INTEGER NOT NULL,"
                     "time INTEGER NOT NULL,"
                     "event TEXT NOT NULL,"
                     "param1,"
                     "param2,"
                     "param3,"
                     "param4"
                     ")",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX combatevents_runid ON combatevents (runid)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (sqlite3_exec(db,
                     "CREATE INDEX combatevents_runid_savepoint ON combatevents (runid, savepoint)",
                     NULL,
                     NULL,
                     NULL) != SQLITE_OK)
        ret = false;

    if (!ret)
        logStr(Error, _S"Failed to create combatevents table");

    return ret;
}

bool dbCreateSchema(sqlite3* db)
{
    bool ret = false;

    if (!dbCreateRuns(db) || !dbCreateSectors(db) || !dbCreateBeacons(db) || !dbCreateSaves(db) ||
        !dbCreateEvents(db) || !dbCreateCombatEvents(db))
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

static bool updateVer(sqlite3* db, int newver) {
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
        !dbCreateEvents(db) || !dbCreateCombatEvents(db))
        return false;

    // special case; this function upgrades stright to the current version and does not step through
    // the rest
    *destver = DB_CURRENT_SCHEMA_VER;

    return true;
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

#include "schema.h"
#include <sqlite3.h>

bool dbCreateSchema(sqlite3 *db)
{
    bool ret = false;

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

    sqlite3_bind_int(s, 1, DB_CURRENT_SCHEMA_VER);

    if (sqlite3_step(s) == SQLITE_DONE)
        ret = true;

    sqlite3_finalize(s);
    return ret;
}

bool dbUpgradeSchema(sqlite3* db, int from)
{
    return false;
}

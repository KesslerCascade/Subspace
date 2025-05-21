// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "database.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <sqlite3.h>
#include "schema.h"

_objfactory_guaranteed Database* Database_create(Subspace* ss)
{
    Database* self;
    self = objInstCreate(Database);

    self->ss = ss;

    objInstInit(self);
    return self;
}

bool Database_open(_In_ Database* self)
{
    string fspath = 0;
    bool ret      = false;

    vfsCreateAll(self->ss->fs, DBDIR_FILENAME);
    if (!vfsGetFSPath(&fspath, self->ss->fs, DBDIR_FILENAME))
        return false;

    pathJoin(&fspath, fspath, DB_FILENAME);
    pathToPlatform(&fspath, fspath);

    // SQLITE_OPEN_NOMUTEX is used because we're using a TRFifo to simulate a mutex rather than
    // using sqlite's mutexes
    if (sqlite3_open_v2(strC(fspath),
                        &self->db,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX,
                        NULL) == SQLITE_OK)
        ret = true;

    sqlite3_exec(self->db, "PRAGMA journal_mode=WAL", NULL, NULL, NULL);

    strDestroy(&fspath);
    return ret;
}

bool Database_check(_In_ Database* self)
{
    bool ret = false;

    // check if schema table exists (i.e. blank database)
    sqlite3_stmt* s;
    if (sqlite3_prepare_v2(self->db,
                           "SELECT name FROM sqlite_master WHERE type='table' AND name='subspace'",
                           -1,
                           &s,
                           NULL) != SQLITE_OK)
        return false;

    int code = sqlite3_step(s);
    sqlite3_finalize(s);

    // tables does not exist, create the schema
    if (code == SQLITE_DONE)
        return dbCreateSchema(self->db);

    if (code != SQLITE_ROW)
        return false;   // something else went wrong

    int ver = -1;
    // get current schema version
    if (sqlite3_prepare_v2(self->db, "SELECT ver FROM subspace", -1, &s, NULL) != SQLITE_OK)
        return false;
    code = sqlite3_step(s);
    if (code == SQLITE_ROW)
        ver = sqlite3_column_int(s, 0);
    sqlite3_finalize(s);

    if (ver == -1)
        return dbCreateSchema(self->db);   // shouldn't happen, but create schema if it does

    if (ver < DB_CURRENT_SCHEMA_VER)
        return dbUpgradeSchema(self->db, ver);

    return true;
}

void Database_close(_In_ Database* self)
{
    if (self->db) {
        sqlite3_close(self->db);
        self->db = NULL;
    }
    return;
}

void Database_destroy(_In_ Database* self)
{
    databaseClose(self);
    // Autogen begins -----
    objRelease(&self->fifo);
    // Autogen ends -------
}

// Autogen begins -----
#include "database.auto.inc"
// Autogen ends -------

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "db/dbstmt.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <sqlite3.h>
#include "db/database.h"

_objfactory_check DbStmt* DbStmt_create(Database* db, _In_opt_ strref sql)
{
    DbStmt* self;
    self = objInstCreate(DbStmt);

    if (sqlite3_prepare(db->db, strC(sql), strLen(sql), &self->stmt, NULL) != SQLITE_OK) {
        xaFree(self);
        return NULL;
    }
    self->db = objAcquire(db);

    if (!objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

_objinit_guaranteed bool DbStmt_init(_In_ DbStmt* self)
{
    // Autogen begins -----
    saInit(&self->row, stvar, 1);
    return true;
    // Autogen ends -------
}

bool DbStmt_bind(_In_ DbStmt* self, int ncol, stvar val)
{
    bool ret = false;

    if (stvarIs(&val, none)){
        if (sqlite3_bind_null(self->stmt, ncol) == SQLITE_OK)
            ret = true;
    } else if (stvarIs(&val, string)) {
        if (sqlite3_bind_text(self->stmt,
                              ncol,
                              strC(val.data.st_string),
                              strLen(val.data.st_string),
                              SQLITE_TRANSIENT) == SQLITE_OK)
            ret = true;
    } else if (STYPE_CLASS(val.type) == STCLASS_INT || STYPE_CLASS(val.type) == STCLASS_UINT) {
        int64 ival = 0;
        if (stConvert(int64, &ival, stvar, val, ST_Lossless) &&
            sqlite3_bind_int64(self->stmt, ncol, ival) == SQLITE_OK)
            ret = true;
    } else if (STYPE_CLASS(val.type) == STCLASS_FLOAT) {
        float64 dval = 0;
        if (stConvert(float64, &dval, stvar, val, ST_Lossless) &&
            sqlite3_bind_double(self->stmt, ncol, dval) == SQLITE_OK)
            ret = true;
    }

    if (!ret) {
        // fallback, try to convert it to a string
        string tmp = 0;
        if (stConvert(string, &tmp, stvar, val)) {
            if (sqlite3_bind_text(self->stmt, ncol, strC(tmp), strLen(tmp), SQLITE_TRANSIENT) ==
                SQLITE_OK)
                ret = true;
            strDestroy(&tmp);
        }
    }

    return ret;
}

static bool DbStmt_parseRow(DbStmt* self)
{
    bool ret = true;
    saClear(&self->row);

    int rc = sqlite3_column_count(self->stmt);
    saSetSize(&self->row, rc);
    for (int i = 0; i < rc; i++) {
        int typ = sqlite3_column_type(self->stmt, i);

        switch (typ) {
        case SQLITE_INTEGER:
            self->row.a[i] = stvar(int64, sqlite3_column_int64(self->stmt, i));
            break;
        case SQLITE_FLOAT:
            self->row.a[i] = stvar(float64, sqlite3_column_double(self->stmt, i));
            break;
        case SQLITE_TEXT: {
            string str = 0;
            strCopy(&str, (strref)sqlite3_column_text(self->stmt, i));
            self->row.a[i] = stvar(string, str);
        } break;
        case SQLITE_NULL:
            self->row.a[i] = stvar(none, NULL);
            break;
        default:
            // unknown type
            logFmt(Warn, _S"Unknown data type ${int} int database", stvar(int32, typ));
            ret = false;
            break;
        }
    }

    return ret;
}

bool DbStmt_exec(_In_ DbStmt* self)
{
    int res  = sqlite3_step(self->stmt);
    bool ret = false;

    if (res == SQLITE_ROW) {
        ret = DbStmt_parseRow(self);
    } else {
        saClear(&self->row);
    }

    if (res == SQLITE_DONE)
        ret = true;

    return ret;
}

void DbStmt_reset(_In_ DbStmt* self)
{
    sqlite3_reset(self->stmt);
}

void DbStmt_clearBind(_In_ DbStmt* self)
{
    sqlite3_clear_bindings(self->stmt);
}

void DbStmt_destroy(_In_ DbStmt* self)
{
    sqlite3_finalize(self->stmt);
    // Autogen begins -----
    objRelease(&self->db);
    saDestroy(&self->row);
    // Autogen ends -------
}

// Autogen begins -----
#include "db/dbstmt.auto.inc"
// Autogen ends -------

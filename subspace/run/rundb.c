#include "rundb.h"
#include <cx/string.h>
#include "db/database.h"

bool rundbSetAbandoned(Subspace* ss, int64 runid)
{
    bool ret     = false;
    DbStmt* stmt = dbstmtCreate(ss->db, _S"UPDATE runs SET result=3 WHERE runid=? AND result=0");
    if (!stmt)
        return false;

    dbstmtBind(stmt, 1, stvar(int64, runid));
    if (dbstmtExec(stmt))
        ret = true;
    objRelease(&stmt);

    return ret;
}

static bool clearTable(Database* db, strref table, int64 runid)
{
    bool ret   = false;
    string sql = 0;
    strNConcat(&sql, _S"DELETE FROM ", table, _S" WHERE runid=?");
    DbStmt* stmt = dbstmtCreate(db, sql);
    strDestroy(&sql);

    if (!stmt)
        return false;

    dbstmtBind(stmt, 1, stvar(int64, runid));
    if (dbstmtExec(stmt))
        ret = true;
    objRelease(&stmt);

    return ret;
}

bool rundbDelete(Subspace* ss, int64 runid)
{
    bool ret     = false;
    DbStmt* stmt = dbstmtCreate(ss->db, _S"BEGIN");
    if (!stmt || !dbstmtExec(stmt))
        goto out;
    objRelease(&stmt);

    ret = clearTable(ss->db, _S"beacons", runid) && clearTable(ss->db, _S"combatlog", runid) &&
        clearTable(ss->db, _S"log", runid) && clearTable(ss->db, _S"saves", runid) &&
        clearTable(ss->db, _S"sectors", runid) && clearTable(ss->db, _S"runs", runid);

    stmt = dbstmtCreate(ss->db, ret ? _S"COMMIT" : _S"ROLLBACK");
    if (!stmt || !dbstmtExec(stmt))
        goto out;
    objRelease(&stmt);

out:
    objRelease(&stmt);
    return ret;
}

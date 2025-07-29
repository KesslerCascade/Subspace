#include <cx/format.h>
#include <cx/utils.h>
#include "db/database.h"
#include "feature/featureregistry.h"
#include "feature/runtracker/runtracker.h"
#include "lang/lang.h"
#include "runinfo.h"

static LazyInitState rundirvars_init_state;
static hashtable rundirvars;

static void rundirvars_init(void* dummy)
{
    //${year}, ${month}, ${day}, ${hour}, ${minute}, ${second}, ${shipname}, ${shiptype},
    //${difficulty}. and ${seed}."
    htInit(&rundirvars, string, string, 20);
    htInsert(&rundirvars, string, _S"year", string, _S"${0uint(4):year;0000}");
    htInsert(&rundirvars, string, _S"month", string, _S"${0uint(2):month;00}");
    htInsert(&rundirvars, string, _S"day", string, _S"${0uint(2):day;00}");
    htInsert(&rundirvars, string, _S"hour", string, _S"${0uint(2):hour;00}");
    htInsert(&rundirvars, string, _S"minute", string, _S"${0uint(2):minute;00}");
    htInsert(&rundirvars, string, _S"second", string, _S"${0uint(2):second;00}");
    htInsert(&rundirvars, string, _S"shipname", string, _S"${string:shipname;The Nameless One}");
    htInsert(&rundirvars, string, _S"shiptype", string, _S"${string:shiptype;Unknown A}");
    htInsert(&rundirvars, string, _S"difficulty", string, _S"${string:difficulty;Impossible}");
    htInsert(&rundirvars, string, _S"seed", string, _S"${uint:seed;0}");
}

bool RunInfo_getRunDir(_In_ RunInfo* self, string* out)
{
    lazyInit(&rundirvars_init_state, rundirvars_init, NULL);

    bool ret = false;
    withReadLock (&self->lock) {
        if (!strEmpty(self->runDir)) {
            strDup(out, self->runDir);
            ret = true;
        }
    }

    if (ret)
        return ret;

    // need to calculate one and save it
    string temp = 0, fmt = 0;
    hashtable ivars, svars;
    RunTracker* rt = fregGet(RunTracker, self->ss->freg);
    if (!rt)
        return false;

    // create format string by substituting in variables
    ssdStringOutD(rt->settings, _S"rundirfmt", &temp, RUNDIRFMT_DEFAULT);
    pathSubst(&fmt, temp, rundirvars);

    // hash tables for integer and string vars
    htInit(&ivars, string, uint32, 10);
    htInit(&svars, string, string, 10);

    withWriteLock (&self->lock) {
        TimeParts tp;
        timeDecompose(&tp, toLocalTime(self->startTime));
        htInsert(&ivars, string, _S"year", uint32, tp.year);
        htInsert(&ivars, string, _S"month", uint32, tp.month);
        htInsert(&ivars, string, _S"day", uint32, tp.day);
        htInsert(&ivars, string, _S"hour", uint32, tp.hour);
        htInsert(&ivars, string, _S"minute", uint32, tp.minute);
        htInsert(&ivars, string, _S"second", uint32, tp.second);
        htInsert(&ivars, string, _S"seed", uint32, self->seed);

        htInsert(&svars, string, _S"shipname", string, self->shipName);

        strConcat(&temp, _S"ship_shortname_", self->shipType);
        htInsert(&svars, string, _S"shiptype", strref, langGetD(self->ss, temp, self->shipType));

        string diffstr = _S"difficulty_unknown";
        if (self->difficulty == 2)
            diffstr = _S"difficulty_hard";
        else if (self->difficulty == 1)
            diffstr = _S"difficulty_normal";
        else if (self->difficulty == 0)
            diffstr = _S"difficulty_easy";
        htInsert(&svars, string, _S"difficulty", strref, langGet(self->ss, diffstr));

        strFormat(&temp, fmt, stvar(hashtable, ivars), stvar(hashtable, svars));
        pathFilter(&self->runDir, temp);

        if (!strEmpty(self->runDir)) {
            DbStmt* stmt = dbPrepare(self->ss->db, _S"UPDATE runs SET rundir=? WHERE runid=?");
            if (stmt) {
                dbstmtBind(stmt, 1, stvar(string, self->runDir));
                dbstmtBind(stmt, 2, stvar(int64, self->runid));
                ret = dbstmtExec(stmt);
            }
            objRelease(&stmt);
        }

        if (ret)
            strDup(out, self->runDir);
        else
            strDestroy(&self->runDir);
    }

    htDestroy(&ivars);
    htDestroy(&svars);
    strDestroy(&temp);
    strDestroy(&fmt);
    return ret;
}

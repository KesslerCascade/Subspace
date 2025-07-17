// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "scraptracker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "control/controlserver.h"
#include "runinfo.h"

_objfactory_guaranteed ScrapTracker* ScrapTracker_create(RunInfo* run)
{
    ScrapTracker* self;
    self = objInstCreate(ScrapTracker);

    self->ss  = run->ss;
    self->run = objGetWeak(RunInfo, run);

    objInstInit(self);

    // size so these top out at around 50% occupancy
    htInit(&self->beacons, int64, opaque(ScrapTotals), 200);
    htInit(&self->sectors, int64, opaque(ScrapTotals), 16);

    return self;
}

_objinit_guaranteed bool ScrapTracker_init(_In_ ScrapTracker* self)
{
    logrelaySubscribe(self->ss->runlog, self, _S"Scrap");

    // Autogen begins -----
    rwlockInit(&self->lock);
    return true;
    // Autogen ends -------
}

void ScrapTracker_reset(_In_ ScrapTracker* self)
{
    withWriteLock (&self->lock) {
        htClear(&self->beacons);
        htClear(&self->sectors);
        memset(&self->total, 0, sizeof(self->total));
    }
    ssuiNotify(self->ss->ui, _S"Scrap_Reset", stvNone);
}

bool ScrapTracker_getTotal(_In_ ScrapTracker* self, ScrapTotals* out)
{
    withReadLock (&self->lock) {
        memcpy(out, &self->total, sizeof(ScrapTotals));
    }
    return true;
}

bool ScrapTracker_getBeacon(_In_ ScrapTracker* self, int64 savepoint, ScrapTotals* out)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = htFind(self->beacons, int64, savepoint, opaque, out);
    }
    return ret;
}

bool ScrapTracker_getSector(_In_ ScrapTracker* self, int64 sectorpoint, ScrapTotals* out)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = htFind(self->sectors, int64, sectorpoint, opaque, out);
    }
    return ret;
}

static void ScrapTotalAggregate(ScrapTotals* dest, ScrapTotals* src)
{
    dest->rewards += src->rewards;
    dest->rawrewards += src->rawrewards;
    dest->eventloss += src->eventloss;

    dest->dropvalue += src->dropvalue;
    dest->rawdropvalue += src->rawdropvalue;

    dest->storesold += src->storesold;
    dest->storebought += src->storebought;
    dest->storerepair += src->storerepair;

    dest->sysupg += src->sysupg;
    dest->powerupg += src->powerupg;
}

static void ScrapTotalAddToHash(hashtable* tbl, int64 key, ScrapTotals* src)
{
    htelem elem = htFind(*tbl, int64, key, none, NULL);
    if (!elem) {
        ScrapTotals nt = { 0 };
        elem           = htInsert(tbl, int64, key, opaque, nt);
    }

    ScrapTotals* dest = hteValPtr(*tbl, opaque, elem);
    ScrapTotalAggregate(dest, src);
}

void ScrapTracker_logNotify(_In_ ScrapTracker* self, LogEnt* ent, bool replay)
{
    if (ent->spec == &Log_Scrap) {
        ScrapTotals delta = { 0 };

        strref source = cfieldString(ent->params, _S"source");
        int amount    = cfieldValD(int32, ent->params, _S"amount", 0);
        int rawamount = cfieldValD(int32, ent->params, _S"rawamount", 0);

        if (strEq(source, _S"Event")) {
            if (amount > 0) {
                delta.rewards    = amount;
                delta.rawrewards = rawamount;
            } else if (amount < 0) {
                delta.eventloss = -amount;
            }
        }

        ScrapTotalAddToHash(&self->beacons, ent->savepoint, &delta);
        ScrapTotalAddToHash(&self->sectors, ent->sectorpoint, &delta);
        ScrapTotalAggregate(&self->total, &delta);

        ssuiNotify(self->ss->ui,
                   _S"Scrap_Update",
                   stvar(int64, ent->savepoint),
                   stvar(int64, ent->sectorpoint),
                   stvar(bool, replay));
    }
}

void ScrapTracker_logReset(_In_ ScrapTracker* self)
{
    scraptrackerReset(self);
}

void ScrapTracker_logReplayComplete(_In_ ScrapTracker* self)
{
    ssuiNotify(self->ss->ui, _S"Scrap_Refresh", stvNone);
}

void ScrapTracker_destroy(_In_ ScrapTracker* self)
{
    // Autogen begins -----
    objDestroyWeak(&self->run);
    rwlockDestroy(&self->lock);
    htDestroy(&self->sectors);
    htDestroy(&self->beacons);
    // Autogen ends -------
}

// Autogen begins -----
#include "scraptracker.auto.inc"
// Autogen ends -------

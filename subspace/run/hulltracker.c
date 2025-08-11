// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "run/hulltracker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "control/controlserver.h"
#include "run/runinfo.h"

_objfactory_guaranteed HullTracker* HullTracker_create(RunInfo* run)
{
    HullTracker* self;
    self = objInstCreate(HullTracker);

    self->ss  = run->ss;
    self->run = objGetWeak(RunInfo, run);

    objInstInit(self);

    // size so these top out at around 50% occupancy
    htInit(&self->beacons, int64, opaque(HullTotals), 200);
    htInit(&self->sectors, int64, opaque(HullTotals), 16);

    return self;
}

_objinit_guaranteed bool HullTracker_init(_In_ HullTracker* self)
{
    logrelaySubscribe(self->ss->runlog, self, _S"HullDamage");
    logrelaySubscribe(self->ss->runlog, self, _S"Sector");

    // Autogen begins -----
    rwlockInit(&self->lock);
    return true;
    // Autogen ends -------
}

bool HullTracker_getTotal(_In_ HullTracker* self, HullTotals* out)
{
    withReadLock (&self->lock) {
        memcpy(out, &self->total, sizeof(HullTotals));
    }
    return true;
}

bool HullTracker_getBeacon(_In_ HullTracker* self, int64 savepoint, HullTotals* out)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = htFind(self->beacons, int64, savepoint, opaque, out);
    }
    return ret;
}

bool HullTracker_getSector(_In_ HullTracker* self, int64 sectorpoint, HullTotals* out)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = htFind(self->sectors, int64, sectorpoint, opaque, out);
    }
    return ret;
}

void HullTracker_reset(_In_ HullTracker* self)
{
    withWriteLock (&self->lock) {
        htClear(&self->beacons);
        htClear(&self->sectors);
        memset(&self->total, 0, sizeof(self->total));
    }
    ssuiNotify(self->ss->ui, _S"Hull_Reset", stvNone);
}

static void HullTotalAggregate(HullTotals* dest, HullTotals* src)
{
    dest->projectile += src->projectile;
    dest->missile += src->missile;
    dest->beam += src->beam;
    dest->asteroid += src->asteroid;
    dest->asb += src->asb;
    dest->solarflare += src->solarflare;
    dest->systemdestroyed += src->systemdestroyed;
    dest->event += src->event;
    dest->other += src->other;

    dest->damage += src->projectile + src->missile + src->beam + src->asteroid + src->asb +
        src->solarflare + src->systemdestroyed + src->event + src->other;

    dest->repair += src->repair;
}

static void HullTotalAddToHash(hashtable* tbl, int64 key, HullTotals* src)
{
    htelem elem = htFind(*tbl, int64, key, none, NULL);
    if (!elem) {
        HullTotals nt = { 0 };
        elem          = htInsert(tbl, int64, key, opaque, nt);
    }

    HullTotals* dest = hteValPtr(*tbl, opaque, elem);
    HullTotalAggregate(dest, src);
}

void HullTracker_logNotify(_In_ HullTracker* self, LogEnt* ent, bool replay)
{
    if (ent->spec == &Log_HullDamage) {
        HullTotals delta = { 0 };

        strref source = cfieldString(ent->params, _S"source");
        int amount    = cfieldValD(int32, ent->params, _S"amount", 0);

        if (amount > 0) {
            if (strEq(source, _S"Projectile"))
                delta.projectile = amount;
            else if (strEq(source, _S"Missile"))
                delta.missile = amount;
            else if (strEq(source, _S"Beam"))
                delta.beam = amount;
            else if (strEq(source, _S"Asteroid"))
                delta.asteroid = amount;
            else if (strEq(source, _S"ASB"))
                delta.asb = amount;
            else if (strEq(source, _S"SolarFlare"))
                delta.solarflare = amount;
            else if (strEq(source, _S"SystemDestroyed"))
                delta.systemdestroyed = amount;
            else if (strEq(source, _S"Event"))
                delta.event = amount;
            else
                delta.other = amount;
        } else if (amount < 0) {
            delta.repair = -amount;
        }

        HullTotalAddToHash(&self->beacons, ent->savepoint, &delta);
        HullTotalAddToHash(&self->sectors, ent->sectorpoint, &delta);
        HullTotalAggregate(&self->total, &delta);

        ssuiNotify(self->ss->ui,
                   _S"Hull_Update",
                   stvar(int64, ent->savepoint),
                   stvar(int64, ent->sectorpoint),
                   stvar(bool, replay));
    } else if (ent->spec = &Log_Sector) {
        // when entering a new sector, pre-create the aggregate entries in the hashes so they
        // already exist
        HullTotals delta = { 0 };
        HullTotalAddToHash(&self->sectors, ent->sectorpoint, &delta);

        // this helps keep the UI updated
        ssuiNotify(self->ss->ui,
                   _S"Hull_Update",
                   stvar(int64, ent->savepoint),
                   stvar(int64, ent->sectorpoint),
                   stvar(bool, replay));
    }
}

void HullTracker_logReset(_In_ HullTracker* self)
{
    hulltrackerReset(self);
}

void HullTracker_logReplayComplete(_In_ HullTracker* self)
{
    ssuiNotify(self->ss->ui, _S"Hull_Refresh", stvNone);
}

void HullTracker_destroy(_In_ HullTracker* self)
{
    // Autogen begins -----
    objDestroyWeak(&self->run);
    rwlockDestroy(&self->lock);
    htDestroy(&self->sectors);
    htDestroy(&self->beacons);
    // Autogen ends -------
}

// Autogen begins -----
#include "run/hulltracker.auto.inc"
// Autogen ends -------

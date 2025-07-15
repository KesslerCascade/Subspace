// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "logent.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/utils.h>

static hashtable spechash;

static LazyInitState specinitstate;
static void addspec(LogEntSpec* spec)
{
    htInsert(&spechash, strref, (strref)spec->id, ptr, spec);
}

static void specinit(void* dummy)
{
    htInit(&spechash, string, ptr, 10);
    addspec(&Log_Scrap);
    addspec(&Log_Victory);
    addspec(&Log_Defeat);
    addspec(&Log_Abandon);
    addspec(&Log_HullDamage);
    addspec(&Log_Jump);
    addspec(&Log_Wait);
    addspec(&Log_Event);
    addspec(&Log_Sector);
    addspec(&Log_Ship);
    addspec(&Log_Start);
}

void LogEnt_destroy(_In_ LogEnt* self)
{
    // Autogen begins -----
    strDestroy(&self->id);
    saDestroy(&self->rawparams);
    htDestroy(&self->params);
    // Autogen ends -------
}

_objinit_guaranteed bool LogEnt_init(_In_ LogEnt* self)
{
    // Autogen begins -----
    saInit(&self->rawparams, stvar, 4);
    htInit(&self->params, string, stvar, 16);
    return true;
    // Autogen ends -------
}

_objfactory_check LogEnt*
LogEnt_create(int64 sectorpoint, int64 savepoint, int64 time, float64 gametime, _In_opt_ strref id,
              stvar params[LOG_MAX_PARAMS])
{
    lazyInit(&specinitstate, specinit, NULL);

    LogEnt* self;
    self = objInstCreate(LogEnt);

    self->sectorpoint = sectorpoint;
    self->savepoint   = savepoint;
    self->time        = time;
    self->gametime    = gametime;
    strDup(&self->id, id);
    htFind(spechash, strref, id, ptr, &self->spec);

    if (!self->spec) {
        xaFree(self);
        return NULL;
    }

    objInstInit(self);

    for (int i = 0; i < self->spec->numParams; i++) {
        saPush(&self->rawparams, stvar, params[i]);
        htInsert(&self->params, strref, (strref)self->spec->paramNames[i], stvar, params[i]);
    }

    return self;
}

// Autogen begins -----
#include "logent.auto.inc"
// Autogen ends -------

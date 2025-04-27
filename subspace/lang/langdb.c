// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "langdb.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/serialize/jsonparse.h>

_objfactory_guaranteed LanguageDB* LanguageDB_create()
{
    LanguageDB* self;
    self = objInstCreate(LanguageDB);
    objInstInit(self);
    return self;
}

_objinit_guaranteed bool LanguageDB_init(_In_ LanguageDB* self)
{
    // Autogen begins -----
    htInit(&self->primary, string, string, 16);
    htInit(&self->fallback, string, string, 16);
    return true;
    // Autogen ends -------
}

static void langParseCB(_In_ JSONParseEvent *ev, _Inout_opt_ void *userdata)
{
    hashtable* tgt = (hashtable*)userdata;

    if (ev->etype != JSON_String || ev->ctx->ctype != JSON_Object)
        return;

    htInsert(tgt, string, ev->ctx->cdata.curKey, string, ev->edata.strData);
}

bool LanguageDB_load(_In_ LanguageDB* self, StreamBuffer* sb, bool fallback)
{
    return jsonParse(sb, langParseCB, fallback ? &self->fallback : &self->primary);
}

strref LanguageDB_get(_In_ LanguageDB* self, _In_opt_ strref key)
{
    htelem elem;

    elem = htFind(self->primary, strref, key, none, NULL);
    if (elem)
        return hteVal(self->primary, strref, elem);

    elem = htFind(self->fallback, strref, key, none, NULL);
    if (elem)
        return hteVal(self->fallback, strref, elem);

    return key;
}

void LanguageDB_destroy(_In_ LanguageDB* self)
{
    // Autogen begins -----
    htDestroy(&self->primary);
    htDestroy(&self->fallback);
    // Autogen ends -------
}

// Autogen begins -----
#include "langdb.auto.inc"
// Autogen ends -------

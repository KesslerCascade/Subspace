// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "uistrcache.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "lang/lang.h"

_objinit_guaranteed bool UIStrCache_init(_In_ UIStrCache* self)
{
    // Autogen begins -----
    htInit(&self->cache, string, none, 16);
    return true;
    // Autogen ends -------
}

_objfactory_guaranteed UIStrCache* UIStrCache_create(Subspace* ss)
{
    UIStrCache* self;
    self = objInstCreate(UIStrCache);

    self->ss = ss;

    objInstInit(self);
    return self;
}

strref UIStrCache_add(_In_ UIStrCache* self, _In_opt_ strref str)
{
    htelem elem = htInsert(&self->cache, strref, str, none, NULL, HT_Ignore);
    return hteKey(self->cache, string, elem);
}

const_char* UIStrCache_cadd(_In_ UIStrCache* self, _In_opt_ strref str)
{
    htelem elem = htInsert(&self->cache, strref, str, none, NULL, HT_Ignore);
    return strPC(hteKeyPtr(self->cache, string, elem));
}

strref UIStrCache_trans(_In_ UIStrCache* self, _In_opt_ strref lang_key)
{
    strref translated = langGet(self->ss, lang_key);
    return UIStrCache_add(self, translated);
}

const_char* UIStrCache_ctrans(_In_ UIStrCache* self, _In_opt_ strref lang_key)
{
    strref translated = langGet(self->ss, lang_key);
    return UIStrCache_cadd(self, translated);
}

void UIStrCache_destroy(_In_ UIStrCache* self)
{
    // Autogen begins -----
    htDestroy(&self->cache);
    // Autogen ends -------
}

// Autogen begins -----
#include "uistrcache.auto.inc"
// Autogen ends -------

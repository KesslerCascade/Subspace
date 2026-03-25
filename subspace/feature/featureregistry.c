// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/featureregistry.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "features.h"

_objfactory_guaranteed FeatureRegistry* FeatureRegistry_create(Subspace* ss)
{
    FeatureRegistry* self;
    self = objInstCreate(FeatureRegistry);

    self->ss = ss;

    objInstInit(self);
    registerAllFeatures(self);
    return self;
}

_objinit_guaranteed bool FeatureRegistry_init(_In_ FeatureRegistry* self)
{
    // Autogen begins -----
    htInit(&self->features, string, object, 16);
    return true;
    // Autogen ends -------
}

SubspaceFeature* FeatureRegistry_find(_In_ FeatureRegistry* self, _In_opt_ strref name)
{
    htelem elem = htFind(self->features, strref, name, none, NULL);
    if (elem)
        return (SubspaceFeature*)hteVal(self->features, object, elem);
    return NULL;
}

void FeatureRegistry_destroy(_In_ FeatureRegistry* self)
{
    // Autogen begins -----
    htDestroy(&self->features);
    // Autogen ends -------
}

bool FeatureRegistry_isEnabled(_In_ FeatureRegistry* self, _In_opt_ strref name)
{
    htelem elem = htFind(self->features, strref, name, none, NULL);
    if (!elem)
        return false;

    return featureIsEnabled((SubspaceFeature*)hteVal(self->features, object, elem));
}

// Autogen begins -----
// clang-format off
#include "feature/featureregistry.auto.inc"
// clang-format on
// Autogen ends -------

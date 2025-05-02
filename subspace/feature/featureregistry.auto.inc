// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
FeatureRegistry_ClassIf FeatureRegistry_ClassIf_tmpl = {
    ._size = sizeof(FeatureRegistry_ClassIf),
};

static FeatureRegistry_ClassIf _impl_FeatureRegistry_FeatureRegistry_ClassIf = {
    ._size = sizeof(FeatureRegistry_ClassIf),
    ._implements = (ObjIface*)&FeatureRegistry_ClassIf_tmpl,
    .find = (SubspaceFeature* (*)(void*, strref))FeatureRegistry_find,
};

static ObjIface* _ifimpl_FeatureRegistry[] = {
    (ObjIface*)&_impl_FeatureRegistry_FeatureRegistry_ClassIf,
    NULL
};

ObjClassInfo FeatureRegistry_clsinfo = {
    .instsize = sizeof(FeatureRegistry),
    .classif = (ObjIface*)&FeatureRegistry_ClassIf_tmpl,
    .init = (bool(*)(void*))FeatureRegistry_init,
    .destroy = (void(*)(void*))FeatureRegistry_destroy,
    .ifimpl = _ifimpl_FeatureRegistry,
};

// ==================== Auto-generated section ends ======================

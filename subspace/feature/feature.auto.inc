// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SubspaceFeature_ClassIf SubspaceFeature_ClassIf_tmpl = {
    ._size = sizeof(SubspaceFeature_ClassIf),
};

static SubspaceFeature_ClassIf _impl_SubspaceFeature_SubspaceFeature_ClassIf = {
    ._size = sizeof(SubspaceFeature_ClassIf),
    ._implements = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
    .getSettingsPage = (SettingsPage* (*)(void*))SubspaceFeature_getSettingsPage,
    .enable = (void (*)(void*, bool))SubspaceFeature_enable,
};

static ObjIface* _ifimpl_SubspaceFeature[] = {
    (ObjIface*)&_impl_SubspaceFeature_SubspaceFeature_ClassIf,
    NULL
};

ObjClassInfo SubspaceFeature_clsinfo = {
    .instsize = sizeof(SubspaceFeature),
    .classif = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
    .init = (bool(*)(void*))SubspaceFeature_init,
    .destroy = (void(*)(void*))SubspaceFeature_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_SubspaceFeature,
};

static ObjIface* _ifimpl_ClientFeature[] = {
    NULL
};

ObjClassInfo ClientFeature_clsinfo = {
    .instsize = sizeof(ClientFeature),
    .destroy = (void(*)(void*))ClientFeature_destroy,
    .ifimpl = _ifimpl_ClientFeature,
};

// ==================== Auto-generated section ends ======================

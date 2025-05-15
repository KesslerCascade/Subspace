// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Tweaks_ClassIf Tweaks_ClassIf_tmpl = {
    ._size = sizeof(Tweaks_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

static Tweaks_ClassIf _impl_Tweaks_Tweaks_ClassIf = {
    ._size = sizeof(Tweaks_ClassIf),
    ._implements = (ObjIface*)&Tweaks_ClassIf_tmpl,
    .createSettingsPage = (SettingsPage* (*)(void*, SubspaceUI*))Tweaks_createSettingsPage,
    .applyDefaultSettings = (void (*)(void*))Tweaks_applyDefaultSettings,
};

static ObjIface* _ifimpl_Tweaks[] = {
    (ObjIface*)&_impl_Tweaks_Tweaks_ClassIf,
    NULL
};

ObjClassInfo Tweaks_clsinfo = {
    .instsize = sizeof(Tweaks),
    .classif = (ObjIface*)&Tweaks_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_Tweaks,
};

// ==================== Auto-generated section ends ======================

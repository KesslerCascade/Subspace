// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SaveManager_ClassIf SaveManager_ClassIf_tmpl = {
    ._size = sizeof(SaveManager_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

static SaveManager_ClassIf _impl_SaveManager_SaveManager_ClassIf = {
    ._size = sizeof(SaveManager_ClassIf),
    ._implements = (ObjIface*)&SaveManager_ClassIf_tmpl,
    .createSettingsPage = (SettingsPage* (*)(void*, SubspaceUI*))SaveManager_createSettingsPage,
    .applyDefaultSettings = (void (*)(void*))SaveManager_applyDefaultSettings,
};

static ObjIface* _ifimpl_SaveManager[] = {
    (ObjIface*)&_impl_SaveManager_SaveManager_ClassIf,
    NULL
};

ObjClassInfo SaveManager_clsinfo = {
    .instsize = sizeof(SaveManager),
    .classif = (ObjIface*)&SaveManager_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_SaveManager,
};

// ==================== Auto-generated section ends ======================

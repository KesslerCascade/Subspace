// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
InfoBlock_ClassIf InfoBlock_ClassIf_tmpl = {
    ._size = sizeof(InfoBlock_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

static InfoBlock_ClassIf _impl_InfoBlock_InfoBlock_ClassIf = {
    ._size = sizeof(InfoBlock_ClassIf),
    ._implements = (ObjIface*)&InfoBlock_ClassIf_tmpl,
    .createSettingsPage = (SettingsPage* (*)(void*, SubspaceUI*))InfoBlock_createSettingsPage,
    .applyDefaultSettings = (void (*)(void*))InfoBlock_applyDefaultSettings,
};

static ObjIface* _ifimpl_InfoBlock[] = {
    (ObjIface*)&_impl_InfoBlock_InfoBlock_ClassIf,
    NULL
};

ObjClassInfo InfoBlock_clsinfo = {
    .instsize = sizeof(InfoBlock),
    .classif = (ObjIface*)&InfoBlock_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_InfoBlock,
};

// ==================== Auto-generated section ends ======================

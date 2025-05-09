// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SettingsPage_ClassIf SettingsPage_ClassIf_tmpl = {
    ._size = sizeof(SettingsPage_ClassIf),
};

static SettingsPage_ClassIf _impl_SettingsPage_SettingsPage_ClassIf = {
    ._size = sizeof(SettingsPage_ClassIf),
    ._implements = (ObjIface*)&SettingsPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))SettingsPage_make,
    .update = (bool (*)(void*))SettingsPage_update,
};

static ObjIface* _ifimpl_SettingsPage[] = {
    (ObjIface*)&_impl_SettingsPage_SettingsPage_ClassIf,
    NULL
};

ObjClassInfo SettingsPage_clsinfo = {
    .instsize = sizeof(SettingsPage),
    .classif = (ObjIface*)&SettingsPage_ClassIf_tmpl,
    .destroy = (void(*)(void*))SettingsPage_destroy,
    .ifimpl = _ifimpl_SettingsPage,
};

// ==================== Auto-generated section ends ======================

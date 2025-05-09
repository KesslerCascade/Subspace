// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SettingsWin_ClassIf SettingsWin_ClassIf_tmpl = {
    ._size = sizeof(SettingsWin_ClassIf),
};

static SettingsWin_ClassIf _impl_SettingsWin_SettingsWin_ClassIf = {
    ._size = sizeof(SettingsWin_ClassIf),
    ._implements = (ObjIface*)&SettingsWin_ClassIf_tmpl,
    .showPage = (void (*)(void*, int))SettingsWin_showPage,
    .showPageByList = (void (*)(void*, int))SettingsWin_showPageByList,
    .make = (bool (*)(void*))SettingsWin_make,
    .show = (void (*)(void*))SettingsWin_show,
    .updateList = (void (*)(void*))SettingsWin_updateList,
    .updatePage = (bool (*)(void*, strref))SettingsWin_updatePage,
    .updateAll = (void (*)(void*))SettingsWin_updateAll,
    .finish = (void (*)(void*))SettingsWin_finish,
};

static ObjIface* _ifimpl_SettingsWin[] = {
    (ObjIface*)&_impl_SettingsWin_SettingsWin_ClassIf,
    NULL
};

ObjClassInfo SettingsWin_clsinfo = {
    .instsize = sizeof(SettingsWin),
    .classif = (ObjIface*)&SettingsWin_ClassIf_tmpl,
    .init = (bool(*)(void*))SettingsWin_init,
    .destroy = (void(*)(void*))SettingsWin_destroy,
    .ifimpl = _ifimpl_SettingsWin,
};

// ==================== Auto-generated section ends ======================

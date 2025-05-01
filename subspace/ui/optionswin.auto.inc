// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
OptionsWin_ClassIf OptionsWin_ClassIf_tmpl = {
    ._size = sizeof(OptionsWin_ClassIf),
};

static OptionsWin_ClassIf _impl_OptionsWin_OptionsWin_ClassIf = {
    ._size = sizeof(OptionsWin_ClassIf),
    ._implements = (ObjIface*)&OptionsWin_ClassIf_tmpl,
    .make = (bool (*)(void*))OptionsWin_make,
    .show = (void (*)(void*))OptionsWin_show,
    .update = (void (*)(void*))OptionsWin_update,
    .finish = (void (*)(void*))OptionsWin_finish,
};

static ObjIface* _ifimpl_OptionsWin[] = {
    (ObjIface*)&_impl_OptionsWin_OptionsWin_ClassIf,
    NULL
};

ObjClassInfo OptionsWin_clsinfo = {
    .instsize = sizeof(OptionsWin),
    .classif = (ObjIface*)&OptionsWin_ClassIf_tmpl,
    .init = (bool(*)(void*))OptionsWin_init,
    .ifimpl = _ifimpl_OptionsWin,
};

// ==================== Auto-generated section ends ======================

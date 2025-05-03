// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
OptionsPage_ClassIf OptionsPage_ClassIf_tmpl = {
    ._size = sizeof(OptionsPage_ClassIf),
};

static OptionsPage_ClassIf _impl_OptionsPage_OptionsPage_ClassIf = {
    ._size = sizeof(OptionsPage_ClassIf),
    ._implements = (ObjIface*)&OptionsPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))OptionsPage_make,
    .update = (bool (*)(void*))OptionsPage_update,
};

static ObjIface* _ifimpl_OptionsPage[] = {
    (ObjIface*)&_impl_OptionsPage_OptionsPage_ClassIf,
    NULL
};

ObjClassInfo OptionsPage_clsinfo = {
    .instsize = sizeof(OptionsPage),
    .classif = (ObjIface*)&OptionsPage_ClassIf_tmpl,
    .destroy = (void(*)(void*))OptionsPage_destroy,
    .ifimpl = _ifimpl_OptionsPage,
};

// ==================== Auto-generated section ends ======================

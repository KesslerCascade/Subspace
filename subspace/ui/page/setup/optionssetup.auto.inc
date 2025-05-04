// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SetupPage_ClassIf SetupPage_ClassIf_tmpl = {
    ._size = sizeof(SetupPage_ClassIf),
    ._parent = (ObjIface*)&OptionsPage_ClassIf_tmpl,
};

static SetupPage_ClassIf _impl_SetupPage_SetupPage_ClassIf = {
    ._size = sizeof(SetupPage_ClassIf),
    ._implements = (ObjIface*)&SetupPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))SetupPage_make,
    .update = (bool (*)(void*))SetupPage_update,
};

static ObjIface* _ifimpl_SetupPage[] = {
    (ObjIface*)&_impl_SetupPage_SetupPage_ClassIf,
    NULL
};

ObjClassInfo SetupPage_clsinfo = {
    .instsize = sizeof(SetupPage),
    .classif = (ObjIface*)&SetupPage_ClassIf_tmpl,
    .parent = &OptionsPage_clsinfo,
    .init = (bool(*)(void*))SetupPage_init,
    .destroy = (void(*)(void*))SetupPage_destroy,
    .ifimpl = _ifimpl_SetupPage,
};

// ==================== Auto-generated section ends ======================

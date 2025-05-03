// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
GeneralPage_ClassIf GeneralPage_ClassIf_tmpl = {
    ._size = sizeof(GeneralPage_ClassIf),
    ._parent = (ObjIface*)&OptionsPage_ClassIf_tmpl,
};

static GeneralPage_ClassIf _impl_GeneralPage_GeneralPage_ClassIf = {
    ._size = sizeof(GeneralPage_ClassIf),
    ._implements = (ObjIface*)&GeneralPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))GeneralPage_make,
    .update = (bool (*)(void*))GeneralPage_update,
};

static ObjIface* _ifimpl_GeneralPage[] = {
    (ObjIface*)&_impl_GeneralPage_GeneralPage_ClassIf,
    NULL
};

ObjClassInfo GeneralPage_clsinfo = {
    .instsize = sizeof(GeneralPage),
    .classif = (ObjIface*)&GeneralPage_ClassIf_tmpl,
    .parent = &OptionsPage_clsinfo,
    .init = (bool(*)(void*))GeneralPage_init,
    .destroy = (void(*)(void*))GeneralPage_destroy,
    .ifimpl = _ifimpl_GeneralPage,
};

// ==================== Auto-generated section ends ======================

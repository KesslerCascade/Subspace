// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
InfoBlockPage_ClassIf InfoBlockPage_ClassIf_tmpl = {
    ._size = sizeof(InfoBlockPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static InfoBlockPage_ClassIf _impl_InfoBlockPage_InfoBlockPage_ClassIf = {
    ._size = sizeof(InfoBlockPage_ClassIf),
    ._implements = (ObjIface*)&InfoBlockPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))InfoBlockPage_make,
    .update = (bool (*)(void*))InfoBlockPage_update,
};

static ObjIface* _ifimpl_InfoBlockPage[] = {
    (ObjIface*)&_impl_InfoBlockPage_InfoBlockPage_ClassIf,
    NULL
};

ObjClassInfo InfoBlockPage_clsinfo = {
    .instsize = sizeof(InfoBlockPage),
    .classif = (ObjIface*)&InfoBlockPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .ifimpl = _ifimpl_InfoBlockPage,
};

// ==================== Auto-generated section ends ======================

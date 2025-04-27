// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ImageLoad_ClassIf ImageLoad_ClassIf_tmpl = {
    ._size = sizeof(ImageLoad_ClassIf),
    ._parent = (ObjIface*)&MultiphaseTask_ClassIf_tmpl,
};

static ImageLoad_ClassIf _impl_ImageLoad_ImageLoad_ClassIf = {
    ._size = sizeof(ImageLoad_ClassIf),
    ._implements = (ObjIface*)&ImageLoad_ClassIf_tmpl,
};

static ObjIface* _ifimpl_ImageLoad[] = {
    (ObjIface*)&_impl_ImageLoad_ImageLoad_ClassIf,
    NULL
};

ObjClassInfo ImageLoad_clsinfo = {
    .instsize = sizeof(ImageLoad),
    .classif = (ObjIface*)&ImageLoad_ClassIf_tmpl,
    .parent = &MultiphaseTask_clsinfo,
    .init = (bool(*)(void*))ImageLoad_init,
    .destroy = (void(*)(void*))ImageLoad_destroy,
    .ifimpl = _ifimpl_ImageLoad,
};

// ==================== Auto-generated section ends ======================

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
UIStrCache_ClassIf UIStrCache_ClassIf_tmpl = {
    ._size = sizeof(UIStrCache_ClassIf),
};

static UIStrCache_ClassIf _impl_UIStrCache_UIStrCache_ClassIf = {
    ._size = sizeof(UIStrCache_ClassIf),
    ._implements = (ObjIface*)&UIStrCache_ClassIf_tmpl,
    .add = (strref (*)(void*, strref))UIStrCache_add,
    .cadd = (const_char* (*)(void*, strref))UIStrCache_cadd,
    .trans = (strref (*)(void*, strref))UIStrCache_trans,
    .ctrans = (const_char* (*)(void*, strref))UIStrCache_ctrans,
};

static ObjIface* _ifimpl_UIStrCache[] = {
    (ObjIface*)&_impl_UIStrCache_UIStrCache_ClassIf,
    NULL
};

ObjClassInfo UIStrCache_clsinfo = {
    .instsize = sizeof(UIStrCache),
    .classif = (ObjIface*)&UIStrCache_ClassIf_tmpl,
    .init = (bool(*)(void*))UIStrCache_init,
    .destroy = (void(*)(void*))UIStrCache_destroy,
    .ifimpl = _ifimpl_UIStrCache,
};

// ==================== Auto-generated section ends ======================

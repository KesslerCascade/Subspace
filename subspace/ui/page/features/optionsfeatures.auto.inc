// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
FeaturesPage_ClassIf FeaturesPage_ClassIf_tmpl = {
    ._size = sizeof(FeaturesPage_ClassIf),
    ._parent = (ObjIface*)&OptionsPage_ClassIf_tmpl,
};

static FeaturesPage_ClassIf _impl_FeaturesPage_FeaturesPage_ClassIf = {
    ._size = sizeof(FeaturesPage_ClassIf),
    ._implements = (ObjIface*)&FeaturesPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))FeaturesPage_make,
};

static ObjIface* _ifimpl_FeaturesPage[] = {
    (ObjIface*)&_impl_FeaturesPage_FeaturesPage_ClassIf,
    NULL
};

ObjClassInfo FeaturesPage_clsinfo = {
    .instsize = sizeof(FeaturesPage),
    .classif = (ObjIface*)&FeaturesPage_ClassIf_tmpl,
    .parent = &OptionsPage_clsinfo,
    .ifimpl = _ifimpl_FeaturesPage,
};

// ==================== Auto-generated section ends ======================

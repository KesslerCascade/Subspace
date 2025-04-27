// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Image_ClassIf Image_ClassIf_tmpl = {
    ._size = sizeof(Image_ClassIf),
};

static Image_ClassIf _impl_Image_Image_ClassIf = {
    ._size = sizeof(Image_ClassIf),
    ._implements = (ObjIface*)&Image_ClassIf_tmpl,
    .slice = (Image* (*)(void*, int, int, int, int))Image_slice,
    .iupImage = (Ihandle* (*)(void*))Image_iupImage,
    .resample = (bool (*)(void*, int, int))Image_resample,
    .resampleToDPI = (bool (*)(void*, int))Image_resampleToDPI,
    .clear = (void (*)(void*))Image_clear,
};

static ObjIface* _ifimpl_Image[] = {
    (ObjIface*)&_impl_Image_Image_ClassIf,
    NULL
};

ObjClassInfo Image_clsinfo = {
    .instsize = sizeof(Image),
    .classif = (ObjIface*)&Image_ClassIf_tmpl,
    .init = (bool(*)(void*))Image_init,
    .destroy = (void(*)(void*))Image_destroy,
    .ifimpl = _ifimpl_Image,
};

// ==================== Auto-generated section ends ======================

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ImageResample_ClassIf ImageResample_ClassIf_tmpl = {
    ._size = sizeof(ImageResample_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static ImageResample_ClassIf _impl_ImageResample_ImageResample_ClassIf = {
    ._size = sizeof(ImageResample_ClassIf),
    ._implements = (ObjIface*)&ImageResample_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))ImageResample_run,
};

static ObjIface* _ifimpl_ImageResample[] = {
    (ObjIface*)&_impl_ImageResample_ImageResample_ClassIf,
    NULL
};

ObjClassInfo ImageResample_clsinfo = {
    .instsize = sizeof(ImageResample),
    .classif = (ObjIface*)&ImageResample_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .init = (bool(*)(void*))ImageResample_init,
    .destroy = (void(*)(void*))ImageResample_destroy,
    .ifimpl = _ifimpl_ImageResample,
};

// ==================== Auto-generated section ends ======================

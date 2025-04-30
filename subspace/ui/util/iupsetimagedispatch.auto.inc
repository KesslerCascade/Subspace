// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
IupSetImageDispatch_ClassIf IupSetImageDispatch_ClassIf_tmpl = {
    ._size = sizeof(IupSetImageDispatch_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static IupSetImageDispatch_ClassIf _impl_IupSetImageDispatch_IupSetImageDispatch_ClassIf = {
    ._size = sizeof(IupSetImageDispatch_ClassIf),
    ._implements = (ObjIface*)&IupSetImageDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))IupSetImageDispatch_run,
};

static ObjIface* _ifimpl_IupSetImageDispatch[] = {
    (ObjIface*)&_impl_IupSetImageDispatch_IupSetImageDispatch_ClassIf,
    NULL
};

ObjClassInfo IupSetImageDispatch_clsinfo = {
    .instsize = sizeof(IupSetImageDispatch),
    .classif = (ObjIface*)&IupSetImageDispatch_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .destroy = (void(*)(void*))IupSetImageDispatch_destroy,
    .ifimpl = _ifimpl_IupSetImageDispatch,
};

// ==================== Auto-generated section ends ======================

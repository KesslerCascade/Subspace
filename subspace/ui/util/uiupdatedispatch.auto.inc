// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
UIUpdateDispatch_ClassIf UIUpdateDispatch_ClassIf_tmpl = {
    ._size = sizeof(UIUpdateDispatch_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static UIUpdateDispatch_ClassIf _impl_UIUpdateDispatch_UIUpdateDispatch_ClassIf = {
    ._size = sizeof(UIUpdateDispatch_ClassIf),
    ._implements = (ObjIface*)&UIUpdateDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))UIUpdateDispatch_run,
};

static ObjIface* _ifimpl_UIUpdateDispatch[] = {
    (ObjIface*)&_impl_UIUpdateDispatch_UIUpdateDispatch_ClassIf,
    NULL
};

ObjClassInfo UIUpdateDispatch_clsinfo = {
    .instsize = sizeof(UIUpdateDispatch),
    .classif = (ObjIface*)&UIUpdateDispatch_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .destroy = (void(*)(void*))UIUpdateDispatch_destroy,
    .ifimpl = _ifimpl_UIUpdateDispatch,
};

// ==================== Auto-generated section ends ======================

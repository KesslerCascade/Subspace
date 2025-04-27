// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
IupImageAttachDispatch_ClassIf IupImageAttachDispatch_ClassIf_tmpl = {
    ._size = sizeof(IupImageAttachDispatch_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static IupImageAttachDispatch_ClassIf _impl_IupImageAttachDispatch_IupImageAttachDispatch_ClassIf = {
    ._size = sizeof(IupImageAttachDispatch_ClassIf),
    ._implements = (ObjIface*)&IupImageAttachDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))IupImageAttachDispatch_run,
};

static ObjIface* _ifimpl_IupImageAttachDispatch[] = {
    (ObjIface*)&_impl_IupImageAttachDispatch_IupImageAttachDispatch_ClassIf,
    NULL
};

ObjClassInfo IupImageAttachDispatch_clsinfo = {
    .instsize = sizeof(IupImageAttachDispatch),
    .classif = (ObjIface*)&IupImageAttachDispatch_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .destroy = (void(*)(void*))IupImageAttachDispatch_destroy,
    .ifimpl = _ifimpl_IupImageAttachDispatch,
};

// ==================== Auto-generated section ends ======================

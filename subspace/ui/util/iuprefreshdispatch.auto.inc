// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
IupRefreshDispatch_ClassIf IupRefreshDispatch_ClassIf_tmpl = {
    ._size = sizeof(IupRefreshDispatch_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static IupRefreshDispatch_ClassIf _impl_IupRefreshDispatch_IupRefreshDispatch_ClassIf = {
    ._size = sizeof(IupRefreshDispatch_ClassIf),
    ._implements = (ObjIface*)&IupRefreshDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))IupRefreshDispatch_run,
};

static ObjIface* _ifimpl_IupRefreshDispatch[] = {
    (ObjIface*)&_impl_IupRefreshDispatch_IupRefreshDispatch_ClassIf,
    NULL
};

ObjClassInfo IupRefreshDispatch_clsinfo = {
    .instsize = sizeof(IupRefreshDispatch),
    .classif = (ObjIface*)&IupRefreshDispatch_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_IupRefreshDispatch,
};

// ==================== Auto-generated section ends ======================

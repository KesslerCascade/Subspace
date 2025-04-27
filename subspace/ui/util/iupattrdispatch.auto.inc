// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
IupAttrDispatch_ClassIf IupAttrDispatch_ClassIf_tmpl = {
    ._size = sizeof(IupAttrDispatch_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static IupAttrDispatch_ClassIf _impl_IupAttrDispatch_IupAttrDispatch_ClassIf = {
    ._size = sizeof(IupAttrDispatch_ClassIf),
    ._implements = (ObjIface*)&IupAttrDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))IupAttrDispatch_run,
};

static ObjIface* _ifimpl_IupAttrDispatch[] = {
    (ObjIface*)&_impl_IupAttrDispatch_IupAttrDispatch_ClassIf,
    NULL
};

ObjClassInfo IupAttrDispatch_clsinfo = {
    .instsize = sizeof(IupAttrDispatch),
    .classif = (ObjIface*)&IupAttrDispatch_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .destroy = (void(*)(void*))IupAttrDispatch_destroy,
    .ifimpl = _ifimpl_IupAttrDispatch,
};

// ==================== Auto-generated section ends ======================

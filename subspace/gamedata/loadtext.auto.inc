// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LoadText_ClassIf LoadText_ClassIf_tmpl = {
    ._size = sizeof(LoadText_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static LoadText_ClassIf _impl_LoadText_LoadText_ClassIf = {
    ._size = sizeof(LoadText_ClassIf),
    ._implements = (ObjIface*)&LoadText_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LoadText_run,
};

static ObjIface* _ifimpl_LoadText[] = {
    (ObjIface*)&_impl_LoadText_LoadText_ClassIf,
    NULL
};

ObjClassInfo LoadText_clsinfo = {
    .instsize = sizeof(LoadText),
    .classif = (ObjIface*)&LoadText_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))LoadText_destroy,
    .ifimpl = _ifimpl_LoadText,
};

// ==================== Auto-generated section ends ======================

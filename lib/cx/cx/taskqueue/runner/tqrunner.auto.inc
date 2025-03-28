// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQRunner_ClassIf TQRunner_ClassIf_tmpl = {
    ._size = sizeof(TQRunner_ClassIf),
};

static TQRunner_ClassIf _impl_TQRunner_TQRunner_ClassIf = {
    ._size = sizeof(TQRunner_ClassIf),
    ._implements = (ObjIface*)&TQRunner_ClassIf_tmpl,
    .start = (bool (*)(void*, TaskQueue*))TQRunner_start,
    .stop = (bool (*)(void*))TQRunner_stop,
};

static ObjIface* _ifimpl_TQRunner[] = {
    (ObjIface*)&_impl_TQRunner_TQRunner_ClassIf,
    NULL
};

ObjClassInfo TQRunner_clsinfo = {
    .instsize = sizeof(TQRunner),
    .classif = (ObjIface*)&TQRunner_ClassIf_tmpl,
    .destroy = (void(*)(void*))TQRunner_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_TQRunner,
};

// ==================== Auto-generated section ends ======================

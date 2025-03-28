// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQManualRunner_ClassIf TQManualRunner_ClassIf_tmpl = {
    ._size = sizeof(TQManualRunner_ClassIf),
    ._parent = (ObjIface*)&TQRunner_ClassIf_tmpl,
};

static TQManualRunner_ClassIf _impl_TQManualRunner_TQManualRunner_ClassIf = {
    ._size = sizeof(TQManualRunner_ClassIf),
    ._implements = (ObjIface*)&TQManualRunner_ClassIf_tmpl,
    .start = (bool (*)(void*, TaskQueue*))TQManualRunner_start,
    .stop = (bool (*)(void*))TQManualRunner_stop,
    .tick = (int64 (*)(void*))TQManualRunner_tick,
};

static ObjIface* _ifimpl_TQManualRunner[] = {
    (ObjIface*)&_impl_TQManualRunner_TQManualRunner_ClassIf,
    NULL
};

ObjClassInfo TQManualRunner_clsinfo = {
    .instsize = sizeof(TQManualRunner),
    .classif = (ObjIface*)&TQManualRunner_ClassIf_tmpl,
    .parent = &TQRunner_clsinfo,
    .destroy = (void(*)(void*))TQManualRunner_destroy,
    .ifimpl = _ifimpl_TQManualRunner,
};

// ==================== Auto-generated section ends ======================

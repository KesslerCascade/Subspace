// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQManualWorker_ClassIf TQManualWorker_ClassIf_tmpl = {
    ._size = sizeof(TQManualWorker_ClassIf),
    ._parent = (ObjIface*)&TQWorker_ClassIf_tmpl,
};

static TQManualWorker_ClassIf _impl_TQManualWorker_TQManualWorker_ClassIf = {
    ._size = sizeof(TQManualWorker_ClassIf),
    ._implements = (ObjIface*)&TQManualWorker_ClassIf_tmpl,
    .tick = (int64 (*)(void*, TaskQueue*))TQManualWorker_tick,
};

static ObjIface* _ifimpl_TQManualWorker[] = {
    (ObjIface*)&_impl_TQManualWorker_TQManualWorker_ClassIf,
    NULL
};

ObjClassInfo TQManualWorker_clsinfo = {
    .instsize = sizeof(TQManualWorker),
    .classif = (ObjIface*)&TQManualWorker_ClassIf_tmpl,
    .parent = &TQWorker_clsinfo,
    .ifimpl = _ifimpl_TQManualWorker,
};

// ==================== Auto-generated section ends ======================

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQWorker_ClassIf TQWorker_ClassIf_tmpl = {
    ._size = sizeof(TQWorker_ClassIf),
};

static TQWorker_ClassIf _impl_TQWorker_TQWorker_ClassIf = {
    ._size = sizeof(TQWorker_ClassIf),
    ._implements = (ObjIface*)&TQWorker_ClassIf_tmpl,
    .onStart = (void (*)(void*, TaskQueue*))TQWorker_onStart,
    .onStop = (void (*)(void*, TaskQueue*))TQWorker_onStop,
};

static ObjIface* _ifimpl_TQWorker[] = {
    (ObjIface*)&_impl_TQWorker_TQWorker_ClassIf,
    NULL
};

ObjClassInfo TQWorker_clsinfo = {
    .instsize = sizeof(TQWorker),
    .classif = (ObjIface*)&TQWorker_ClassIf_tmpl,
    ._abstract = true,
    .ifimpl = _ifimpl_TQWorker,
};

// ==================== Auto-generated section ends ======================

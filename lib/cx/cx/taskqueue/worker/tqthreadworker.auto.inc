// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQThreadWorker_ClassIf TQThreadWorker_ClassIf_tmpl = {
    ._size = sizeof(TQThreadWorker_ClassIf),
    ._parent = (ObjIface*)&TQWorker_ClassIf_tmpl,
};

static TQThreadWorker_ClassIf _impl_TQThreadWorker_TQThreadWorker_ClassIf = {
    ._size = sizeof(TQThreadWorker_ClassIf),
    ._implements = (ObjIface*)&TQThreadWorker_ClassIf_tmpl,
    .startThread = (bool (*)(void*, TaskQueue*))TQThreadWorker_startThread,
    .tick = (int64 (*)(void*, TaskQueue*))TQThreadWorker_tick,
};

static ObjIface* _ifimpl_TQThreadWorker[] = {
    (ObjIface*)&_impl_TQThreadWorker_TQThreadWorker_ClassIf,
    NULL
};

ObjClassInfo TQThreadWorker_clsinfo = {
    .instsize = sizeof(TQThreadWorker),
    .classif = (ObjIface*)&TQThreadWorker_ClassIf_tmpl,
    .parent = &TQWorker_clsinfo,
    .destroy = (void(*)(void*))TQThreadWorker_destroy,
    .ifimpl = _ifimpl_TQThreadWorker,
};

// ==================== Auto-generated section ends ======================

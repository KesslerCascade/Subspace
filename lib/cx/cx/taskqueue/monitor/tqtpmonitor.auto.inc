// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQThreadPoolMonitor_ClassIf TQThreadPoolMonitor_ClassIf_tmpl = {
    ._size = sizeof(TQThreadPoolMonitor_ClassIf),
    ._parent = (ObjIface*)&TQMonitor_ClassIf_tmpl,
};

static TQThreadPoolMonitor_ClassIf _impl_TQThreadPoolMonitor_TQThreadPoolMonitor_ClassIf = {
    ._size = sizeof(TQThreadPoolMonitor_ClassIf),
    ._implements = (ObjIface*)&TQThreadPoolMonitor_ClassIf_tmpl,
    .start = (bool (*)(void*, TaskQueue*))TQThreadPoolMonitor_start,
    .tick = (int64 (*)(void*))TQThreadPoolMonitor_tick,
    .stop = (bool (*)(void*))TQThreadPoolMonitor_stop,
};

static ObjIface* _ifimpl_TQThreadPoolMonitor[] = {
    (ObjIface*)&_impl_TQThreadPoolMonitor_TQThreadPoolMonitor_ClassIf,
    NULL
};

ObjClassInfo TQThreadPoolMonitor_clsinfo = {
    .instsize = sizeof(TQThreadPoolMonitor),
    .classif = (ObjIface*)&TQThreadPoolMonitor_ClassIf_tmpl,
    .parent = &TQMonitor_clsinfo,
    .init = (bool(*)(void*))TQThreadPoolMonitor_init,
    .destroy = (void(*)(void*))TQThreadPoolMonitor_destroy,
    .ifimpl = _ifimpl_TQThreadPoolMonitor,
};

// ==================== Auto-generated section ends ======================

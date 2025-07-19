// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
HullTracker_ClassIf HullTracker_ClassIf_tmpl = {
    ._size = sizeof(HullTracker_ClassIf),
};

static LogSubscriber _impl_HullTracker_LogSubscriber = {
    ._size = sizeof(LogSubscriber),
    ._implements = (ObjIface*)&LogSubscriber_tmpl,
    .logNotify = (void (*)(void*, LogEnt*, bool))HullTracker_logNotify,
    .logReset = (void (*)(void*))HullTracker_logReset,
    .logReplayComplete = (void (*)(void*))HullTracker_logReplayComplete,
};

static HullTracker_ClassIf _impl_HullTracker_HullTracker_ClassIf = {
    ._size = sizeof(HullTracker_ClassIf),
    ._implements = (ObjIface*)&HullTracker_ClassIf_tmpl,
    .getTotal = (bool (*)(void*, HullTotals*))HullTracker_getTotal,
    .getBeacon = (bool (*)(void*, int64, HullTotals*))HullTracker_getBeacon,
    .getSector = (bool (*)(void*, int64, HullTotals*))HullTracker_getSector,
    .reset = (void (*)(void*))HullTracker_reset,
    .logNotify = (void (*)(void*, LogEnt*, bool))HullTracker_logNotify,
    .logReset = (void (*)(void*))HullTracker_logReset,
    .logReplayComplete = (void (*)(void*))HullTracker_logReplayComplete,
};

static ObjIface* _ifimpl_HullTracker[] = {
    (ObjIface*)&_impl_HullTracker_LogSubscriber,
    (ObjIface*)&_impl_HullTracker_HullTracker_ClassIf,
    NULL
};

ObjClassInfo HullTracker_clsinfo = {
    .instsize = sizeof(HullTracker),
    .classif = (ObjIface*)&HullTracker_ClassIf_tmpl,
    .init = (bool(*)(void*))HullTracker_init,
    .destroy = (void(*)(void*))HullTracker_destroy,
    .ifimpl = _ifimpl_HullTracker,
};

// ==================== Auto-generated section ends ======================

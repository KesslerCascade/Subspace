// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ScrapTracker_ClassIf ScrapTracker_ClassIf_tmpl = {
    ._size = sizeof(ScrapTracker_ClassIf),
};

static LogSubscriber _impl_ScrapTracker_LogSubscriber = {
    ._size = sizeof(LogSubscriber),
    ._implements = (ObjIface*)&LogSubscriber_tmpl,
    .logNotify = (void (*)(void*, LogEnt*, bool))ScrapTracker_logNotify,
    .logReset = (void (*)(void*))ScrapTracker_logReset,
    .logReplayComplete = (void (*)(void*))ScrapTracker_logReplayComplete,
};

static ScrapTracker_ClassIf _impl_ScrapTracker_ScrapTracker_ClassIf = {
    ._size = sizeof(ScrapTracker_ClassIf),
    ._implements = (ObjIface*)&ScrapTracker_ClassIf_tmpl,
    .getTotal = (bool (*)(void*, ScrapTotals*))ScrapTracker_getTotal,
    .getBeacon = (bool (*)(void*, int64, ScrapTotals*))ScrapTracker_getBeacon,
    .getSector = (bool (*)(void*, int64, ScrapTotals*))ScrapTracker_getSector,
    .reset = (void (*)(void*))ScrapTracker_reset,
    .logNotify = (void (*)(void*, LogEnt*, bool))ScrapTracker_logNotify,
    .logReset = (void (*)(void*))ScrapTracker_logReset,
    .logReplayComplete = (void (*)(void*))ScrapTracker_logReplayComplete,
};

static ObjIface* _ifimpl_ScrapTracker[] = {
    (ObjIface*)&_impl_ScrapTracker_LogSubscriber,
    (ObjIface*)&_impl_ScrapTracker_ScrapTracker_ClassIf,
    NULL
};

ObjClassInfo ScrapTracker_clsinfo = {
    .instsize = sizeof(ScrapTracker),
    .classif = (ObjIface*)&ScrapTracker_ClassIf_tmpl,
    .init = (bool(*)(void*))ScrapTracker_init,
    .destroy = (void(*)(void*))ScrapTracker_destroy,
    .ifimpl = _ifimpl_ScrapTracker,
};

// ==================== Auto-generated section ends ======================

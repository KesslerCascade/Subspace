// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ScrapGraphPanel_ClassIf ScrapGraphPanel_ClassIf_tmpl = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static LogSubscriber _impl_ScrapGraphPanel_LogSubscriber = {
    ._size = sizeof(LogSubscriber),
    ._implements = (ObjIface*)&LogSubscriber_tmpl,
    .logNotify = (void (*)(void*, LogEnt*, bool))ScrapGraphPanel_logNotify,
    .logReset = (void (*)(void*))ScrapGraphPanel_logReset,
    .logReplayComplete = (void (*)(void*))ScrapGraphPanel_logReplayComplete,
};

static ScrapGraphPanel_ClassIf _impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._implements = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .clear = (void (*)(void*))ScrapGraphPanel_clear,
    .make = (bool (*)(void*))ScrapGraphPanel_make,
    .update = (bool (*)(void*))ScrapGraphPanel_update,
    .logNotify = (void (*)(void*, LogEnt*, bool))ScrapGraphPanel_logNotify,
    .logReset = (void (*)(void*))ScrapGraphPanel_logReset,
    .logReplayComplete = (void (*)(void*))ScrapGraphPanel_logReplayComplete,
};

static ObjIface* _ifimpl_ScrapGraphPanel[] = {
    (ObjIface*)&_impl_ScrapGraphPanel_LogSubscriber,
    (ObjIface*)&_impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf,
    NULL
};

ObjClassInfo ScrapGraphPanel_clsinfo = {
    .instsize = sizeof(ScrapGraphPanel),
    .classif = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .init = (bool(*)(void*))ScrapGraphPanel_init,
    .destroy = (void(*)(void*))ScrapGraphPanel_destroy,
    .ifimpl = _ifimpl_ScrapGraphPanel,
};

// ==================== Auto-generated section ends ======================

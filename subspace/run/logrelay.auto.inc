// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LogSubscriber LogSubscriber_tmpl = {
    ._size = sizeof(LogSubscriber),
};

LogRelay_ClassIf LogRelay_ClassIf_tmpl = {
    ._size = sizeof(LogRelay_ClassIf),
};

LogDispatcher_ClassIf LogDispatcher_ClassIf_tmpl = {
    ._size = sizeof(LogDispatcher_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static LogRelay_ClassIf _impl_LogRelay_LogRelay_ClassIf = {
    ._size = sizeof(LogRelay_ClassIf),
    ._implements = (ObjIface*)&LogRelay_ClassIf_tmpl,
    .subscribe = (bool (*)(void*, ObjInst*, strref))LogRelay_subscribe,
    .reset = (void (*)(void*))LogRelay_reset,
    .replayComplete = (void (*)(void*))LogRelay_replayComplete,
    .send = (bool (*)(void*, LogEnt*, bool))LogRelay_send,
};

static ObjIface* _ifimpl_LogRelay[] = {
    (ObjIface*)&_impl_LogRelay_LogRelay_ClassIf,
    NULL
};

ObjClassInfo LogRelay_clsinfo = {
    .instsize = sizeof(LogRelay),
    .classif = (ObjIface*)&LogRelay_ClassIf_tmpl,
    .init = (bool(*)(void*))LogRelay_init,
    .destroy = (void(*)(void*))LogRelay_destroy,
    .ifimpl = _ifimpl_LogRelay,
};

static LogDispatcher_ClassIf _impl_LogDispatcher_LogDispatcher_ClassIf = {
    ._size = sizeof(LogDispatcher_ClassIf),
    ._implements = (ObjIface*)&LogDispatcher_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LogDispatcher_run,
};

static ObjIface* _ifimpl_LogDispatcher[] = {
    (ObjIface*)&_impl_LogDispatcher_LogDispatcher_ClassIf,
    NULL
};

ObjClassInfo LogDispatcher_clsinfo = {
    .instsize = sizeof(LogDispatcher),
    .classif = (ObjIface*)&LogDispatcher_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))LogDispatcher_destroy,
    .ifimpl = _ifimpl_LogDispatcher,
};

// ==================== Auto-generated section ends ======================

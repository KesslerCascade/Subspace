// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LogSubscriber LogSubscriber_tmpl = {
    ._size = sizeof(LogSubscriber),
};

SubscriberInfo_ClassIf SubscriberInfo_ClassIf_tmpl = {
    ._size = sizeof(SubscriberInfo_ClassIf),
};

LogRelay_ClassIf LogRelay_ClassIf_tmpl = {
    ._size = sizeof(LogRelay_ClassIf),
};

LogDispatch_ClassIf LogDispatch_ClassIf_tmpl = {
    ._size = sizeof(LogDispatch_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static Sortable _impl_SubscriberInfo_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))SubscriberInfo_cmp,
};

static Hashable _impl_SubscriberInfo_Hashable = {
    ._size = sizeof(Hashable),
    ._implements = (ObjIface*)&Hashable_tmpl,
    .hash = (uint32 (*)(void*, uint32))SubscriberInfo_hash,
};

static SubscriberInfo_ClassIf _impl_SubscriberInfo_SubscriberInfo_ClassIf = {
    ._size = sizeof(SubscriberInfo_ClassIf),
    ._implements = (ObjIface*)&SubscriberInfo_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))SubscriberInfo_cmp,
    .hash = (uint32 (*)(void*, uint32))SubscriberInfo_hash,
};

static ObjIface* _ifimpl_SubscriberInfo[] = {
    (ObjIface*)&_impl_SubscriberInfo_Sortable,
    (ObjIface*)&_impl_SubscriberInfo_Hashable,
    (ObjIface*)&_impl_SubscriberInfo_SubscriberInfo_ClassIf,
    NULL
};

ObjClassInfo SubscriberInfo_clsinfo = {
    .instsize = sizeof(SubscriberInfo),
    .classif = (ObjIface*)&SubscriberInfo_ClassIf_tmpl,
    .destroy = (void(*)(void*))SubscriberInfo_destroy,
    .ifimpl = _ifimpl_SubscriberInfo,
};

static LogRelay_ClassIf _impl_LogRelay_LogRelay_ClassIf = {
    ._size = sizeof(LogRelay_ClassIf),
    ._implements = (ObjIface*)&LogRelay_ClassIf_tmpl,
    .subscribe = (bool (*)(void*, ObjInst*, strref))LogRelay_subscribe,
    .subscribeUI = (bool (*)(void*, ObjInst*, strref))LogRelay_subscribeUI,
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

static LogDispatch_ClassIf _impl_LogDispatch_LogDispatch_ClassIf = {
    ._size = sizeof(LogDispatch_ClassIf),
    ._implements = (ObjIface*)&LogDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LogDispatch_run,
};

static ObjIface* _ifimpl_LogDispatch[] = {
    (ObjIface*)&_impl_LogDispatch_LogDispatch_ClassIf,
    NULL
};

ObjClassInfo LogDispatch_clsinfo = {
    .instsize = sizeof(LogDispatch),
    .classif = (ObjIface*)&LogDispatch_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))LogDispatch_destroy,
    .ifimpl = _ifimpl_LogDispatch,
};

// ==================== Auto-generated section ends ======================

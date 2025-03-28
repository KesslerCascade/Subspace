// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TRMutex_ClassIf TRMutex_ClassIf_tmpl = {
    ._size = sizeof(TRMutex_ClassIf),
    ._parent = (ObjIface*)&TaskResource_ClassIf_tmpl,
};

static TRMutex_ClassIf _impl_TRMutex_TRMutex_ClassIf = {
    ._size = sizeof(TRMutex_ClassIf),
    ._implements = (ObjIface*)&TRMutex_ClassIf_tmpl,
    .wakeup = (void (*)(void*))TRMutex_wakeup,
    .registerTask = (bool (*)(void*, ComplexTask*))TRMutex_registerTask,
    .canAcquire = (bool (*)(void*, ComplexTask*))TRMutex_canAcquire,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TRMutex_tryAcquire,
    .release = (void (*)(void*, ComplexTask*))TRMutex_release,
};

static ObjIface* _ifimpl_TRMutex[] = {
    (ObjIface*)&_impl_TRMutex_TRMutex_ClassIf,
    NULL
};

ObjClassInfo TRMutex_clsinfo = {
    .instsize = sizeof(TRMutex),
    .classif = (ObjIface*)&TRMutex_ClassIf_tmpl,
    .parent = &TaskResource_clsinfo,
    .init = (bool(*)(void*))TRMutex_init,
    .destroy = (void(*)(void*))TRMutex_destroy,
    .ifimpl = _ifimpl_TRMutex,
};

// ==================== Auto-generated section ends ======================

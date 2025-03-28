// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TRFifo_ClassIf TRFifo_ClassIf_tmpl = {
    ._size = sizeof(TRFifo_ClassIf),
    ._parent = (ObjIface*)&TaskResource_ClassIf_tmpl,
};

static TRFifo_ClassIf _impl_TRFifo_TRFifo_ClassIf = {
    ._size = sizeof(TRFifo_ClassIf),
    ._implements = (ObjIface*)&TRFifo_ClassIf_tmpl,
    .registerTask = (bool (*)(void*, ComplexTask*))TRFifo_registerTask,
    .canAcquire = (bool (*)(void*, ComplexTask*))TRFifo_canAcquire,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TRFifo_tryAcquire,
    .release = (void (*)(void*, ComplexTask*))TRFifo_release,
};

static ObjIface* _ifimpl_TRFifo[] = {
    (ObjIface*)&_impl_TRFifo_TRFifo_ClassIf,
    NULL
};

ObjClassInfo TRFifo_clsinfo = {
    .instsize = sizeof(TRFifo),
    .classif = (ObjIface*)&TRFifo_ClassIf_tmpl,
    .parent = &TaskResource_clsinfo,
    .init = (bool(*)(void*))TRFifo_init,
    .destroy = (void(*)(void*))TRFifo_destroy,
    .ifimpl = _ifimpl_TRFifo,
};

// ==================== Auto-generated section ends ======================

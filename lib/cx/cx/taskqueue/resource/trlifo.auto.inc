// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TRLifo_ClassIf TRLifo_ClassIf_tmpl = {
    ._size = sizeof(TRLifo_ClassIf),
    ._parent = (ObjIface*)&TaskResource_ClassIf_tmpl,
};

static TRLifo_ClassIf _impl_TRLifo_TRLifo_ClassIf = {
    ._size = sizeof(TRLifo_ClassIf),
    ._implements = (ObjIface*)&TRLifo_ClassIf_tmpl,
    .registerTask = (bool (*)(void*, ComplexTask*))TRLifo_registerTask,
    .canAcquire = (bool (*)(void*, ComplexTask*))TRLifo_canAcquire,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TRLifo_tryAcquire,
    .release = (void (*)(void*, ComplexTask*))TRLifo_release,
};

static ObjIface* _ifimpl_TRLifo[] = {
    (ObjIface*)&_impl_TRLifo_TRLifo_ClassIf,
    NULL
};

ObjClassInfo TRLifo_clsinfo = {
    .instsize = sizeof(TRLifo),
    .classif = (ObjIface*)&TRLifo_ClassIf_tmpl,
    .parent = &TaskResource_clsinfo,
    .init = (bool(*)(void*))TRLifo_init,
    .destroy = (void(*)(void*))TRLifo_destroy,
    .ifimpl = _ifimpl_TRLifo,
};

// ==================== Auto-generated section ends ======================

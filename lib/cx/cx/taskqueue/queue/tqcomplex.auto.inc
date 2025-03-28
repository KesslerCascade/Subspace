// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ComplexTaskQueue_ClassIf ComplexTaskQueue_ClassIf_tmpl = {
    ._size = sizeof(ComplexTaskQueue_ClassIf),
    ._parent = (ObjIface*)&TaskQueue_ClassIf_tmpl,
};

static ComplexTaskQueue_ClassIf _impl_ComplexTaskQueue_ComplexTaskQueue_ClassIf = {
    ._size = sizeof(ComplexTaskQueue_ClassIf),
    ._implements = (ObjIface*)&ComplexTaskQueue_ClassIf_tmpl,
    .schedule = (bool (*)(void*, ComplexTask*, int64))ComplexTaskQueue_schedule,
    .defer = (bool (*)(void*, ComplexTask*))ComplexTaskQueue_defer,
    .advance = (bool (*)(void*, ComplexTask*))ComplexTaskQueue_advance,
    .add = (bool (*)(void*, BasicTask*))ComplexTaskQueue_add,
    ._processDone = (bool (*)(void*))ComplexTaskQueue__processDone,
    ._processExtra = (int64 (*)(void*, bool))ComplexTaskQueue__processExtra,
    ._queueMaint = (bool (*)(void*))ComplexTaskQueue__queueMaint,
    ._runTask = (bool (*)(void*, BasicTask**, TQWorker*))ComplexTaskQueue__runTask,
    ._clear = (void (*)(void*))ComplexTaskQueue__clear,
};

static ObjIface* _ifimpl_ComplexTaskQueue[] = {
    (ObjIface*)&_impl_ComplexTaskQueue_ComplexTaskQueue_ClassIf,
    NULL
};

ObjClassInfo ComplexTaskQueue_clsinfo = {
    .instsize = sizeof(ComplexTaskQueue),
    .classif = (ObjIface*)&ComplexTaskQueue_ClassIf_tmpl,
    .parent = &TaskQueue_clsinfo,
    .init = (bool(*)(void*))ComplexTaskQueue_init,
    .destroy = (void(*)(void*))ComplexTaskQueue_destroy,
    .ifimpl = _ifimpl_ComplexTaskQueue,
};

// ==================== Auto-generated section ends ======================

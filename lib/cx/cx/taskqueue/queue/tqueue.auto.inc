// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TaskQueue_ClassIf TaskQueue_ClassIf_tmpl = {
    ._size = sizeof(TaskQueue_ClassIf),
};

static TaskQueue_ClassIf _impl_TaskQueue_TaskQueue_ClassIf = {
    ._size = sizeof(TaskQueue_ClassIf),
    ._implements = (ObjIface*)&TaskQueue_ClassIf_tmpl,
    .start = (bool (*)(void*))TaskQueue_start,
    .stop = (bool (*)(void*, int64))TaskQueue_stop,
    .add = (bool (*)(void*, BasicTask*))TaskQueue_add,
    .tick = (int64 (*)(void*))TaskQueue_tick,
    ._processDone = (bool (*)(void*))TaskQueue__processDone,
    ._processExtra = (int64 (*)(void*, bool))TaskQueue__processExtra,
    ._queueMaint = (bool (*)(void*))TaskQueue__queueMaint,
    ._runTask = (bool (*)(void*, BasicTask**, TQWorker*))TaskQueue__runTask,
    ._clear = (void (*)(void*))TaskQueue__clear,
};

static ObjIface* _ifimpl_TaskQueue[] = {
    (ObjIface*)&_impl_TaskQueue_TaskQueue_ClassIf,
    NULL
};

ObjClassInfo TaskQueue_clsinfo = {
    .instsize = sizeof(TaskQueue),
    .classif = (ObjIface*)&TaskQueue_ClassIf_tmpl,
    .init = (bool(*)(void*))TaskQueue_init,
    .destroy = (void(*)(void*))TaskQueue_destroy,
    .ifimpl = _ifimpl_TaskQueue,
};

// ==================== Auto-generated section ends ======================

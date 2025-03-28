// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Task_ClassIf Task_ClassIf_tmpl = {
    ._size = sizeof(Task_ClassIf),
    ._parent = (ObjIface*)&BasicTask_ClassIf_tmpl,
};

static Task_ClassIf _impl_Task_Task_ClassIf = {
    ._size = sizeof(Task_ClassIf),
    ._implements = (ObjIface*)&Task_ClassIf_tmpl,
    .wait = (bool (*)(void*, int64))Task_wait,
    .reset = (bool (*)(void*))Task_reset,
};

static ObjIface* _ifimpl_Task[] = {
    (ObjIface*)&_impl_Task_Task_ClassIf,
    NULL
};

ObjClassInfo Task_clsinfo = {
    .instsize = sizeof(Task),
    .classif = (ObjIface*)&Task_ClassIf_tmpl,
    .parent = &BasicTask_clsinfo,
    .init = (bool(*)(void*))Task_init,
    .destroy = (void(*)(void*))Task_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_Task,
};

// ==================== Auto-generated section ends ======================

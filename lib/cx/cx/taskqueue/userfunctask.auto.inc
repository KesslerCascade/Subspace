// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
UserFuncTask_ClassIf UserFuncTask_ClassIf_tmpl = {
    ._size = sizeof(UserFuncTask_ClassIf),
    ._parent = (ObjIface*)&BasicTask_ClassIf_tmpl,
};

static UserFuncTask_ClassIf _impl_UserFuncTask_UserFuncTask_ClassIf = {
    ._size = sizeof(UserFuncTask_ClassIf),
    ._implements = (ObjIface*)&UserFuncTask_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))UserFuncTask_run,
};

static ObjIface* _ifimpl_UserFuncTask[] = {
    (ObjIface*)&_impl_UserFuncTask_UserFuncTask_ClassIf,
    NULL
};

ObjClassInfo UserFuncTask_clsinfo = {
    .instsize = sizeof(UserFuncTask),
    .classif = (ObjIface*)&UserFuncTask_ClassIf_tmpl,
    .parent = &BasicTask_clsinfo,
    .ifimpl = _ifimpl_UserFuncTask,
};

// ==================== Auto-generated section ends ======================

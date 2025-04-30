// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdValidate_ClassIf CmdValidate_ClassIf_tmpl = {
    ._size = sizeof(CmdValidate_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdValidate_ClassIf _impl_CmdValidate_CmdValidate_ClassIf = {
    ._size = sizeof(CmdValidate_ClassIf),
    ._implements = (ObjIface*)&CmdValidate_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdValidate_run,
};

static ObjIface* _ifimpl_CmdValidate[] = {
    (ObjIface*)&_impl_CmdValidate_CmdValidate_ClassIf,
    NULL
};

ObjClassInfo CmdValidate_clsinfo = {
    .instsize = sizeof(CmdValidate),
    .classif = (ObjIface*)&CmdValidate_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdValidate,
};

// ==================== Auto-generated section ends ======================

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdLoading_ClassIf CmdLoading_ClassIf_tmpl = {
    ._size = sizeof(CmdLoading_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdLoading_ClassIf _impl_CmdLoading_CmdLoading_ClassIf = {
    ._size = sizeof(CmdLoading_ClassIf),
    ._implements = (ObjIface*)&CmdLoading_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdLoading_run,
};

static ObjIface* _ifimpl_CmdLoading[] = {
    (ObjIface*)&_impl_CmdLoading_CmdLoading_ClassIf,
    NULL
};

ObjClassInfo CmdLoading_clsinfo = {
    .instsize = sizeof(CmdLoading),
    .classif = (ObjIface*)&CmdLoading_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdLoading,
};

// ==================== Auto-generated section ends ======================

// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdLaunchFail_ClassIf CmdLaunchFail_ClassIf_tmpl = {
    ._size = sizeof(CmdLaunchFail_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdLaunchFail_ClassIf _impl_CmdLaunchFail_CmdLaunchFail_ClassIf = {
    ._size = sizeof(CmdLaunchFail_ClassIf),
    ._implements = (ObjIface*)&CmdLaunchFail_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdLaunchFail_run,
};

static ObjIface* _ifimpl_CmdLaunchFail[] = {
    (ObjIface*)&_impl_CmdLaunchFail_CmdLaunchFail_ClassIf,
    NULL
};

ObjClassInfo CmdLaunchFail_clsinfo = {
    .instsize = sizeof(CmdLaunchFail),
    .classif = (ObjIface*)&CmdLaunchFail_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdLaunchFail,
};

// ==================== Auto-generated section ends ======================

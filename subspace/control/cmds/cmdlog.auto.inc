// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdLog_ClassIf CmdLog_ClassIf_tmpl = {
    ._size = sizeof(CmdLog_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdLog_ClassIf _impl_CmdLog_CmdLog_ClassIf = {
    ._size = sizeof(CmdLog_ClassIf),
    ._implements = (ObjIface*)&CmdLog_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdLog_run,
};

static ObjIface* _ifimpl_CmdLog[] = {
    (ObjIface*)&_impl_CmdLog_CmdLog_ClassIf,
    NULL
};

ObjClassInfo CmdLog_clsinfo = {
    .instsize = sizeof(CmdLog),
    .classif = (ObjIface*)&CmdLog_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdLog,
};

// ==================== Auto-generated section ends ======================

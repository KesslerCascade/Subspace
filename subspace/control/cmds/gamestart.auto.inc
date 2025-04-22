// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdGameStart_ClassIf CmdGameStart_ClassIf_tmpl = {
    ._size = sizeof(CmdGameStart_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdGameStart_ClassIf _impl_CmdGameStart_CmdGameStart_ClassIf = {
    ._size = sizeof(CmdGameStart_ClassIf),
    ._implements = (ObjIface*)&CmdGameStart_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdGameStart_run,
};

static ObjIface* _ifimpl_CmdGameStart[] = {
    (ObjIface*)&_impl_CmdGameStart_CmdGameStart_ClassIf,
    NULL
};

ObjClassInfo CmdGameStart_clsinfo = {
    .instsize = sizeof(CmdGameStart),
    .classif = (ObjIface*)&CmdGameStart_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdGameStart,
};

// ==================== Auto-generated section ends ======================

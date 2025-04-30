// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdGameState_ClassIf CmdGameState_ClassIf_tmpl = {
    ._size = sizeof(CmdGameState_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdGameState_ClassIf _impl_CmdGameState_CmdGameState_ClassIf = {
    ._size = sizeof(CmdGameState_ClassIf),
    ._implements = (ObjIface*)&CmdGameState_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdGameState_run,
};

static ObjIface* _ifimpl_CmdGameState[] = {
    (ObjIface*)&_impl_CmdGameState_CmdGameState_ClassIf,
    NULL
};

ObjClassInfo CmdGameState_clsinfo = {
    .instsize = sizeof(CmdGameState),
    .classif = (ObjIface*)&CmdGameState_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdGameState,
};

// ==================== Auto-generated section ends ======================

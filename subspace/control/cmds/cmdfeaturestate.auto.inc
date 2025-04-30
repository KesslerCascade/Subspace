// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
CmdFeatureState_ClassIf CmdFeatureState_ClassIf_tmpl = {
    ._size = sizeof(CmdFeatureState_ClassIf),
    ._parent = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static CmdFeatureState_ClassIf _impl_CmdFeatureState_CmdFeatureState_ClassIf = {
    ._size = sizeof(CmdFeatureState_ClassIf),
    ._implements = (ObjIface*)&CmdFeatureState_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))CmdFeatureState_run,
};

static ObjIface* _ifimpl_CmdFeatureState[] = {
    (ObjIface*)&_impl_CmdFeatureState_CmdFeatureState_ClassIf,
    NULL
};

ObjClassInfo CmdFeatureState_clsinfo = {
    .instsize = sizeof(CmdFeatureState),
    .classif = (ObjIface*)&CmdFeatureState_ClassIf_tmpl,
    .parent = &ControlTask_clsinfo,
    .ifimpl = _ifimpl_CmdFeatureState,
};

// ==================== Auto-generated section ends ======================

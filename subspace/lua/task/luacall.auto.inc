// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LuaCall_ClassIf LuaCall_ClassIf_tmpl = {
    ._size = sizeof(LuaCall_ClassIf),
    ._parent = (ObjIface*)&LuaTask_ClassIf_tmpl,
};

LuaCallbackDispatch_ClassIf LuaCallbackDispatch_ClassIf_tmpl = {
    ._size = sizeof(LuaCallbackDispatch_ClassIf),
    ._parent = (ObjIface*)&LuaCall_ClassIf_tmpl,
};

LuaFuncRefCleanup_ClassIf LuaFuncRefCleanup_ClassIf_tmpl = {
    ._size = sizeof(LuaFuncRefCleanup_ClassIf),
    ._parent = (ObjIface*)&LuaTask_ClassIf_tmpl,
};

static LuaCall_ClassIf _impl_LuaCall_LuaCall_ClassIf = {
    ._size = sizeof(LuaCall_ClassIf),
    ._implements = (ObjIface*)&LuaCall_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LuaCall_run,
};

static ObjIface* _ifimpl_LuaCall[] = {
    (ObjIface*)&_impl_LuaCall_LuaCall_ClassIf,
    NULL
};

ObjClassInfo LuaCall_clsinfo = {
    .instsize = sizeof(LuaCall),
    .classif = (ObjIface*)&LuaCall_ClassIf_tmpl,
    .parent = &LuaTask_clsinfo,
    .init = (bool(*)(void*))LuaCall_init,
    .destroy = (void(*)(void*))LuaCall_destroy,
    .ifimpl = _ifimpl_LuaCall,
};

static LuaCallbackDispatch_ClassIf _impl_LuaCallbackDispatch_LuaCallbackDispatch_ClassIf = {
    ._size = sizeof(LuaCallbackDispatch_ClassIf),
    ._implements = (ObjIface*)&LuaCallbackDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LuaCallbackDispatch_run,
};

static ObjIface* _ifimpl_LuaCallbackDispatch[] = {
    (ObjIface*)&_impl_LuaCallbackDispatch_LuaCallbackDispatch_ClassIf,
    NULL
};

ObjClassInfo LuaCallbackDispatch_clsinfo = {
    .instsize = sizeof(LuaCallbackDispatch),
    .classif = (ObjIface*)&LuaCallbackDispatch_ClassIf_tmpl,
    .parent = &LuaCall_clsinfo,
    .destroy = (void(*)(void*))LuaCallbackDispatch_destroy,
    .ifimpl = _ifimpl_LuaCallbackDispatch,
};

static ObjIface* _ifimpl_LuaFuncRef[] = {
    NULL
};

ObjClassInfo LuaFuncRef_clsinfo = {
    .instsize = sizeof(LuaFuncRef),
    .destroy = (void(*)(void*))LuaFuncRef_destroy,
    .ifimpl = _ifimpl_LuaFuncRef,
};

static LuaFuncRefCleanup_ClassIf _impl_LuaFuncRefCleanup_LuaFuncRefCleanup_ClassIf = {
    ._size = sizeof(LuaFuncRefCleanup_ClassIf),
    ._implements = (ObjIface*)&LuaFuncRefCleanup_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LuaFuncRefCleanup_run,
};

static ObjIface* _ifimpl_LuaFuncRefCleanup[] = {
    (ObjIface*)&_impl_LuaFuncRefCleanup_LuaFuncRefCleanup_ClassIf,
    NULL
};

ObjClassInfo LuaFuncRefCleanup_clsinfo = {
    .instsize = sizeof(LuaFuncRefCleanup),
    .classif = (ObjIface*)&LuaFuncRefCleanup_ClassIf_tmpl,
    .parent = &LuaTask_clsinfo,
    .ifimpl = _ifimpl_LuaFuncRefCleanup,
};

// ==================== Auto-generated section ends ======================

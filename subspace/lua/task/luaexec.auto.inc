// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LuaExecString_ClassIf LuaExecString_ClassIf_tmpl = {
    ._size = sizeof(LuaExecString_ClassIf),
    ._parent = (ObjIface*)&LuaTask_ClassIf_tmpl,
};

LuaExecFile_ClassIf LuaExecFile_ClassIf_tmpl = {
    ._size = sizeof(LuaExecFile_ClassIf),
    ._parent = (ObjIface*)&LuaTask_ClassIf_tmpl,
};

static LuaExecString_ClassIf _impl_LuaExecString_LuaExecString_ClassIf = {
    ._size = sizeof(LuaExecString_ClassIf),
    ._implements = (ObjIface*)&LuaExecString_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LuaExecString_run,
};

static ObjIface* _ifimpl_LuaExecString[] = {
    (ObjIface*)&_impl_LuaExecString_LuaExecString_ClassIf,
    NULL
};

ObjClassInfo LuaExecString_clsinfo = {
    .instsize = sizeof(LuaExecString),
    .classif = (ObjIface*)&LuaExecString_ClassIf_tmpl,
    .parent = &LuaTask_clsinfo,
    .destroy = (void(*)(void*))LuaExecString_destroy,
    .ifimpl = _ifimpl_LuaExecString,
};

static LuaExecFile_ClassIf _impl_LuaExecFile_LuaExecFile_ClassIf = {
    ._size = sizeof(LuaExecFile_ClassIf),
    ._implements = (ObjIface*)&LuaExecFile_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LuaExecFile_run,
};

static ObjIface* _ifimpl_LuaExecFile[] = {
    (ObjIface*)&_impl_LuaExecFile_LuaExecFile_ClassIf,
    NULL
};

ObjClassInfo LuaExecFile_clsinfo = {
    .instsize = sizeof(LuaExecFile),
    .classif = (ObjIface*)&LuaExecFile_ClassIf_tmpl,
    .parent = &LuaTask_clsinfo,
    .destroy = (void(*)(void*))LuaExecFile_destroy,
    .ifimpl = _ifimpl_LuaExecFile,
};

// ==================== Auto-generated section ends ======================

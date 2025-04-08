// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
LuaTask_ClassIf LuaTask_ClassIf_tmpl = {
    ._size = sizeof(LuaTask_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static LuaTask_ClassIf _impl_LuaTask_LuaTask_ClassIf = {
    ._size = sizeof(LuaTask_ClassIf),
    ._implements = (ObjIface*)&LuaTask_ClassIf_tmpl,
};

static ObjIface* _ifimpl_LuaTask[] = {
    (ObjIface*)&_impl_LuaTask_LuaTask_ClassIf,
    NULL
};

ObjClassInfo LuaTask_clsinfo = {
    .instsize = sizeof(LuaTask),
    .classif = (ObjIface*)&LuaTask_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    ._abstract = true,
    .ifimpl = _ifimpl_LuaTask,
};

// ==================== Auto-generated section ends ======================

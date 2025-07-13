// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
GameData_ClassIf GameData_ClassIf_tmpl = {
    ._size = sizeof(GameData_ClassIf),
};

GameDataLoader_ClassIf GameDataLoader_ClassIf_tmpl = {
    ._size = sizeof(GameDataLoader_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static GameData_ClassIf _impl_GameData_GameData_ClassIf = {
    ._size = sizeof(GameData_ClassIf),
    ._implements = (ObjIface*)&GameData_ClassIf_tmpl,
    .load = (bool (*)(void*, Subspace*, VFS*, strref))GameData_load,
    .state = (GameDataState (*)(void*))GameData_state,
    .wait = (void (*)(void*, ComplexTask*))GameData_wait,
    .getText = (strref (*)(void*, strref))GameData_getText,
    .getTextD = (strref (*)(void*, strref, strref))GameData_getTextD,
};

static ObjIface* _ifimpl_GameData[] = {
    (ObjIface*)&_impl_GameData_GameData_ClassIf,
    NULL
};

ObjClassInfo GameData_clsinfo = {
    .instsize = sizeof(GameData),
    .classif = (ObjIface*)&GameData_ClassIf_tmpl,
    .init = (bool(*)(void*))GameData_init,
    .destroy = (void(*)(void*))GameData_destroy,
    .ifimpl = _ifimpl_GameData,
};

static GameDataLoader_ClassIf _impl_GameDataLoader_GameDataLoader_ClassIf = {
    ._size = sizeof(GameDataLoader_ClassIf),
    ._implements = (ObjIface*)&GameDataLoader_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))GameDataLoader_run,
};

static ObjIface* _ifimpl_GameDataLoader[] = {
    (ObjIface*)&_impl_GameDataLoader_GameDataLoader_ClassIf,
    NULL
};

ObjClassInfo GameDataLoader_clsinfo = {
    .instsize = sizeof(GameDataLoader),
    .classif = (ObjIface*)&GameDataLoader_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))GameDataLoader_destroy,
    .ifimpl = _ifimpl_GameDataLoader,
};

// ==================== Auto-generated section ends ======================

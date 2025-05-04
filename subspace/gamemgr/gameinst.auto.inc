// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
GameInst_ClassIf GameInst_ClassIf_tmpl = {
    ._size = sizeof(GameInst_ClassIf),
};

static GameInst_ClassIf _impl_GameInst_GameInst_ClassIf = {
    ._size = sizeof(GameInst_ClassIf),
    ._implements = (ObjIface*)&GameInst_ClassIf_tmpl,
    .launch = (bool (*)(void*))GameInst_launch,
    .setState = (void (*)(void*, GameInstState))GameInst_setState,
    .setStateLocked = (void (*)(void*, GameInstState))GameInst_setStateLocked,
    .getState = (GameInstState (*)(void*))GameInst_getState,
    .onGameReady = (void (*)(void*, ControlClient*))GameInst_onGameReady,
};

static ObjIface* _ifimpl_GameInst[] = {
    (ObjIface*)&_impl_GameInst_GameInst_ClassIf,
    NULL
};

ObjClassInfo GameInst_clsinfo = {
    .instsize = sizeof(GameInst),
    .classif = (ObjIface*)&GameInst_ClassIf_tmpl,
    .init = (bool(*)(void*))GameInst_init,
    .destroy = (void(*)(void*))GameInst_destroy,
    .ifimpl = _ifimpl_GameInst,
};

// ==================== Auto-generated section ends ======================

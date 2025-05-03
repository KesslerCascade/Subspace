// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
GameMgr_ClassIf GameMgr_ClassIf_tmpl = {
    ._size = sizeof(GameMgr_ClassIf),
};

static GameMgr_ClassIf _impl_GameMgr_GameMgr_ClassIf = {
    ._size = sizeof(GameMgr_ClassIf),
    ._implements = (ObjIface*)&GameMgr_ClassIf_tmpl,
    .reg = (void (*)(void*, GameInst*))GameMgr_reg,
    .unreg = (void (*)(void*, GameInst*))GameMgr_unreg,
    .acquireByCookie = (GameInst* (*)(void*, uint32))GameMgr_acquireByCookie,
    .launchGame = (bool (*)(void*, LaunchMode, GameInst**))GameMgr_launchGame,
    .genCookie = (uint32 (*)(void*))GameMgr_genCookie,
};

static ObjIface* _ifimpl_GameMgr[] = {
    (ObjIface*)&_impl_GameMgr_GameMgr_ClassIf,
    NULL
};

ObjClassInfo GameMgr_clsinfo = {
    .instsize = sizeof(GameMgr),
    .classif = (ObjIface*)&GameMgr_ClassIf_tmpl,
    .init = (bool(*)(void*))GameMgr_init,
    .destroy = (void(*)(void*))GameMgr_destroy,
    .ifimpl = _ifimpl_GameMgr,
};

// ==================== Auto-generated section ends ======================

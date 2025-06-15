// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
RunInfo_ClassIf RunInfo_ClassIf_tmpl = {
    ._size = sizeof(RunInfo_ClassIf),
};

static RunInfo_ClassIf _impl_RunInfo_RunInfo_ClassIf = {
    ._size = sizeof(RunInfo_ClassIf),
    ._implements = (ObjIface*)&RunInfo_ClassIf_tmpl,
    .newGame = (void (*)(void*, int, strref, strref, int))RunInfo_newGame,
    .loadGame = (void (*)(void*, int, strref, strref, int, int))RunInfo_loadGame,
    .loadHistoric = (bool (*)(void*, int64))RunInfo_loadHistoric,
    .replayLog = (void (*)(void*, bool, int64, int64))RunInfo_replayLog,
    .enterSector = (void (*)(void*, int, int, strref, bool))RunInfo_enterSector,
    .updateStats = (void (*)(void*, int, int, int, int))RunInfo_updateStats,
    .runLog = (void (*)(void*, int, int, int64, strref, stvar[LOG_MAX_PARAMS]))RunInfo_runLog,
    .processLog = (void (*)(void*, LogEnt*))RunInfo_processLog,
    .processScrap = (void (*)(void*, strref, int, int))RunInfo_processScrap,
    .score = (int32 (*)(void*))RunInfo_score,
    .finish = (void (*)(void*, RunResult))RunInfo_finish,
};

static ObjIface* _ifimpl_RunInfo[] = {
    (ObjIface*)&_impl_RunInfo_RunInfo_ClassIf,
    NULL
};

ObjClassInfo RunInfo_clsinfo = {
    .instsize = sizeof(RunInfo),
    .classif = (ObjIface*)&RunInfo_ClassIf_tmpl,
    .init = (bool(*)(void*))RunInfo_init,
    .destroy = (void(*)(void*))RunInfo_destroy,
    .ifimpl = _ifimpl_RunInfo,
};

// ==================== Auto-generated section ends ======================

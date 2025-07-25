// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
RunInfo_ClassIf RunInfo_ClassIf_tmpl = {
    ._size = sizeof(RunInfo_ClassIf),
};

LogReplay_ClassIf LogReplay_ClassIf_tmpl = {
    ._size = sizeof(LogReplay_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
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
    .runLog = (void (*)(void*, int, int, int64, float64, strref, stvar[LOG_MAX_PARAMS]))RunInfo_runLog,
    .beacon = (void (*)(void*, int, int, int, int, int, int64, strref))RunInfo_beacon,
    .processLog = (void (*)(void*, LogEnt*))RunInfo_processLog,
    .processScrap = (void (*)(void*, strref, int, int))RunInfo_processScrap,
    .processHullDamage = (void (*)(void*, strref, int))RunInfo_processHullDamage,
    .processShip = (void (*)(void*, strref))RunInfo_processShip,
    .setRecording = (void (*)(void*, bool))RunInfo_setRecording,
    .setFocused = (void (*)(void*, bool))RunInfo_setFocused,
    .isFocused = (bool (*)(void*))RunInfo_isFocused,
    .isRecording = (bool (*)(void*))RunInfo_isRecording,
    .getSector = (SectorInfo* (*)(void*, int64))RunInfo_getSector,
    .getHull = (HullTracker* (*)(void*))RunInfo_getHull,
    .getScrap = (ScrapTracker* (*)(void*))RunInfo_getScrap,
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

static LogReplay_ClassIf _impl_LogReplay_LogReplay_ClassIf = {
    ._size = sizeof(LogReplay_ClassIf),
    ._implements = (ObjIface*)&LogReplay_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))LogReplay_run,
};

static ObjIface* _ifimpl_LogReplay[] = {
    (ObjIface*)&_impl_LogReplay_LogReplay_ClassIf,
    NULL
};

ObjClassInfo LogReplay_clsinfo = {
    .instsize = sizeof(LogReplay),
    .classif = (ObjIface*)&LogReplay_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))LogReplay_destroy,
    .ifimpl = _ifimpl_LogReplay,
};

// ==================== Auto-generated section ends ======================

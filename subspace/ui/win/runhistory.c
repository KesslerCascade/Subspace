// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ui/win/runhistory.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <iupcontrols.h>
#include "db/database.h"
#include "gamemgr/gameinst.h"
#include "run/rundb.h"
#include "run/runinfo.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

#define PAGESIZE    100
#define BGCOLOR     "255 255 255"
#define SELCOLOR    "192 192 255"
#define ACTIVECOLOR "240 240 180"

_objfactory_guaranteed RunHistoryWin* RunHistoryWin_create(SubspaceUI* ui)
{
    RunHistoryWin* self;
    self = objInstCreate(RunHistoryWin);

    self->ss = ui->ss;
    self->ui = ui;

    self->sort     = RHC_Start;
    self->sortdesc = true;
    self->page     = 1;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool RunHistoryWin_init(_In_ RunHistoryWin* self)
{
    // Autogen begins -----
    saInit(&self->rows, sarray, 1);
    return true;
    // Autogen ends -------
}

static int closebtn_action(Ihandle* ih)
{
    RunHistoryWin* win = iupGetParentObj(RunHistoryWin, ih);
    if (win) {
        runhistorywinOnClose(win->win);
    }
    return IUP_DEFAULT;
}

static int loadbtn_action(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);

    int64 runid = runhistorywinSelectedRunId(self);
    if (runid > 0) {
        GameInst* inst = subspaceGame(self->ss);
        RunInfo* irun  = inst ? ginstRun(inst) : NULL;
        int64 activeid = irun ? irun->runid : -1;
        objRelease(&inst);

        // if this is the active run, just focus it, otherwise load a run
        if (runid == activeid) {
            subspaceSetRun(self->ss, irun);
            runinfoReplayLog(irun, false, 0, 0);
        } else {
            RunInfo* nrun = runinfoCreate(self->ss);
            if (runinfoLoadHistoric(nrun, runid)) {
                subspaceSetRun(self->ss, nrun);
                runinfoReplayLog(nrun, false, 0, 0);
                runhistorywinOnClose(self->win);
            }
            objRelease(&nrun);
        }
        objRelease(&irun);
    }

    return IUP_DEFAULT;
}

static int abandonbtn_action(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);

    Ihandle* dlg = IupMessageDlg();
    IupSetStrAttribute(dlg, "TITLE", langGetC(self->ss, "runhistory_title"));
    IupSetStrAttribute(dlg, "VALUE", langGetC(self->ss, "runhistory_abandon_confirm"));
    IupSetAttribute(dlg, "PARENTDIALOG", "SUBSPACE_RUNHISTORY");
    IupSetAttribute(dlg, "DIALOGTYPE", "QUESTION");
    IupSetAttribute(dlg, "BUTTONS", "YESNO");
    IupSetAttribute(dlg, "BUTTONDEFAULT", "2");
    IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
    int response = IupGetInt(dlg, "BUTTONRESPONSE");
    if (response == 1) {
        rundbSetAbandoned(self->ss, runhistorywinSelectedRunId(self));
        runhistorywinQuery(self);
    }

    return IUP_DEFAULT;
}

static int deletebtn_action(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);

    Ihandle* dlg = IupMessageDlg();
    IupSetStrAttribute(dlg, "TITLE", langGetC(self->ss, "runhistory_title"));
    IupSetStrAttribute(dlg, "VALUE", langGetC(self->ss, "runhistory_delete_confirm"));
    IupSetAttribute(dlg, "PARENTDIALOG", "SUBSPACE_RUNHISTORY");
    IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
    IupSetAttribute(dlg, "BUTTONS", "YESNO");
    IupSetAttribute(dlg, "BUTTONDEFAULT", "2");
    IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
    int response = IupGetInt(dlg, "BUTTONRESPONSE");
    if (response == 1) {
        rundbDelete(self->ss, runhistorywinSelectedRunId(self));
        runhistorywinQuery(self);
    }

    return IUP_DEFAULT;
}

static int history_select(Ihandle* ih, int lin, int col)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);

    runhistorywinSelect(self, lin);

    return IUP_DEFAULT;
}

static int history_click(Ihandle* ih, int lin, int col, char* status)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);
    if (iup_isbutton1(status) && iup_isdouble(status) && lin > 0) {
        // on double click, act like they selected the line and then chose Load
        runhistorywinSelect(self, lin);
        loadbtn_action(ih);
    }
    return IUP_DEFAULT;
}

static int search_change(Ihandle* ih, int c, char* new_value)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);
    strClear(&self->searchtxt);
    strCopy(&self->searchtxt, (strref)new_value);

    // reset timer so it goes off 1 second after the last input
    IupSetAttribute(self->searchtimer, "RUN", "NO");
    IupSetAttribute(self->searchtimer, "TIME", "1000");
    IupSetAttribute(self->searchtimer, "RUN", "YES");

    return IUP_DEFAULT;
}

static int search_timer(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);
    IupSetAttribute(ih, "RUN", "NO");
    runhistorywinQuery(self);

    return IUP_DEFAULT;
}

static int pgup_action(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);
    self->page          = clamplow(self->page - 1, 1);
    runhistorywinQuery(self);

    return IUP_DEFAULT;
}

static int pgdown_action(Ihandle* ih)
{
    RunHistoryWin* self = iupGetParentObj(RunHistoryWin, ih);
    self->page          = clamphigh(self->page + 1, self->npages);
    runhistorywinQuery(self);

    return IUP_DEFAULT;
}

bool RunHistoryWin_make(_In_ RunHistoryWin* self)
{
    self->searchtimer = IupTimer();
    iupSetObj(self->searchtimer, ObjNone, self, self->ui);
    IupSetCallback(self->searchtimer, "ACTION_CB", (Icallback)search_timer);

    Ihandle* searchlbl = IupLabel(langGetC(self->ss, "runhistory_search"));
    IupSetAttribute(searchlbl, "EXPAND", "VERTICALFREE");
    self->search = IupText("");
    IupSetAttribute(self->search, "EXPAND", "HORIZONTAL");
    iupSetObj(self->search, ObjNone, self, self->ui);
    IupSetCallback(self->search, "ACTION", (Icallback)search_change);

    Ihandle* catsep = IupSpace();
    IupSetAttribute(catsep, "SIZE", "20x1");

    Ihandle* catlbl = IupLabel(langGetC(self->ss, "runhistory_category"));
    IupSetAttribute(catlbl, "EXPAND", "VERTICALFREE");

    self->category = IupList("");
    IupSetAttribute(self->category, "DROPDOWN", "YES");
    IupSetStrAttribute(self->category, "1", langGetC(self->ss, "runhistory_category_all"));
    IupSetAttribute(self->category, "VALUE", "1");
    IupSetAttribute(self->category, "ACTIVE", "NO");

    Ihandle* topline = IupHbox(searchlbl, self->search, catsep, catlbl, self->category, NULL);
    IupSetAttribute(topline, "CMARGIN", "0x0");
    IupSetAttribute(topline, "CGAP", "3");

    self->rmtx = IupMatrix(NULL);
    IupSetAttribute(self->rmtx, "EXPAND", "YES");
    IupSetAttribute(self->rmtx, "CURSOR", "ARROW");
    IupSetAttribute(self->rmtx, "FLAT", "YES");
    IupSetAttribute(self->rmtx, "READONLY", "YES");
    IupSetAttribute(self->rmtx, "HIDEFOCUS", "YES");
    IupSetAttribute(self->rmtx, "NUMCOL", "8");
    IupSetAttribute(self->rmtx, "NUMCOL_VISIBLE", "8");
    IupSetAttribute(self->rmtx, "ALIGNMENT", "ALEFT");
    IupSetAttribute(self->rmtx, "SCROLLBAR", "VERTICAL");
    IupSetAttribute(self->rmtx, "YAUTOHIDE", "NO");
    IupSetAttribute(self->rmtx, "BORDER", "YES");
    IupSetAttribute(self->rmtx, "BGCOLOR", "255 255 255");
    IupSetAttribute(self->rmtx, "FRAMECOLOR", "255 255 255");
    IupSetAttribute(self->rmtx, "ALIGNMENTLIN0", "ALEFT");
    IupSetStrAttribute(self->rmtx, "0:1", langGetC(self->ss, "runhistory_col_start"));
    IupSetStrAttribute(self->rmtx, "0:2", langGetC(self->ss, "runhistory_col_result"));
    IupSetStrAttribute(self->rmtx, "0:3", langGetC(self->ss, "runhistory_col_shiptype"));
    IupSetStrAttribute(self->rmtx, "0:4", langGetC(self->ss, "runhistory_col_shipname"));
    IupSetStrAttribute(self->rmtx, "0:5", langGetC(self->ss, "runhistory_col_sector"));
    IupSetStrAttribute(self->rmtx, "0:6", langGetC(self->ss, "runhistory_col_difficulty"));
    IupSetStrAttribute(self->rmtx, "0:7", langGetC(self->ss, "runhistory_col_category"));
    IupSetStrAttribute(self->rmtx, "0:8", langGetC(self->ss, "runhistory_col_notes"));
    iupSetObj(self->rmtx, ObjNone, self, self->ui);
    IupSetCallback(self->rmtx, "ENTERITEM_CB", (Icallback)history_select);
    IupSetCallback(self->rmtx, "CLICK_CB", (Icallback)history_click);

    Ihandle* rmbg = IupBackgroundBox(self->rmtx);
    IupSetAttribute(rmbg, "BGCOLOR", "255 255 255");

    self->status = IupLabel("");
    IupSetAttribute(self->status, "EXPAND", "VERTICALFREE");

    self->pgup = IupButton("<", "");
    IupSetAttribute(self->pgup, "ACTIVE", "NO");
    iupSetObj(self->pgup, ObjNone, self, self->ui);
    IupSetCallback(self->pgup, "ACTION", (Icallback)pgup_action);
    self->pages = IupLabel("0 / 0");
    IupSetAttribute(self->pages, "EXPAND", "VERTICALFREE");
    self->pgdown = IupButton(">", "");
    IupSetAttribute(self->pgdown, "ACTIVE", "NO");
    iupSetObj(self->pgdown, ObjNone, self, self->ui);
    IupSetCallback(self->pgdown, "ACTION", (Icallback)pgdown_action);

    Ihandle*
        bottomline = IupHbox(self->status, IupFill(), self->pgup, self->pages, self->pgdown, NULL);
    IupSetAttribute(bottomline, "CMARGIN", "0x0");
    IupSetAttribute(bottomline, "CGAP", "3");

    Ihandle* closebtn = IupButton(langGetC(self->ss, "runhistory_close"), NULL);
    IupSetAttribute(closebtn, "CPADDING", "6x3");
    iupSetObj(closebtn, ObjNone, self, self->ui);
    IupSetCallback(closebtn, "ACTION", (Icallback)closebtn_action);

    self->loadbtn = IupButton(langGetC(self->ss, "runhistory_load"), NULL);
    IupSetAttribute(self->loadbtn, "CPADDING", "6x3");
    IupSetAttribute(self->loadbtn, "ACTIVE", "NO");
    iupSetObj(self->loadbtn, ObjNone, self, self->ui);
    IupSetCallback(self->loadbtn, "ACTION", (Icallback)loadbtn_action);

    self->editbtn = IupButton(langGetC(self->ss, "runhistory_edit"), NULL);
    IupSetAttribute(self->editbtn, "CPADDING", "6x3");
    IupSetAttribute(self->editbtn, "ACTIVE", "NO");
    iupSetObj(self->editbtn, ObjNone, self, self->ui);

    self->deletebtn = IupButton(langGetC(self->ss, "runhistory_delete"), NULL);
    IupSetAttribute(self->deletebtn, "CPADDING", "6x3");
    IupSetAttribute(self->deletebtn, "ACTIVE", "NO");
    iupSetObj(self->deletebtn, ObjNone, self, self->ui);
    IupSetCallback(self->deletebtn, "ACTION", (Icallback)deletebtn_action);

    self->abandonbtn = IupButton(langGetC(self->ss, "runhistory_abandon"), NULL);
    IupSetAttribute(self->abandonbtn, "CPADDING", "6x3");
    IupSetAttribute(self->abandonbtn, "ACTIVE", "NO");
    iupSetObj(self->abandonbtn, ObjNone, self, self->ui);
    IupSetCallback(self->abandonbtn, "ACTION", (Icallback)abandonbtn_action);

    Ihandle* buttonbox = IupHbox(closebtn,
                                 self->loadbtn,
                                 self->editbtn,
                                 IupFill(),
                                 self->deletebtn,
                                 self->abandonbtn,
                                 NULL);
    IupSetAttribute(buttonbox, "CMARGIN", "0x0");
    IupSetAttribute(buttonbox, "CGAP", "3");

    Ihandle* dlglayout = IupVbox(topline, rmbg, bottomline, buttonbox, NULL);

    IupSetAttribute(dlglayout, "CMARGIN", "6x6");
    IupSetAttribute(dlglayout, "CGAP", "6");

    self->win = IupDialog(dlglayout);
    IupSetStrf(self->win, "SIZE", "%dx%d", RUNHISTORYWIN_WIDTH, RUNHISTORYWIN_HEIGHT);
    IupSetAttribute(self->win, "RESIZE", "NO");
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, "runhistory_title"));
    IupSetAttribute(self->win, "ICON", "SUBSPACE_ICON");
    iupSetObj(self->win, self, ObjNone, self->ui);
    IupSetAttribute(self->win, "PARENTDIALOG", "SUBSPACE_MAINWIN");
    IupSetHandle("SUBSPACE_RUNHISTORY", self->win);

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)RunHistoryWin_onClose);
    return true;
}

void RunHistoryWin_show(_In_ RunHistoryWin* self)
{
    runhistorywinMake(self);

    IupMap(self->win);

    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    IupSetAttribute(self->win, "SIMULATEMODAL", "YES");

    runhistorywinQuery(self);
}

int RunHistoryWin_onClose(Ihandle* ih)
{
    IupSetAttribute(ih, "SIMULATEMODAL", "NO");
    IupHide(ih);
    return IUP_DEFAULT;
}

void RunHistoryWin_finish(_In_ RunHistoryWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    if (self->searchtimer)
        IupDestroy(self->searchtimer);
    self->win         = NULL;
    self->searchtimer = NULL;
}

static DbStmt* RunQuery(RunHistoryWin* self, int limit, int offset, bool countonly)
{
    Database* db = self->ss->db;
    string sql   = 0;
    string searchlike = 0;

    if (!strEmpty(self->searchtxt)) {
        strNConcat(&searchlike, _S"%", self->searchtxt, _S"%");
    }

    if (countonly)
        strDup(&sql, _S"SELECT COUNT(runid) FROM runs");
    else
        strDup(
            &sql,
            _S"SELECT runid, start, result, shiptype, shipname, sectorpoint, difficulty FROM runs");

    if (!strEmpty(searchlike)) {
        strAppend(&sql, _S" WHERE shiptype LIKE ? OR shipname LIKE ?");
    }

    switch (self->sort) {
    case RHC_Difficulty:
        strAppend(&sql, _S" ORDER BY difficulty");
        break;
    case RHC_Sector:
        strAppend(&sql, _S" ORDER BY sectorpoint");
        break;
    case RHC_ShipName:
        strAppend(&sql, _S" ORDER BY shipname");
        break;
    case RHC_ShipType:
        strAppend(&sql, _S" ORDER BY shiptype");
        break;
    case RHC_Start:
    default:
        strAppend(&sql, _S" ORDER BY start");
        break;
    }

    if (self->sortdesc)
        strAppend(&sql, _S" DESC");

    if (limit > 0)
        strAppend(&sql, _S" LIMIT ?");

    if (offset > 0)
        strAppend(&sql, _S" OFFSET ?");

    DbStmt* ret = dbstmtCreate(db, sql);
    if (!ret)
        goto out;

    int bc = 1;
    if (!strEmpty(searchlike)) {
        dbstmtBind(ret, bc++, stvar(strref, searchlike));
        dbstmtBind(ret, bc++, stvar(strref, searchlike));
    }

    if (limit > 0)
        dbstmtBind(ret, bc++, stvar(int32, limit));

    if (offset > 0)
        dbstmtBind(ret, bc++, stvar(int32, offset));

out:
    strDestroy(&sql);
    strDestroy(&searchlike);
    return ret;
}

void RunHistoryWin_query(_In_ RunHistoryWin* self)
{
    string temp = 0, temp2 = 0;
    DbStmt* stmt = RunQuery(self, PAGESIZE, (self->page - 1) * PAGESIZE, false);
    if (!stmt)
        return;

    GameInst* inst = subspaceGame(self->ss);
    RunInfo* irun  = inst ? ginstRun(inst) : NULL;
    int64 activeid = irun ? irun->runid : -1;
    objRelease(&irun);
    objRelease(&inst);

    self->selectrow = 0;
    IupSetAttribute(self->loadbtn, "ACTIVE", "NO");
    IupSetAttribute(self->editbtn, "ACTIVE", "NO");
    IupSetAttribute(self->abandonbtn, "ACTIVE", "NO");
    IupSetAttribute(self->deletebtn, "ACTIVE", "NO");

    saClear(&self->rows);
    IupSetAttribute(self->rmtx, "NUMLIN", "0");
    IupSetInt(self->rmtx, "NUMLIN", PAGESIZE);
    int row = 1;

    while (dbstmtExec(stmt) && saSize(stmt->row) == 7) {
        int64 runid;
        stConvert(int64, &runid, stvar, stmt->row.a[0]);
        saPush(&self->rows, sarray, stmt->row);

        int64 starttime;
        stConvert(int64, &starttime, stvar, stmt->row.a[1]);
        int64 ltime = toLocalTime(starttime);
        TimeParts p;
        timeDecompose(&p, ltime);

        strFormat(&temp2, _S"weekday_short${int}", stvar(int32, p.wday));
        strFormat(&temp,
                  _S"${string} ${int}-${0int(2)}-${0int(2)} ${0int(2)}:${0int(2)}:${0int(2)}",
                  stvar(strref, langGet(self->ss, temp2)),
                  stvar(int32, p.year),
                  stvar(int32, p.month),
                  stvar(int32, p.day),
                  stvar(int32, p.hour),
                  stvar(int32, p.minute),
                  stvar(int32, p.second));
        IupSetStrAttributeId2(self->rmtx, "", row, 1, strC(temp));

        int32 result = 0;
        stConvert(int32, &result, stvar, stmt->row.a[2]);
        const char* resultstr = NULL;
        if (result == RUN_Victory)
            resultstr = "runhistory_victory";
        else if (result == RUN_Defeat)
            resultstr = "runhistory_defeat";
        else if (result == RUN_Abandoned)
            resultstr = "runhistory_abandoned";
        if (resultstr)
            IupSetStrAttributeId2(self->rmtx, "", row, 2, langGetC(self->ss, resultstr));

        strDestroy(&temp);
        stConvert(string, &temp, stvar, stmt->row.a[3]);
        strConcat(&temp2, _S"ship_shortname_", temp);
        IupSetStrAttributeId2(self->rmtx, "", row, 3, strC(langGetD(self->ss, temp2, temp)));

        strDestroy(&temp);
        stConvert(string, &temp, stvar, stmt->row.a[4]);
        IupSetStrAttributeId2(self->rmtx, "", row, 4, strC(temp));

        int64 sectorpoint = 0;
        stConvert(int64, &sectorpoint, stvar, stmt->row.a[5]);
        spointFormat(&temp, sectorpoint);
        IupSetStrAttributeId2(self->rmtx, "", row, 5, strC(temp));

        int32 diff = 0;
        stConvert(int32, &diff, stvar, stmt->row.a[6]);
        const char* diffstr = "difficulty_unknown";
        if (diff == 2)
            diffstr = "difficulty_hard";
        else if (diff == 1)
            diffstr = "difficulty_normal";
        else if (diff == 0)
            diffstr = "difficulty_easy";
        IupSetStrAttributeId2(self->rmtx, "", row, 6, langGetC(self->ss, diffstr));

        if (runid == activeid) {
            strFormat(&temp, _S"BGCOLOR${int}:*", stvar(int32, row));
            IupSetAttribute(self->rmtx, strC(temp), ACTIVECOLOR);
            for (int i = 0; i < 8; i++) {
                IupSetAttributeId2(self->rmtx, "FRAMEVERTCOLOR", row, i, ACTIVECOLOR);
            }
        }

        row++;
    }

    IupSetInt(self->rmtx, "NUMLIN", row - 1);
    for (int i = 1; i < 8; i++) {
        IupSetAttributeId(self->rmtx, "RASTERWIDTH", i, NULL);
    }
    IupSetAttribute(self->rmtx, "FITTOTEXT", "C1");
    IupSetAttribute(self->rmtx, "FITTOTEXT", "C2");
    IupSetAttribute(self->rmtx, "FITTOTEXT", "C3");
    IupSetAttribute(self->rmtx, "FITTOTEXT", "C5");
    IupSetAttribute(self->rmtx, "FITTOTEXT", "C6");

    // add padding to the autosized columns
    int rh = IupGetInt(self->rmtx, "RASTERHEIGHT0");
    for (int i = 1; i < 8; i++) {
        if (i == 4)
            continue;

        int rw = IupGetIntId(self->rmtx, "RASTERWIDTH", i);
        IupSetIntId(self->rmtx, "RASTERWIDTH", i, rw + rh);
    }
    IupSetAttribute(self->rmtx, "FITTOSIZE", "COLUMNS");
    IupSetAttribute(self->rmtx, "REDRAW", "ALL");

    int total       = 0;
    DbStmt* countst = RunQuery(self, 0, 0, true);
    if (countst && dbstmtExec(countst) && saSize(countst->row) == 1) {
        stConvert(int32, &total, stvar, countst->row.a[0]);
    }
    objRelease(&countst);
    strFormat(&temp,
              langGet(self->ss, _S"runhistory_status"),
              stvar(int32, clamplow(row - 1, 0)),
              stvar(int32, total));
    IupSetStrAttribute(self->status, "TITLE", strC(temp));

    self->npages = (total + PAGESIZE - 1) / PAGESIZE;
    strFormat(&temp, _S"${int} / ${int}", stvar(int32, self->page), stvar(int32, self->npages));
    IupSetStrAttribute(self->pages, "TITLE", strC(temp));

    IupSetAttribute(self->pgup, "ACTIVE", (self->page > 1) ? "YES" : "NO");
    IupSetAttribute(self->pgdown, "ACTIVE", (self->page < self->npages) ? "YES" : "NO");

    objRelease(&stmt);
    strDestroy(&temp);
    strDestroy(&temp2);

    IupRefresh(self->win);
}

void RunHistoryWin_select(_In_ RunHistoryWin* self, int row)
{
    if (row < 1 || row > saSize(self->rows))
        return;

    GameInst* inst = subspaceGame(self->ss);
    RunInfo* irun  = inst ? ginstRun(inst) : NULL;
    int64 activeid = irun ? irun->runid : -1;
    objRelease(&irun);
    objRelease(&inst);

    string temp = 0;
    if (self->selectrow > 0) {
        int64 runid = runhistorywinSelectedRunId(self);

        strFormat(&temp, _S"BGCOLOR${int}:*", stvar(int32, self->selectrow));
        IupSetAttribute(self->rmtx, strC(temp), (runid == activeid) ? ACTIVECOLOR : NULL);
        for (int i = 0; i < 8; i++) {
            IupSetAttributeId2(self->rmtx,
                               "FRAMEVERTCOLOR",
                               self->selectrow,
                               i,
                               (runid == activeid) ? ACTIVECOLOR : NULL);
        }
    }

    strFormat(&temp, _S"BGCOLOR${int}:*", stvar(int32, row));
    IupSetAttribute(self->rmtx, strC(temp), SELCOLOR);
    for (int i = 0; i < 8; i++) {
        IupSetAttributeId2(self->rmtx, "FRAMEVERTCOLOR", row, i, SELCOLOR);
    }
    self->selectrow = row;

    IupSetAttribute(self->loadbtn, "ACTIVE", "YES");
    // IupSetAttribute(self->editbtn, "ACTIVE", "YES");
    bool canabandon = false;

    sa_stvar* rowa = (sa_stvar*)&self->rows.a[row - 1];
    int32 result   = -1;
    stConvert(int32, &result, stvar, rowa->a[2]);
    canabandon = (result == RUN_Active);

    IupSetAttribute(self->abandonbtn, "ACTIVE", canabandon ? "YES" : "NO");
    IupSetAttribute(self->deletebtn, "ACTIVE", "YES");

    IupSetAttribute(self->rmtx, "REDRAW", "ALL");

    strDestroy(&temp);
}

int64 RunHistoryWin_selectedRunId(_In_ RunHistoryWin* self)
{
    int64 runid = 0;
    if (self->selectrow <= saSize(self->rows)) {
        sa_stvar* row = (sa_stvar*)&self->rows.a[self->selectrow - 1];
        stConvert(int64, &runid, stvar, row->a[0]);
    }
    return runid;
}

void RunHistoryWin_destroy(_In_ RunHistoryWin* self)
{
    RunHistoryWin_finish(self);
    // Autogen begins -----
    strDestroy(&self->searchtxt);
    strDestroy(&self->catfilter);
    saDestroy(&self->rows);
    // Autogen ends -------
}

// Autogen begins -----
#include "ui/win/runhistory.auto.inc"
// Autogen ends -------

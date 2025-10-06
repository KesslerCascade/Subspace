// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/screenshot/screenshot.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <cx/utils.h>
#include <iupkey.h>
#include "control/controlserver.h"
#include "feature/screenshot/screenshotpage.h"
#include "kbmgr/kbmgr.h"
#include "run/runinfo.h"
#include "util/pathfilter.h"
#include "util/pathsubst.h"

#define MAX_FILE_ALTERNATES 999

static LazyInitState ssfilevars_init_state;
static hashtable ssfilevars;

static void ssfilevars_init(void* dummy)
{
    htInit(&ssfilevars, string, string, 20);
    htInsert(&ssfilevars, string, _S"year", string, _S"${0uint(4):year;0000}");
    htInsert(&ssfilevars, string, _S"month", string, _S"${0uint(2):month;00}");
    htInsert(&ssfilevars, string, _S"day", string, _S"${0uint(2):day;00}");
    htInsert(&ssfilevars, string, _S"hour", string, _S"${0uint(2):hour;00}");
    htInsert(&ssfilevars, string, _S"minute", string, _S"${0uint(2):minute;00}");
    htInsert(&ssfilevars, string, _S"second", string, _S"${0uint(2):second;00}");
    htInsert(&ssfilevars, string, _S"shipname", string, _S"${string:shipname;The Nameless One}");
    htInsert(&ssfilevars, string, _S"shiptype", string, _S"${string:shiptype;Unknown A}");
    htInsert(&ssfilevars, string, _S"beacon", string, _S"${string:beacon;0}");
    htInsert(&ssfilevars, string, _S"sector", string, _S"${string:sector;0}");
    htInsert(&ssfilevars, string, _S"difficulty", string, _S"${string:difficulty;0}");
    htInsert(&ssfilevars, string, _S"seed", string, _S"${uint:seed;0}");
}

_objfactory_guaranteed Screenshot* Screenshot_create(Subspace* ss)
{
    Screenshot* self;
    self = objInstCreate(Screenshot);

    self->ss = ss;
    strDup(&self->name, _S"Screenshot");

    kbmgrReg(ss->kbmgr, self, _S"screenshot_take", K_F12);

    objInstInit(self);
    return self;
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) \
    SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* Screenshot_createSettingsPage(_In_ Screenshot* self, SubspaceUI* ui)
{
    return SettingsPage(screenshotpageCreate(self, ui));
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void Screenshot_applyDefaultSettings(_In_ Screenshot* self)
{
    ssdLockedTransaction(self->settings)
    {
        if (!ssdPtr(self->settings, _S"sound"))
            ssdSet(self->settings, _S"sound", false, stvar(bool, true));
        if (!ssdPtr(self->settings, _S"hidemouse"))
            ssdSet(self->settings, _S"hidemouse", false, stvar(bool, true));
        if (!ssdPtr(self->settings, _S"hidepause"))
            ssdSet(self->settings, _S"hidepause", false, stvar(bool, false));
        if (!ssdPtr(self->settings, _S"hideinfoblock"))
            ssdSet(self->settings, _S"hideinfoblock", false, stvar(bool, false));
        if (!ssdPtr(self->settings, _S"metadata"))
            ssdSet(self->settings, _S"metadata", false, stvar(bool, true));

        if (!ssdPtr(self->settings, _S"filename"))
            ssdSet(self->settings, _S"filename", true, stvar(string, SCREENSHOTNAME_DEFAULT));

        if (!ssdPtr(self->settings, _S"format"))
            ssdSet(self->settings, _S"format", true, stvar(string, _S"png"));
        if (!ssdPtr(self->settings, _S"jpegquality"))
            ssdSet(self->settings, _S"jpegquality", true, stvar(int32, 95));

        if (!ssdPtr(self->settings, _S"manual/loc"))
            ssdSet(self->settings, _S"manual/loc", true, stvar(int32, SSLOC_Global));
        if (!ssdPtr(self->settings, _S"auto/loc"))
            ssdSet(self->settings, _S"auto/loc", true, stvar(int32, SSLOC_Global));

        if (!ssdPtr(self->settings, _S"events"))
            ssdSet(self->settings, _S"events", true, stvar(uint32, 0));
    }
}

static VFSFile* ssCreateFile(Screenshot* self, VFS* outvfs, string* outfn)
{
    VFSFile* outf = NULL;
    string ocfn   = NULL;   // candidate filename
    string ext    = NULL;
    string num    = NULL;
    int ialt      = 1;

    strDup(&ocfn, *outfn);
    pathGetExt(&ext, ocfn);
    // try to find a filename that doesn't already exist
    withMutex (&self->fileCreateLock) {
        for (int ialt = 1; !outf && ialt < MAX_FILE_ALTERNATES; ialt++) {
            if (ialt > 1) {
                // remove extension, add a numeric suffix, re-add extension
                pathRemoveExt(&ocfn, *outfn);
                strFromInt32(&num, ialt, 10);
                strNConcat(&ocfn, ocfn, _S" ", num);
                pathSetExt(&ocfn, ocfn, ext);
            }

            if (vfsExist(outvfs, ocfn)) {
                // If the file already exists, it's very likely that multiple screenshots are being
                // rapidly created in succession. Cache the last number we used and start checking
                // there to avoid potentially checking an ever-increasing number of filenames each
                // time. On the first iteration of the loop, skip ahead to the cached starting
                // point, since ialt == 1 indicates the un-suffixed filename.
                if (ialt == 1)
                    ialt = max(self->lastAlt, 1);
            } else {
                outf = vfsOpen(outvfs, ocfn, FS_Create | FS_Write);
            }
        }

        self->lastAlt = ialt - 1;
    }

    if (outf)
        strDup(outfn, ocfn);   // give actual filename back to caller

    strDestroy(&ocfn);
    strDestroy(&ext);
    strDestroy(&num);
    return outf;
}

void Screenshot_process(_In_ Screenshot* self, _In_opt_ strref fn, RunInfo* run, int64 timestamp,
                        bool automatic, hashtable fields)
{
    lazyInit(&ssfilevars_init_state, ssfilevars_init, NULL);
    VFS* outvfs   = NULL;
    string outdir = 0, outfn = 0, outpath = 0, temp = 0, fmt = 0;
    bool usemeta = ssdVal(bool, self->settings, _S"metadata", true);
    uint8* data;
    bool jpeg      = false;
    VFSFile* inf   = NULL;
    VFSFile* outf  = NULL;
    uint8* exifbuf = NULL;
    uint32 exifsz  = 0;
    string xmp     = 0;

    ssdStringOutD(self->settings, _S"format", &temp, _S"png");
    if (strEq(temp, _S"jpeg"))
        jpeg = true;

    int32 loc = ssdVal(int32, self->settings, automatic ? _S"auto/loc" : _S"manual/loc", SSLOC_Global);
    switch (loc) {
    case SSLOC_Custom:
        outvfs = self->ss->rootfs;
        ssdStringOut(self->settings, automatic ? _S"auto/custom" : _S"manual/custom", &outdir);
        break;
    case SSLOC_Rundir:
        if (!run)
            goto out;

        outvfs = self->ss->fs;
        runinfoGetRunDir(run, &outdir);
        pathJoin(&outdir, RUNDIR_FILENAME, outdir);

        break;
    case SSLOC_Global:
    default:
        outvfs = self->ss->fs;
        strDup(&outdir, SCREENSHOTDIR_FILENAME);
        break;
    }

    if (!vfsIsDir(outvfs, outdir) && !vfsCreateAll(outvfs, outdir))
        goto out;

    hashtable ivars, svars;
    int64 runid, savepoint;

    // create format string by substituting in variables
    ssdStringOutD(self->settings, _S"filename", &temp, SCREENSHOTNAME_DEFAULT);
    pathSubst(&fmt, temp, ssfilevars);

    // hash tables for integer and string vars
    htInit(&ivars, string, uint32, 10);
    htInit(&svars, string, string, 10);

    if (usemeta) {
        int major = cfieldValD(int32, fields, _S"major", -1);
        if (major != -1) {
            strFormat(&temp,
                      _S"FTL ${int}.${int}.${int}",
                      stvar(int32, major),
                      stvar(int32, cfieldValD(int32, fields, _S"minor", 0)),
                      stvar(int32, cfieldValD(int32, fields, _S"rev", 0)));
        }

        // insert special field for metadata processors to use
        htInsert(&fields, string, _S"meta_software", stvar, stvar(string, temp));
    }

    TimeParts tp;
    timeDecompose(&tp, toLocalTime(timestamp));
    htInsert(&ivars, string, _S"year", uint32, tp.year);
    htInsert(&ivars, string, _S"month", uint32, tp.month);
    htInsert(&ivars, string, _S"day", uint32, tp.day);
    htInsert(&ivars, string, _S"hour", uint32, tp.hour);
    htInsert(&ivars, string, _S"minute", uint32, tp.minute);
    htInsert(&ivars, string, _S"second", uint32, tp.second);

    htInsert(&ivars, string, _S"seed", uint32, cfieldValD(uint32, fields, _S"seed", 0));

    int difficulty = cfieldValD(int32, fields, _S"difficulty", 0);
    string diffstr = _S"difficulty_unknown";
    if (difficulty == 2)
        diffstr = _S"difficulty_hard";
    else if (difficulty == 1)
        diffstr = _S"difficulty_normal";
    else if (difficulty == 0)
        diffstr = _S"difficulty_easy";
    htInsert(&svars, string, _S"difficulty", strref, langGet(self->ss, diffstr));
    htInsert(&fields, string, _S"meta_difficulty", stvar, stvar(strref, langGet(self->ss, diffstr)));

    htInsert(&svars, string, _S"shipname", strref, cfieldString(fields, _S"name"));
    strref shiptype = cfieldString(fields, _S"ship");
    if (!shiptype)
        shiptype = _S"unknown";

    strConcat(&temp, _S"ship_shortname_", cfieldString(fields, _S"ship"));
    htInsert(&svars, string, _S"shiptype", strref, langGetD(self->ss, temp, shiptype));
    htInsert(&fields,
             string,
             _S"meta_shiptype",
             stvar,
             stvar(strref, langGetD(self->ss, temp, shiptype)));

    if (run) {
        withReadLock (&run->lock) {
            spointFormat(&temp, run->savepoint);
            htInsert(&svars, string, _S"beacon", string, temp);
            htInsert(&fields, string, _S"meta_beacon", stvar, stvar(strref, temp));
            spointFormat(&temp, run->sectorpoint);
            htInsert(&svars, string, _S"sector", string, temp);
            htInsert(&fields, string, _S"meta_sector", stvar, stvar(strref, temp));
        }
    } else {
        spointFormat(&temp, SPOINT(cfieldValD(int32, fields, _S"beacon", 0), 0));
        htInsert(&svars, string, _S"beacon", string, temp);
        htInsert(&fields, string, _S"meta_beacon", stvar, stvar(strref, temp));
        spointFormat(&temp, SPOINT(cfieldValD(int32, fields, _S"sector", 0), 0));
        htInsert(&svars, string, _S"sector", string, temp);
        htInsert(&fields, string, _S"meta_sector", stvar, stvar(strref, temp));
    }

    strref sectortype = cfieldString(fields, _S"sectortype");
    if (sectortype) {
        GameData* data = subspaceData(self->ss);
        if (data) {
            strConcat(&temp, _S"sectorname_", sectortype);
            htInsert(&fields,
                     string,
                     _S"meta_sectorname",
                     stvar,
                     stvar(strref, gamedataGetTextD(data, temp, sectortype)));
            objRelease(&data);
        }
    }

    strFormat(&temp, fmt, stvar(hashtable, ivars), stvar(hashtable, svars));
    htDestroy(&ivars);
    htDestroy(&svars);
    pathFilter(&outfn, temp);
    pathSetExt(&outfn, outfn, jpeg ? _S"jpg" : _S"png");

    pathJoin(&outpath, outdir, outfn);

    int w = cfieldValD(int32, fields, _S"w", 0);
    int h = cfieldValD(int32, fields, _S"h", 0);
    if (w <= 0 || h <= 0)
        goto out;

    inf = vfsOpen(self->ss->rootfs, fn, FS_Read);
    if (!inf)
        goto out;

    // read raw image data
    size_t imgsz = w * h * 3;
    size_t didread;
    data = xaAlloc(imgsz);
    if (!vfsRead(inf, data, imgsz, &didread) || didread != imgsz)
        goto out;

    outf = ssCreateFile(self, outvfs, &outpath);
    if (!outf)
        goto out;

    if (usemeta) {
        screenshotPopulateEXIF(self, &exifbuf, &exifsz, run, timestamp, automatic, fields);
        screenshotPopulateXMP(self, &xmp, run, timestamp, automatic, fields);
    }

    bool success = false;
    if (jpeg)
        success = screenshotWriteJpeg(self, outf, data, w, h, exifbuf, exifsz, xmp);
    else
        success = screenshotWritePng(self, outf, data, w, h, exifbuf, exifsz, xmp, timestamp);

    if (!success)
        vfsDelete(outvfs, outpath);

out:
    xaFree(exifbuf);
    xaFree(data);
    vfsClose(inf);
    vfsClose(outf);
    vfsDelete(self->ss->rootfs, fn);
    strDestroy(&outdir);
    strDestroy(&outfn);
    strDestroy(&outpath);
    strDestroy(&temp);
    strDestroy(&fmt);

    return;
}

_objinit_guaranteed bool Screenshot_init(_In_ Screenshot* self)
{
    // Autogen begins -----
    mutexInit(&self->fileCreateLock);
    return true;
    // Autogen ends -------
}

void Screenshot_destroy(_In_ Screenshot* self)
{
    // Autogen begins -----
    mutexDestroy(&self->fileCreateLock);
    // Autogen ends -------
}

// Autogen begins -----
// clang-format off
bool Screenshot_populateEXIF(_In_ Screenshot* self, uint8** buf, uint32* bufsz, RunInfo* run, int64 timestamp, bool automatic, hashtable fields);
bool Screenshot_populateXMP(_In_ Screenshot* self, string* out, RunInfo* run, int64 timestamp, bool automatic, hashtable fields);
bool Screenshot_writeJpeg(_In_ Screenshot* self, VFSFile* out, uint8* data, int w, int h, uint8* exifdata, uint32 exifdsz, string xmpdata);
bool Screenshot_writePng(_In_ Screenshot* self, VFSFile* out, uint8* data, int w, int h, uint8* exifdata, uint32 exifdsz, string xmpdata, int64 timestamp);
#include "feature/screenshot/screenshot.auto.inc"
// clang-format on
// Autogen ends -------

#include "cxautogen.h"
#include <cx/container.h>
#include <cx/string.h>
#include <cx/utils.h>

sa_Interface ifaces;
hashtable ifidx;
sa_Class classes;
hashtable clsidx;
hashtable weakrefidx;
sa_string includes;
sa_string implincludes;
sa_string deps;
sa_string structs;
sa_string fwdclass;
sa_ComplexArrayType artypes;
hashtable knownartypes;
string cpassthrough;
bool needmixinimpl;

bool upToDate(string fname);

int main(int argc, char* argv[])
{
    bool force = false;
    sa_string inputfiles;
    sa_string searchpath;
    string fname   = 0;
    string srcpath = 0;
    string binpath = 0;
    saInit(&ifaces, object, 16);
    htInit(&ifidx, string, object, 16);
    saInit(&classes, object, 16);
    htInit(&clsidx, string, object, 16);
    htInit(&weakrefidx, string, object, 16);
    saInit(&includes, string, 8);
    saInit(&implincludes, string, 4);
    saInit(&deps, string, 8);
    saInit(&structs, string, 8);
    saInit(&fwdclass, string, 8);
    saInit(&artypes, object, 8);
    htInit(&knownartypes, string, bool, 16);

    saInit(&searchpath, string, 8);
    saInit(&inputfiles, string, 4);

    string tmp = 0;
    for (int i = 1; i < argc; i++) {
        strSubStr(&tmp, (string)argv[i], 0, 2);
        if (strEq(tmp, _S"-I")) {
            strSubStr(&tmp, (string)argv[i], 2, strEnd);
            pathFromPlatform(&tmp, tmp);
            saPush(&searchpath, string, tmp);
        } else if (strEq(tmp, _S"-S")) {
            strSubStr(&tmp, (string)argv[i], 2, strEnd);
            pathFromPlatform(&srcpath, tmp);
        } else if (strEq(tmp, _S"-B")) {
            strSubStr(&tmp, (string)argv[i], 2, strEnd);
            pathFromPlatform(&binpath, tmp);
        } else if (strEq((string)argv[i], _S"-f")) {
            force = true;
        } else {
            pathFromPlatform(&tmp, (string)argv[i]);
            saPush(&inputfiles, string, tmp);
        }
    }
    strDestroy(&tmp);

    for (int i = 0; i < saSize(inputfiles); i++) {
        saClear(&ifaces);
        htClear(&ifidx);
        saClear(&classes);
        htClear(&clsidx);
        htClear(&weakrefidx);
        saClear(&includes);
        saClear(&implincludes);
        saClear(&structs);
        saClear(&fwdclass);
        saClear(&artypes);
        htClear(&knownartypes);
        strDestroy(&cpassthrough);
        needmixinimpl = false;

        // standard interfaces should always be available, but it's non-fatal if
        // the file can't be located
        if (!strEndsWith(inputfiles.a[i], _S"objstdif.cxh")) {
            strDup(&fname, _S"cx/obj/objstdif.cxh");
            parseFile(fname, NULL, NULL, searchpath, true, false);
            strClear(&fname);
        }

        if (!parseFile(inputfiles.a[i], &fname, srcpath, searchpath, false, true))
            break;

        if (strEmpty(fname))   // already parsed this file
            continue;

        if (!force && upToDate(fname))
            continue;

        if (!processInterfaces())
            break;
        if (!processClasses())
            break;
        if (!writeHeader(fname, srcpath, binpath))
            break;
        if (!writeImpl(fname, srcpath, binpath, false))
            break;
        if (needmixinimpl && !writeImpl(fname, srcpath, binpath, true))
            break;
    }

    htDestroy(&ifidx);
    saDestroy(&ifaces);
    htDestroy(&clsidx);
    htDestroy(&weakrefidx);
    saDestroy(&classes);
    saDestroy(&implincludes);
    saDestroy(&includes);
    saDestroy(&deps);
    saDestroy(&structs);
    saDestroy(&fwdclass);
    strDestroy(&cpassthrough);
    strDestroy(&fname);
    saDestroy(&searchpath);
    saDestroy(&inputfiles);

    return 0;
}

bool upToDate(string fname)
{
    bool ret     = false;
    string hname = 0;
    pathSetExt(&hname, fname, _S"h");
    string cname = 0;
    pathSetExt(&cname, fname, _S"c");

    // get the oldest timestamp of the generated .c and .h files
    FSStat statv;
    if (fsStat(hname, &statv) == FS_Nonexistent)
        goto out;
    int64 oldestgen = statv.modified;
    if (fsStat(cname, &statv) == FS_Nonexistent)
        goto out;
    oldestgen = min(oldestgen, statv.modified);

    // get the newest timestamp of the input file and all includes
    fsStat(fname, &statv);
    int64 newestsrc = statv.modified;
    for (int i = 0; i < saSize(deps); i++) {
        if (fsStat(deps.a[i], &statv))
            newestsrc = max(newestsrc, statv.modified);
    }

    ret = newestsrc <= oldestgen;

out:
    strDestroy(&hname);
    strDestroy(&cname);
    return ret;
}

uint8* lazyPlatformPath(string path)
{
    string tmp = 0;
    pathToPlatform(&tmp, path);
    uint8* out = scratchGet(strLen(tmp) + 1);
    strCopyOut(tmp, 0, out, strLen(tmp) + 1);
    strDestroy(&tmp);
    return out;
}

void relSrcPath(string* out, strref fname, strref srcpath)
{
    string lfname = 0;
    strDup(&lfname, fname);

    if (strEmpty(srcpath) || !strBeginsWith(fname, srcpath))
        goto out;

    strSubStrI(&lfname, strLen(srcpath), strEnd);
    while (strGetChar(lfname, 0) == '/') strSubStrI(&lfname, 1, strEnd);

out:
    strDup(out, lfname);
    strDestroy(&lfname);
}

void binPath(string* out, strref fname, strref srcpath, strref binpath)
{
    string lfname = 0;
    strDup(&lfname, fname);

    if (strEmpty(srcpath) || strEmpty(binpath))
        goto out;

    if (!strBeginsWith(fname, srcpath))
        goto out;

    strSubStrI(&lfname, strLen(srcpath), strEnd);
    while (strGetChar(lfname, 0) == '/') strSubStrI(&lfname, 1, strEnd);
    pathJoin(&lfname, binpath, lfname);

out:
    strDup(out, lfname);
    strDestroy(&lfname);
}

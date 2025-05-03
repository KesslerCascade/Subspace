#include "lang.h"
#include <cx/serialize/jsonparse.h>

static void langParseCB(_In_ JSONParseEvent* ev, _Inout_opt_ void* userdata)
{
    hashtable* tgt = (hashtable*)userdata;

    if (ev->etype != JSON_String || ev->ctx->ctype != JSON_Object)
        return;

    htInsert(tgt, string, ev->ctx->cdata.curKey, string, ev->edata.strData);
}

bool langList(Subspace* ss, hashtable* out)
{
    bool ret = false;

    VFSFile* lf = vfsOpen(ss->fs, LANGLIST_FILENAME, FS_Read);
    if (!lf)
        return false;

    StreamBuffer* jsbuf = sbufCreate(4096);
    if (sbufFilePRegisterPull(jsbuf, lf, false) && jsonParse(jsbuf, langParseCB, out))
        ret = true;

    vfsClose(lf);
    sbufRelease(&jsbuf);
    return ret;
}

static bool langLoadOne(Subspace* ss, strref lang, bool fallback)
{
    bool ret = false;

    string filename = 0;
    pathJoin(&filename, LANGDIR_FILENAME, lang);
    pathSetExt(&filename, filename, _S"json");
    VFSFile* lf = vfsOpen(ss->fs, filename, FS_Read);
    if (!lf) {
        logFmt(Error, _S"Could not open language file ${string}", stvar(string, filename));
        strDestroy(&filename);
        return false;
    }

    StreamBuffer* jsbuf = sbufCreate(4096);
    if (sbufFilePRegisterPull(jsbuf, lf, false) && languagedbLoad(ss->lang, jsbuf, fallback)) {
        ret = true;
    } else {
        logFmt(Error, _S"Parse error reading language file ${string}", stvar(string, filename));
    }

    vfsClose(lf);
    sbufRelease(&jsbuf);
    strDestroy(&filename);
    return ret;
}

bool langLoad(Subspace* ss, strref lang)
{
    bool ret = false;

    objRelease(&ss->lang);
    ss->lang = languagedbCreate();

    ret = langLoadOne(ss, lang, false);

    // use English as a fallback database for missing translations
    if (!strEq(lang, _S"en-us"))
        ret |= langLoadOne(ss, _S"en-us", true);

    if (ret)
        strDup(&ss->langid, lang);

    return ret;
}

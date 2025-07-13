#pragma once

#include <cx/string.h>
#include "langdb.h"
#include "subspace.h"

bool langList(Subspace* ss, hashtable* out);
bool langLoad(Subspace* ss, strref lang);   // UI MUST NOT BE RUNNING!

_meta_inline strref langGet(Subspace* ss, strref key)
{
    return languagedbGet(ss->lang, key, key);
}

_meta_inline strref langGetD(Subspace* ss, strref key, strref def)
{
    return languagedbGet(ss->lang, key, def);
}

_meta_inline const char* langGetC(Subspace* ss, const char* key)
{
    string* sptr = languagedb_getPtr(ss->lang, (strref)key);
    if (sptr)
        return strPC(sptr);

    return key;
}

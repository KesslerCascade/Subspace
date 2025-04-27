#pragma once

#include <cx/string.h>
#include "langdb.h"
#include "subspace.h"

bool langList(Subspace* ss, hashtable* out);
bool langLoad(Subspace* ss, strref lang);   // UI MUST NOT BE RUNNING!

_meta_inline strref langGet(Subspace* ss, strref key)
{
    return languagedbGet(ss->lang, key);
}

_meta_inline const char* langGetC(Subspace* ss, strref key)
{
    return strC(languagedbGet(ss->lang, key));
}

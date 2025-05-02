#include "controlserver.h"

#define CFIELD_IMPL(type)                                            \
    bool _cfieldVal_##type(hashtable fields, strref name, type* out) \
    {                                                                \
        htelem hte = htFind(fields, strref, name, none, NULL);       \
        if (!hte)                                                    \
            return false;                                            \
        stvar* val = hteValPtr(fields, stvar, hte);                  \
        return stConvert(type, out, stvar, *val);                    \
    }

#define CFIELD_IMPLD(type)                                           \
    type _cfieldValD_##type(hashtable fields, strref name, type def) \
    {                                                                \
        htelem hte = htFind(fields, strref, name, none, NULL);       \
        if (!hte)                                                    \
            return def;                                              \
        stvar* val = hteValPtr(fields, stvar, hte);                  \
        type out;                                                    \
        if (stConvert(type, &out, stvar, *val))                      \
            return out;                                              \
        else                                                         \
            return def;                                              \
    }

CFIELD_IMPL(bool);
CFIELD_IMPLD(bool);
CFIELD_IMPL(int32);
CFIELD_IMPLD(int32);
CFIELD_IMPL(uint32);
CFIELD_IMPLD(uint32);
CFIELD_IMPL(float32);
CFIELD_IMPLD(float32);
CFIELD_IMPL(float64);
CFIELD_IMPLD(float64);

strref cfieldString(hashtable fields, strref name)
{
    htelem hte = htFind(fields, strref, name, none, NULL);
    if (!hte)
        return NULL;
    stvar* val = hteValPtr(fields, stvar, hte);
    if (stEq(val->type, stType(string)))
        return val->data.st_strref;
    return NULL;
}

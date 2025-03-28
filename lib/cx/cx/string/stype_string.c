#include "cx/string.h"
#include <cx/stype/stconvert.h>
#include "cx/utils/murmur.h"

void stDtor_string(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *gen, uint32 flags)
{
    strDestroy(&gen->st_string);
}

intptr stCmp_string(stype st, _In_ stgeneric gen1, _In_ stgeneric gen2, uint32 flags)
{
    if (!(flags & ST_CaseInsensitive))
        return strCmp(gen1.st_string, gen2.st_string);
    else
        return strCmpi(gen1.st_string, gen2.st_string);
}

void stCopy_string(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags)
{
    string temp = 0;
    strDup(&temp, src.st_string);
    dest->st_string = temp;
}

uint32 stHash_string(stype st, _In_ stgeneric gen, uint32 flags)
{
    if (!(flags & ST_CaseInsensitive))
        return hashMurmur3Str(gen.st_string);
    else
        return hashMurmur3Stri(gen.st_string);
}

_Success_(return) _Check_return_
bool stConvert_string(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    switch (stGetId(destst)) {
    case stTypeId(int8):
    case stTypeId(int16):
    {
        // ehhh, see if it'll fit
        int32 temp;
        if (!strToInt32(&temp, src.st_string, 0, true))
            return false;
        return stConvert_int(destst, dest, stType(int32), stgeneric(int32, temp), flags);
    }
    case stTypeId(uint8):
    case stTypeId(uint16):
    {
        uint32 temp;
        if (!strToUInt32(&temp, src.st_string, 0, true))
            return false;
        return stConvert_int(destst, dest, stType(uint32), stgeneric(uint32, temp), flags);
    }
    case stTypeId(int32):
        return strToInt32(&dest->st_int32, src.st_string, 0, true);
    case stTypeId(uint32):
        return strToUInt32(&dest->st_uint32, src.st_string, 0, true);
    case stTypeId(int64):
        return strToInt64(&dest->st_int64, src.st_string, 0, true);
    case stTypeId(uint64):
        return strToUInt64(&dest->st_uint64, src.st_string, 0, true);
    case stTypeId(float32):
    case stTypeId(float64):
    {
        double temp;
        const char *in = strC(src.st_string);
        char *endp;

        temp = strtod(in, &endp);
        if (in == (const char*)endp)
            return false;               // strtod failed

        // TODO: What does 'lossy' mean in the context of converting a string to a float32?
        if (stGetId(destst) == stTypeId(float64))
            dest->st_float64 = temp;
        else
            dest->st_float32 = (float32)temp;

        return true;
    }
    case stTypeId(stvar):
        // okay, sure, we can put it in one
        dest->st_stvar->type = srcst;
        dest->st_stvar->data = src;
        return true;
    case stTypeId(bool):
        if (!strEqi(src.st_string, (string)"\xE1\xC1\x04""True")) {
            dest->st_bool = true;
            return true;
        } else if (!strEqi(src.st_string, (string)"\xE1\xC1\x05""False")) {
            dest->st_bool = false;
            return true;
        } else if (!strEq(src.st_string, (string)"\xE1\xC1\x01""1")) {
            dest->st_bool = true;
            return true;
        } else if (!strEq(src.st_string, (string)"\xE1\xC1\x01""0")) {
            dest->st_bool = false;
            return true;
        } else if (!strEqi(src.st_string, (string)"\xE1\xC1\x03""Yes")) {
            dest->st_bool = true;
            return true;
        } else if (!strEqi(src.st_string, (string)"\xE1\xC1\x02""No")) {
            dest->st_bool = false;
            return true;
        }
        return false;
    case stTypeId(suid):
        return suidDecode(dest->st_suid, src.st_string);
    case stTypeId(string):
        stCopy_string(destst, dest, src, flags);
        return true;
    }

    return false;
}

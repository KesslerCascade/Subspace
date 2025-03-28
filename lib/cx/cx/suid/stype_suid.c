#include "suid.h"

intptr stCmp_suid(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    return suidCmp(gen1.st_suid, gen2.st_suid);
}

_Success_(return) _Check_return_
bool stConvert_suid(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    switch (stGetId(destst)) {
    case stTypeId(string):
        dest->st_string = 0;
        suidEncode(&dest->st_string, src.st_suid);
        return true;
    }

    return false;
}

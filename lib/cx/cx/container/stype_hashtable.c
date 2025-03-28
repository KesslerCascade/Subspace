#include "hashtable_private.h"
#include "cx/utils/murmur.h"

void stDtor_hashtable(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *gen, flags_t flags)
{
    htDestroy(&gen->st_hashtable);
}

void stCopy_hashtable(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags)
{
    htClone(&dest->st_hashtable, src.st_hashtable);
}

uint32 stHash_hashtable(stype st, _In_ stgeneric gen, flags_t flags)
{
    hashtable *htbl = &gen.st_hashtable;
    HashTableHeader *hdr = HTABLE_HDR(*htbl);
    uint32 ret = 0;

    for (uint32 i = _htNextSlot(hdr, 0); i != hashIndexEmpty; i = _htNextSlot(hdr, i)) {
        ret ^= _stHash(hdr->keytype, HDRKEYOPS(hdr),
                       stStored(hdr->keytype, HT_SLOT_KEY_PTR(hdr, i)), 0);
        if (stGetSize(hdr->valtype) > 0)
            ret ^= _stHash(hdr->valtype, HDRVALOPS(hdr),
                           stStored(hdr->valtype, HT_SLOT_VAL_PTR(hdr, i)), 0);
    }

    return ret;
}

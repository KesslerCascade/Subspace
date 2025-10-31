#include "digest_private.h"
#include "cx/debug/assert.h"
#include "cx/utils/compare.h"

_Static_assert(DIGEST_BLOCKSIZE == 64, "DIGEST_BLOCKSIZE must be 64");

uint32 DigestSize[DIGEST_COUNT] = {
    16,   // MD5
    20,   // SHA1
    32    // SHA256
};

_digestBlock _DigestBlockFunc[DIGEST_COUNT] = { _md5Block, _sha1Block, _sha256Block };

_digestInit _DigestInitFunc[DIGEST_COUNT] = { _md5Init, _sha1Init, _sha256Init };

_Use_decl_annotations_
void digestInit(Digest* digest, DigestType type)
{
    devAssert(type >= 0 && type < DIGEST_COUNT);

    digest->type = type;
    digest->size = 0;
    memset(digest->buffer, 0, sizeof(digest->buffer));
    memset(digest->state, 0, sizeof(digest->state));

    _DigestInitFunc[type](digest);
}

_Use_decl_annotations_
void digestUpdate(Digest* digest, uint8* data, uint32 size)
{
    while (size > 0) {
        // faster version of (digest->size % DIGEST_BLOCKSIZE) for powers of two
        uint32 bufpos = digest->size & (DIGEST_BLOCKSIZE - 1);
        uint32 tocopy = clamphigh(DIGEST_BLOCKSIZE - bufpos, size);

        memcpy(&digest->buffer[bufpos], data, tocopy);
        digest->size += tocopy;
        data += tocopy;
        size -= tocopy;

        if ((digest->size & (DIGEST_BLOCKSIZE - 1)) == 0)
            _DigestBlockFunc[digest->type](digest);
    }
}

_Use_decl_annotations_
void digestFinish(Digest* digest, uint8* out)
{
    uint8* p      = digest->buffer;
    uint32 bufpos = digest->size & (DIGEST_BLOCKSIZE - 1);

    // pad with 0x80 followed by zeros
    p[bufpos++] = 0x80;
    if (bufpos > DIGEST_BLOCKSIZE - 8) {
        // not enough space for length, pad rest with zeros and process block
        memset(&p[bufpos], 0, DIGEST_BLOCKSIZE - bufpos);
        _DigestBlockFunc[digest->type](digest);
        bufpos = 0;
    }
    memset(&digest->buffer[bufpos], 0, DIGEST_BLOCKSIZE - 8 - bufpos);

    // append length in bits
    uint32 hi  = digest->size >> 29;
    uint32 len = digest->size << 3;
    if (digest->type == DIGEST_MD5) {
        // MD5 uses little-endian length
        p[56] = len;
        p[57] = len >> 8;
        p[58] = len >> 16;
        p[59] = len >> 24;
        p[60] = hi;
        p[61] = p[62] = p[63] = 0;
    } else {
        // SHA1 and SHA256 use big-endian length
        p[56] = p[57] = p[58] = 0;
        p[59]                 = hi;
        p[60]                 = len >> 24;
        p[61]                 = len >> 16;
        p[62]                 = len >> 8;
        p[63]                 = len >> 0;
    }
    _DigestBlockFunc[digest->type](digest);

    // output result
    uint32 digestsize = DigestSize[digest->type];
    for (uint32 i = 0; i < digestsize >> 2; i++) {
        if (digest->type == DIGEST_MD5) {
            // MD5 uses little-endian output
            out[i * 4]     = (uint8)(digest->state[i] >> 0);
            out[i * 4 + 1] = (uint8)(digest->state[i] >> 8);
            out[i * 4 + 2] = (uint8)(digest->state[i] >> 16);
            out[i * 4 + 3] = (uint8)(digest->state[i] >> 24);
        } else {
            // SHA1 and SHA256 use big-endian output
            out[i * 4]     = (uint8)(digest->state[i] >> 24);
            out[i * 4 + 1] = (uint8)(digest->state[i] >> 16);
            out[i * 4 + 2] = (uint8)(digest->state[i] >> 8);
            out[i * 4 + 3] = (uint8)(digest->state[i] >> 0);
        }
    }
}

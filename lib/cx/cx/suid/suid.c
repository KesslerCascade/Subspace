#include "suid.h"
#include "cx/math/pcg.h"
#include "cx/time.h"
#include "cx/sys/hostid.h"

#include "cx/string.h"

typedef struct SuidTLSData {
    PcgState pcg;
    uint64 lastts;
    uint32 lastrng;
} SuidTLSData;

static _Thread_local SuidTLSData *suidTls;

#define checktls if (!suidTls) suidTls = suidTlsCreate()

// extern inline bool suidEq(const SUID *a, const SUID *b);
// extern inline int suidCmp(const SUID *a, const SUID *b);

SuidTLSData *suidTlsCreate()
{
    SuidTLSData *ret = xaAlloc(sizeof(*suidTls), XA_Zero);
    pcgAutoSeed(&ret->pcg);
    return ret;
}

static void _suidGen(_Out_ SUID *out, uint8 idtype, uint64 hid)
{
    hid &= 0xFFFFFFFFFFULL;             // mask off the part that we actually use

    uint64 systime = clockWall();
    systime /= 1000;                    // microseconds to milliseconds
    systime -= 210866760000000ULL;      // convert to UNIX epoch
    systime &= 0xFFFFFFFFFFFFULL;       // mask off low 48 bits
    out->high = (uint64)idtype << 56;   // high 8 bits are application-defined ID type
    out->high |= systime << 8;          // bits 8-56 are 48-bit timestamp
    out->high |= (hid >> 32);           // low 8 bits are high 8 bits of hostid (out of 40)
    out->low = (hid << 32);             // high 32 bits are low 32 bits of hostid

    if (systime <= suidTls->lastts) {
        // less than 1ms has elapsed, monotonically increment rng part instead
        suidTls->lastrng++;
        if (suidTls->lastrng == 0)      // overflow
            suidTls->lastts++;          // go 1ms to the future
    } else {
        suidTls->lastrng = pcgRandom(&suidTls->pcg);
        suidTls->lastts = systime;
    }

    out->low |= suidTls->lastrng;       // low 32 bits are random
}

_Use_decl_annotations_
void suidGen(SUID *out, uint8 idtype)
{
    checktls;

    HostID hostid;
    hostId(&hostid);

    uint64 id = hostid.id[0] ^ hostid.id[1] ^ hostid.id[2] ^ hostid.id[3];
    _suidGen(out, idtype, id);
}

_Use_decl_annotations_
void suidGenPrivate(SUID *out, uint8 idtype)
{
    uint64 hid;
    checktls;

    // temporary random host id
    hid = pcgRandom(&suidTls->pcg);
    hid <<= 32;
    hid |= pcgRandom(&suidTls->pcg);

    _suidGen(out, idtype, hid);
}

static const char b32encode[33] = "0123456789abcdefghjkmnpqrstvwxyz";

_Use_decl_annotations_
void suidEncodeBytes(uint8 dst[26], const SUID *id)
{
    dst[0] = b32encode[id->high >> 61];
    dst[1] = b32encode[(id->high & 0x1F00000000000000ULL) >> 56];
    dst[2] = b32encode[(id->high & 0xF8000000000000ULL) >> 51];
    dst[3] = b32encode[(id->high & 0x7C00000000000ULL) >> 46];
    dst[4] = b32encode[(id->high & 0x3E0000000000ULL) >> 41];
    dst[5] = b32encode[(id->high & 0x1F000000000ULL) >> 36];
    dst[6] = b32encode[(id->high & 0xF80000000ULL) >> 31];
    dst[7] = b32encode[(id->high & 0x7C000000) >> 26];
    dst[8] = b32encode[(id->high & 0x3E00000) >> 21];
    dst[9] = b32encode[(id->high & 0x1F0000) >> 16];
    dst[10] = b32encode[(id->high & 0xF800) >> 11];
    dst[11] = b32encode[(id->high & 0x7C0) >> 6];
    dst[12] = b32encode[(id->high & 0x3E) >> 1];
    dst[13] = b32encode[((id->high & 1) << 4) | ((id->low & 0xF000000000000000ULL) >> 60)];
    dst[14] = b32encode[(id->low & 0xF80000000000000ULL) >> 55];
    dst[15] = b32encode[(id->low & 0x7C000000000000ULL) >> 50];
    dst[16] = b32encode[(id->low & 0x3E00000000000ULL) >> 45];
    dst[17] = b32encode[(id->low & 0x1F0000000000ULL) >> 40];
    dst[18] = b32encode[(id->low & 0xF800000000ULL) >> 35];
    dst[19] = b32encode[(id->low & 0x7C0000000ULL) >> 30];
    dst[20] = b32encode[(id->low & 0x3E000000) >> 25];
    dst[21] = b32encode[(id->low & 0x1F00000) >> 20];
    dst[22] = b32encode[(id->low & 0xF8000) >> 15];
    dst[23] = b32encode[(id->low & 0x7C00) >> 10];
    dst[24] = b32encode[(id->low & 0x3E0) >> 5];
    dst[25] = b32encode[id->low & 0x1F];
}

_Use_decl_annotations_
void suidEncode(string *out, const SUID *id)
{
    strClear(out);
    uint8 *dst = strBuffer(out, 26);
    suidEncodeBytes(dst, id);
}

static const uint8_t dec[256] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* 0     1     2     3     4     5     6     7  */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    /* 8     9                                      */
    0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    /*    10(A) 11(B) 12(C) 13(D) 14(E) 15(F) 16(G) */
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    /*17(H) 1(I)18(J) 19(K) 1(L)  20(M) 21(N) 0(O)  */
    0x11, 0x01, 0x12, 0x13, 0x01, 0x14, 0x15, 0x00,
    /*22(P)23(Q)24(R) 25(S) 26(T)       27(V) 28(W) */
    0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C,
    /*29(X)30(Y)31(Z)                               */
    0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /*    10(a) 11(b) 12(c) 13(d) 14(e) 15(f) 16(g) */
    0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    /*17(h) 1(i)18(j) 19(k) 1(l)  20(m) 21(n) 0(o)  */
    0x11, 0x01, 0x12, 0x13, 0x01, 0x14, 0x15, 0x00,
    /*22(p)23(q)24(r) 25(s) 26(t)       27(v) 28(w) */
    0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C,
    /*29(x)30(y)31(z)                               */
    0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

_Use_decl_annotations_
bool suidDecodeBytes(SUID *out, const char buf[26])
{
    // validate input
    for (int i = 0; i < 26; i++) {
        if (dec[(uint8)buf[i]] == 0xff)
            return false;
    }
    out->high = ((uint64)dec[(uint8)buf[0]] << 61) |
        ((uint64)dec[(uint8)buf[1]] << 56) |
        ((uint64)dec[(uint8)buf[2]] << 51) |
        ((uint64)dec[(uint8)buf[3]] << 46) |
        ((uint64)dec[(uint8)buf[4]] << 41) |
        ((uint64)dec[(uint8)buf[5]] << 36) |
        ((uint64)dec[(uint8)buf[6]] << 31) |
        ((uint64)dec[(uint8)buf[7]] << 26) |
        ((uint64)dec[(uint8)buf[8]] << 21) |
        ((uint64)dec[(uint8)buf[9]] << 16) |
        ((uint64)dec[(uint8)buf[10]] << 11) |
        ((uint64)dec[(uint8)buf[11]] << 6) |
        ((uint64)dec[(uint8)buf[12]] << 1) |
        ((uint64)dec[(uint8)buf[13]] >> 4);
    out->low = ((uint64)dec[(uint8)buf[13]] << 60) |
        ((uint64)dec[(uint8)buf[14]] << 55) |
        ((uint64)dec[(uint8)buf[15]] << 50) |
        ((uint64)dec[(uint8)buf[16]] << 45) |
        ((uint64)dec[(uint8)buf[17]] << 40) |
        ((uint64)dec[(uint8)buf[18]] << 35) |
        ((uint64)dec[(uint8)buf[19]] << 30) |
        ((uint64)dec[(uint8)buf[20]] << 25) |
        ((uint64)dec[(uint8)buf[21]] << 20) |
        ((uint64)dec[(uint8)buf[22]] << 15) |
        ((uint64)dec[(uint8)buf[23]] << 10) |
        ((uint64)dec[(uint8)buf[24]] << 5) |
        dec[(uint8)buf[25]];

    return true;
}

_Use_decl_annotations_
bool suidDecode(SUID *out, strref str)
{
    if (!str || strLen(str) < 26)
        return false;

    const char *buf = strC(str);
    return suidDecodeBytes(out, buf);
}

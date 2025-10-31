#pragma once

// Generic support for several message digest algorithms with 64-byte block size.

#include <cx/cx.h>

#define DIGEST_BLOCKSIZE 64

typedef enum {
    DIGEST_MD5,
    DIGEST_SHA1,
    DIGEST_SHA256,
    DIGEST_COUNT
} DigestType;

extern uint32 DigestSize[DIGEST_COUNT]; // size of the resulting digest in bytes

typedef struct Digest {
    DigestType type;
    uint32 size;    // message size
    uint8 buffer[DIGEST_BLOCKSIZE]; // data buffer
    uint32 state[8]; // state (MD5: 4 words, SHA1: 5 words, SHA256: 8 words)
} Digest;

void digestInit(_Out_ Digest* digest, DigestType type);
void digestUpdate(_Inout_ Digest* digest, _In_ uint8* data, uint32 size);
void digestFinish(_Inout_ Digest* digest, _Out_ uint8* out);
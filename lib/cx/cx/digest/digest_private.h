#pragma once

#include "digest.h"

typedef void (*_digestInit)(Digest* digest);   // populate initial state
typedef void (*_digestBlock)(Digest* digest);  // process a full block from digest->buffer

void _md5Init(Digest* digest);
void _sha1Init(Digest* digest);
void _sha256Init(Digest* digest);

void _md5Block(Digest* digest);
void _sha1Block(Digest* digest);
void _sha256Block(Digest* digest);

extern _digestInit _DigestInitFunc[DIGEST_COUNT];
extern _digestBlock _DigestBlockFunc[DIGEST_COUNT];
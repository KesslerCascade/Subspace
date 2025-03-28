#pragma once

#include <cx/cx.h>

CX_C_BEGIN

uint32 hashMurmur3(_In_reads_bytes_(len) const uint8* key, size_t len);
uint32 hashMurmur3i(_In_reads_bytes_(len) const uint8* key, size_t len);
uint32 hashMurmur3Str(_In_opt_ strref s);
uint32 hashMurmur3Stri(_In_opt_ strref s);

CX_C_END

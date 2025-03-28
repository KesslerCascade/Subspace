#pragma once

#include "sarray.h"

#define ELEMPTR(hdr, i) ((void*)((uintptr)(&hdr->data[0]) + (uintptr)(i) * stGetSize(hdr->elemtype)))
#define SARRAY_SMALLHDR_OFFSET (offsetof(SArrayHeader, elemtype))
#define HDRTYPEOPS(hdr) ((hdr->flags & SAINT_Extended) ? &hdr->typeops : NULL)

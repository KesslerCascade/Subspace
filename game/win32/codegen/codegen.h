#pragma once

#include "codegen/asm.h"
#include "codegen/funcinfo.h"
#include "codegen/hookgen.h"
#include "codegen/wrapgen.h"

// NOTE! Codegen holds a critical section between Reserve and Complete, so don't
// delay too long before calling Complete.
unsigned char *cgReserve(int maxsize);
bool cgComplete(int actualsize);

// If you're doing a bunch of codegen in a row, wrap them in a batch
// to avoid constantly protecting / unprotecting memory.
bool cgStartBatch();
bool cgEndBatch();

#pragma once
#include <stdbool.h>
#include "loader/loader.h"

typedef enum {
    R_IGNORE   = 1,
    R_REQUIRED = 2,
    R_OPTIONAL = 4,
} PatchRelevancy;

enum PatchSegmentEnum {
    P_SEG_CODE = 0,   // Code
    P_SEG_DATA,       // Global read-write data
    P_SEG_RDATA,      // Read-only data like string literals
    P_SEG_COUNT       // Not a real segment, just the count
};

typedef struct PatchState {
    addr_t base;
    SegInfo segs[3];
    uintptr_t origprotect[3];
} PatchState;

typedef int PatchHandle;
typedef struct Patch Patch;
typedef struct Patch {
    PatchRelevancy (*Relevant)(addr_t base, Patch* p, PatchState* ps);
    bool (*Validate)(addr_t base, Patch* p, PatchState* ps);
    bool (*Apply)(addr_t base, Patch* p, PatchState* ps);
    int _state;
} Patch;

typedef Patch* PatchSequence[];

bool patchBegin(PatchState* ps, addr_t base);
bool patchEnd(PatchState* ps);
bool patchApplySeq(PatchState* ps, PatchSequence seq);

// Helper functions that can be used in patch structures
PatchRelevancy AlwaysRequired(addr_t base, Patch* p, PatchState* ps);
PatchRelevancy AlwaysOptional(addr_t base, Patch* p, PatchState* ps);
bool NoValidation(addr_t base, Patch* p, PatchState* ps);

// Generic function that can be used for replacements
int DoNothing();

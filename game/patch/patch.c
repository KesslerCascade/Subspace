#include "patch.h"

bool patchBegin(PatchState* ps, addr_t base)
{
    // get the segments we need to unprotect
    if (!getCodeSeg(base, &ps->segs[P_SEG_CODE]) || !getDataSeg(base, &ps->segs[P_SEG_DATA]) ||
        !getRDataSeg(base, &ps->segs[P_SEG_RDATA]))
        return false;

    for (int i = 0; i < P_SEG_COUNT; i++) {
        ps->origprotect[i] = makeSegWritable(&ps->segs[i]);
    }
    ps->base = base;
    return true;
}

bool patchEnd(PatchState* ps)
{
    for (int i = 0; i < P_SEG_COUNT; i++) {
        restoreSegAccess(&ps->segs[i], ps->origprotect[i]);
    }
    return true;
}

#define S_INIT      0x00
#define S_VALIDATED 0x08
#define S_APPLIED   0x10
void patchScanSeq(PatchState* ps, PatchSequence seq)
{
    addr_t base = ps->base;

    // First, scan the list to cache what is relevant
    for (Patch** pl = seq; *pl; pl++) {
        Patch* p = *pl;
        if (p->_state == S_INIT)
            p->_state |= p->Relevant(base, p, ps);
    }
}

bool patchValidateSeq(PatchState* ps, PatchSequence seq)
{
    addr_t base = ps->base;

    if (base == 0)
        return false;

    patchScanSeq(ps, seq);

    // Second, perform validation
    for (Patch** pl = seq; *pl; pl++) {
        Patch* p = *pl;
        if (p->_state & (R_IGNORE | S_VALIDATED))
            continue;

        if (p->Validate(base, p, ps))
            p->_state |= S_VALIDATED;
        else if (p->_state & R_REQUIRED)   // Abort whole sequence if a required
            return false;                  // patch fails to validate
    }

    return true;
}

bool patchApplySeq(PatchState* ps, PatchSequence seq)
{
    addr_t base = ps->base;

    if (!patchValidateSeq(ps, seq))
        return false;

    // Third, apply all validated patches still needed
    bool ret = true;
    for (Patch** pl = seq; *pl; pl++) {
        Patch* p = *pl;
        if ((p->_state & S_APPLIED) || !(p->_state & S_VALIDATED))
            continue;

        if (p->Apply(base, p, ps))
            p->_state |= S_APPLIED;
        else if (p->_state & R_REQUIRED)   // Already patching, don't quit halfway,
            ret = false;                   // but still return failure
    }

    return ret;
}

// helper functions

PatchRelevancy AlwaysRequired(addr_t base, Patch* p, PatchState* ps)
{
    return R_REQUIRED;
}

PatchRelevancy AlwaysOptional(addr_t base, Patch* p, PatchState* ps)
{
    return R_OPTIONAL;
}

bool NoValidation(addr_t base, Patch* p, PatchState* ps)
{
    return true;
}

bool NoPatch(addr_t base, Patch* p, PatchState* ps)
{
    return true;
}

int DoNothing()
{
    return 0;
}

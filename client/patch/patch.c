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
bool patchApplySeq(PatchState* ps, PatchSequence seq)
{
    addr_t base = ps->base;
    Patch **pl, *p;
    bool ret = true;

    if (base == 0)
        return false;

    // First, scan the list to cache what is relevant
    for (pl = seq; *pl; pl++) {
        p = *pl;
        if (p->_state == S_INIT)
            p->_state |= p->Relevant(base, p, ps);
    }

    // Second, perform validation
    for (pl = seq; *pl; pl++) {
        p = *pl;
        if (p->_state & (R_IGNORE | S_VALIDATED))
            continue;

        if (p->Validate(base, p, ps))
            p->_state |= S_VALIDATED;
        else if (p->_state & R_REQUIRED)   // Abort whole sequence if a required
            return false;                  // patch fails to validate
    }

    // Third, apply all validated patches still needed
    for (pl = seq; *pl; pl++) {
        p = *pl;
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

int DoNothing()
{
    return 0;
}

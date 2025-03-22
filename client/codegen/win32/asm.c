#include "asm.h"
#include "codegen.h"

#include "minicrt.h"

bool cgAsmInit(AsmState* as, int maxinstr)
{
    cgInit();

    if (maxinstr > 0) {
        as->dest = smalloc_heap(&codegen_heap, maxinstr * MAXCMDSIZE);
        if (!as->dest)
            return false;
        as->remaining = maxinstr * MAXCMDSIZE;
        as->reserved  = true;
    } else {
        as->reserved = false;
    }
    as->p           = as->dest;
    as->ci          = empty_asm;
    as->error       = false;
    as->errortxt[0] = '\0';

    return true;
}

bool cgAsm(AsmState* as)
{
    t_asmmodel res;
    t_asmmodel best;
    int attempt, csize;
    bool anysuccess;
    char tmperror[TEXTLEN];
    char zerror[TEXTLEN];

    // Optimize generated code by finding the shortest possible encoding
    best.length = MAXCMDSIZE + 1;
    attempt     = 0;
    do {
        anysuccess = false;
        for (csize = 0; csize < 4; csize++) {
            if (Assemble(&as->ci,
                         addr(as->p),
                         &res,
                         attempt,
                         csize,
                         (attempt == 0 && csize == 0) ? zerror : tmperror) > 0) {
                anysuccess = true;
                if (res.length < best.length)
                    best = res;
            }
        }
        attempt++;
    } while (anysuccess);

    if (best.length == MAXCMDSIZE + 1) {
        // failed to find anything
        strcpy(as->errortxt, zerror);
        as->error = true;
        return false;
    }

    if ((unsigned)best.length > as->remaining) {
        strcpy(as->errortxt, "Codegen buffer too small");
        as->error = true;
        return false;
    }

    // Copy generated code into buffer
    memcpy(as->p, best.code, best.length);
    as->p += best.length;
    as->remaining -= best.length;
    return true;
}

bool cgAsmFinish(AsmState* as)
{
    bool ret = true;
    if (as->reserved) {
        // TODO: smalloc shrink!!!
        /*        void* rp = srealloc_heap(&codegen_heap, as->dest, as->p - as->dest);
                if (rp)
                    as->dest = rp;
                else
                    ret = false; */
        as->p         = NULL;
        as->reserved  = false;
        as->remaining = 0;
    }
    return ret && !as->error;
}

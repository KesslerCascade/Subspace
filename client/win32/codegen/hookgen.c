#include "hookgen.h"
#include "codegen_private.h"
#include "asm.h"

#include "minicrt.h"

typedef struct HookState {
    const FuncInfo* fi;
    int espoffset;
    int argoffsets[32];
    int argsz;   // for stdcall
    int localsz;
} HookState;

static void callAdapt(AsmState* as, HookState* hs, addr_t func, int insertreg)
{
    const FuncInfo* fi = hs->fi;
    int i, j, dstack = 0;
    int fcallesp = 0;

    // all args are on the stack now -- InitArgs stashes register args in locals
    for (i = fi->nargs - 1; i >= 0; --i) {
        for (j = (fi->args[i].size / 4 - 1) * 4; j >= 0; j -= 4) {
            asmrd(as, I_MOV, REG_EAX, REG_ESP, -1, 0, hs->argoffsets[i] + j + hs->espoffset, 4);
            asmr(as, I_PUSH, REG_EAX);
            hs->espoffset += 4;
            fcallesp += 4;
        }
    }

    // If we're inserting an extra argument, do it last so that it comes first
    if (insertreg != REG_UNDEF) {
        asmr(as, I_PUSH, insertreg);
        hs->espoffset += 4;
        fcallesp += 4;
    }

    asmi(as, I_CALL, func);
    // Clean up after call
    if (fcallesp > 0) {
        asmri(as, I_ADD, REG_ESP, fcallesp);
        hs->espoffset -= fcallesp;
    }
}

static void initArgs(AsmState* as, HookState* hs)
{
    const FuncInfo* fi = hs->fi;
    int stackpos       = 4;
    int i;

    for (i = 0; i < fi->nargs; i++) {
        const ArgInfo* arg = &fi->args[i];
        if (arg->type == ARG_DUMMY) {
            // not actually used, but the wrapper needs something, so push a zero
            if (arg->size == 4) {
                asmi(as, I_PUSH, 0);
            } else {
                strcpy(as->errortxt, "Ignored args must be 32 bits in size");
                as->error = true;
            }
            hs->espoffset += 4;
            hs->localsz += 4;
            hs->argoffsets[i] = -hs->localsz;
        } else if (arg->stack) {
            hs->argoffsets[i] = stackpos;
            if (arg->size > 0 && arg->size < 4)
                stackpos += 4;   // shorts and chars are always extended
            else
                stackpos += arg->size;
        } else {
            // turn register arguments into locals, since registers will get clubbed
            if (arg->size == 4) {
                asmr(as, I_PUSH, arg->reg);
            } else if (arg->size == 2 && arg->reg >= REG_AX && arg->reg <= REG_DI) {
                // bit of a hack, just use whatever is in the high bits for padding
                asmr(as, I_PUSH, arg->reg - REG_AX);
            } else if (arg->size == 1 && arg->reg >= REG_AL && arg->reg <= REG_BL) {
                asmr(as, I_PUSH, arg->reg - REG_AL);
            } else {
                strcpy(as->errortxt, "Invalid register used for fastcall");
                as->error = true;
            }
            hs->espoffset += 4;
            hs->localsz += 4;
            hs->argoffsets[i] = -hs->localsz;
        }
    }

    hs->argsz = stackpos - 4;
}

static void cleanupLocals(AsmState* as, HookState* hs)
{
    if (hs->localsz > 0) {
        asmri(as, I_ADD, REG_ESP, hs->localsz);
        hs->espoffset -= hs->localsz;
        hs->localsz = 0;
    }
}

void* hookCreate(addr_t addr, const FuncInfo* fi, void* pre, void* post)
{
    AsmState asd;
    AsmState* as  = &asd;
    HookState hsd = { .fi = fi };
    HookState* hs = &hsd;
    int fcallesp  = 0;
    unsigned char* jmpsrc;
    int i, j;

    cgAsmInit(as, 32 + fi->nargs * 6);
    initArgs(as, hs);

    if (pre) {
        callAdapt(as, hs, addr(pre), -1);
        asmrr(as, I_TEST, REG_EAX, REG_EAX);

        // Save current location because we'll need to fix it up later.
        jmpsrc = as->p;

        asmi_p(as, I_JZ, 0);   // placeholder
        // Force long jump as we can't be sure how big the paramter code will be.
        as->ci.arg[0].ptrsize = -2;
        cgAsm(as);
    }

    // Simplified version of wrapper code to call original function. Don't need to worry
    // about saving registers since they are the same ones passed to this function.

    // Pass 1, copy parameters to stack of function we're calling in the right order
    for (i = fi->nargs - 1; i >= 0; --i) {
        if (!fi->args[i].stack || fi->args[i].type == ARG_DUMMY)
            continue;
        for (j = (fi->args[i].size / 4 - 1) * 4; j >= 0; j -= 4) {
            asmrd(as, I_MOV, REG_EAX, REG_ESP, -1, 0, hs->argoffsets[i] + j + hs->espoffset, 4);
            asmr(as, I_PUSH, REG_EAX);
            hs->espoffset += 4;
            fcallesp += 4;
        }
    }

    // Pass 2, load any register arguments
    for (i = 0; i < fi->nargs; i++) {
        if (fi->args[i].stack || fi->args[i].type == ARG_DUMMY)
            continue;
        asmrd(as,
              I_MOV,
              fi->args[i].reg,
              REG_ESP,
              -1,
              0,
              hs->argoffsets[i] + hs->espoffset,
              fi->args[i].size);
    }

    // Call the function
    asmi(as, I_CALL, addr);

    // Clean up after call
    if (fcallesp > 0 && !fi->stdcall) {
        asmri(as, I_ADD, REG_ESP, fcallesp);
    }

    hs->espoffset -= fcallesp;   // either we cleaned it up or stdcall function did

    if (post) {
        // Save return value; EAX is used by CallAdapt
        asmrr(as, I_MOV, REG_ECX, REG_EAX);
        callAdapt(as, hs, addr(post), REG_ECX);
    }

    if (pre) {
        unsigned long saved = as->remaining;
        unsigned char* sp   = as->p;

        // Go back and fix the jump
        as->p = jmpsrc;
        asmi_p(as, I_JZ, addr(sp));
        as->ci.arg[0].ptrsize = -2;
        cgAsm(as);

        as->p         = sp;
        as->remaining = saved;
    }

    cleanupLocals(as, hs);
    if (fi->stdcall && hs->argsz > 0) {
        asmi(as, I_RETN, hs->argsz);
    } else {
        asmn(as, I_RET);
    }

    if (!cgAsmFinish(as))
        return 0;

    return as->dest;
}

void* replacementCreate(const FuncInfo* fi, void* replacement)
{
    AsmState asd;
    AsmState* as  = &asd;
    HookState hsd = { .fi = fi };
    HookState* hs = &hsd;

    if (fi->purecdecl)
        return replacement;   // ok!

    cgAsmInit(as, 16 + fi->nargs * 2);
    initArgs(as, hs);

    callAdapt(as, hs, addr(replacement), -1);

    cleanupLocals(as, hs);
    if (fi->stdcall && hs->argsz > 0) {
        asmi(as, I_RETN, hs->argsz);
    } else {
        asmn(as, I_RET);
    }

    if (!cgAsmFinish(as))
        return 0;

    return as->dest;
}

#include "wrapgen.h"
#include "codegen_private.h"
#include "asm.h"

// Create a wrapper to adapt the calling convention of the specified function
void *wrapCreate(addr_t addr, const FuncInfo *fi)
{
    AsmState asd; AsmState *as = &asd;
    bool needsave[8] = { 0 };           // for saving registers we're not supposed to overwrite
    int argoffsets[32];
    int espoffset = 0;                  // amount that we've moved the stack pointer
    int fcallesp = 0;                   // how much of that offset is just for the wrapped call
    int stackpos = 4;                   // current position when scanning arguments
    int i, j;

    // If the function is pure cdecl, we can just call it directly
    if (fi->purecdecl)
        return (void*)addr;

    // Use a rough (over)guesstimate of how much space is needed
    if (!cgAsmInit(as, 4 + fi->nargs * 2))
        return 0;           // probably going to crash

    // Incoming arguments are all passed on stack, find them
    for (i = 0; i < fi->nargs; i++) {
        argoffsets[i] = stackpos;
        if (fi->args[i].size > 0 && fi->args[i].size < 4)
            stackpos += 4;             // shorts and chars are always extended
        else
            stackpos += fi->args[i].size;
        if (!fi->args[i].stack && fi->args[i].reg >= REG_EBX && fi->args[i].reg <= REG_EDI) {
            needsave[fi->args[i].reg] = true;           // callee-saved register
        }
    }

    // Pass 0.5, save any needed registers
    for (i = REG_EBX; i <= REG_EDI; i++) {
        if (needsave[i]) {
            asmr(as, I_PUSH, i);
            espoffset += 4;
        }
    }

    // Pass 1, copy parameters to stack of function we're calling in the right order
    for (i = fi->nargs - 1; i >= 0 ; --i) {
        if (!fi->args[i].stack)
            continue;
        for (j = (fi->args[i].size / 4 - 1) * 4; j >= 0; j -= 4) {
            asmrd(as, I_MOV, REG_EAX, REG_ESP, -1, 0, argoffsets[i] + j + espoffset, 4);
            asmr(as, I_PUSH, REG_EAX);
            espoffset += 4;
            fcallesp += 4;
        }
    }

    // Pass 2, load any register arguments
    for (i = 0; i < fi->nargs; i++) {
        if (fi->args[i].stack)
            continue;
        asmrd(as, I_MOV, fi->args[i].reg, REG_ESP, -1, 0, argoffsets[i] + espoffset, fi->args[i].size);
    }

    // Call the function
    asmi(as, I_CALL, addr);

    // Clean up after call
    if (fcallesp > 0 && !fi->stdcall) {
        asmri(as, I_ADD, REG_ESP, fcallesp);
    }

    // Pop any saved registers
    for (i = REG_EDI; i >= REG_EBX; i--) {
        if (needsave[i]) {
            asmr(as, I_POP, i);
            espoffset += 4;
        }
    }

    asmn(as, I_RET);

    if (!cgAsmFinish(as))
        return 0;

    return as->dest;
}
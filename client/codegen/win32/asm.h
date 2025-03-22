#pragma once
#include <stdbool.h>
#include "loader/win32/imgutil.h"
#include "disasm.h"
#include "minicrt.h"

// helpers for assembling code statefully

typedef struct AsmState {
    unsigned char* dest;       // destination buffer
    unsigned char* p;          // pointer to next instruction
    unsigned long remaining;   // space left in buffer
    t_asm ci;                  // next instruction to assemble
    char errortxt[TEXTLEN];    // text of last error
    bool error;                // error occured during run
    bool reserved;             // internal use
} AsmState;

// Reset an AsmState. If maxinstr is nonzero, a new codegen buffer is allocated
// to hold up to maxinstr instructions. If it is zero, no allocation is performed
// and you are responsible for setting dest and remaining before calling cgAsmInit.
bool cgAsmInit(AsmState* as, int maxinstr);

// Assembles the next instruction
bool cgAsm(AsmState* as);

bool cgAsmFinish(AsmState* as);

// Clears out the next instruction to defaults
static inline void cgAsmClear(AsmState* as)
{
    as->ci = empty_asm;
}

// Convenience macros for making t_asm struct filling prettier
// The _p  variants just prepare the instruction and don't assemble it into the buffer
#define asminst(as, i)       (as->ci.inst = i)
#define asmargc(as)          (as->ci.arg[as->ci.nargs])
#define asmarg(as)           (as->ci.arg[as->ci.nargs++])
#define asmn_p(as, i)        (cgAsmClear(as), asminst(as, i))
#define asmn(as, i)          (asmn_p(as, i), cgAsm(as))
#define asmr_p(as, i, reg)   (cgAsmClear(as), asminst(as, i), asmarg(as).base = reg)
#define asmr(as, i, reg)     (asmr_p(as, i, reg), cgAsm(as))
#define asmi_p(as, i, immed) (cgAsmClear(as), asminst(as, i), asmarg(as).addr = immed)
#define asmi(as, i, immed)   (asmi_p(as, i, immed), cgAsm(as))
#define asmrm_p(as, i, reg, address, sz) \
    (cgAsmClear(as),                     \
     asminst(as, i),                     \
     asmarg(as).base     = reg,          \
     asmargc(as).ptrsize = sz,           \
     asmarg(as).addr     = address)
#define asmrm(as, i, reg, address, sz) (asmrm_p(as, i, reg, address, sz), cgAsm(as))
#define asmmr_p(as, i, address, reg, sz) \
    (cgAsmClear(as),                     \
     asminst(as, i),                     \
     asmargc(as).ptrsize = sz,           \
     asmarg(as).addr     = address,      \
     asmarg(as).base     = reg)
#define asmmr(as, i, address, reg, sz) (asmmr_p(as, i, address, reg, sz), cgAsm(as))
#define asmri_p(as, i, reg, immed) \
    (cgAsmClear(as), asminst(as, i), asmarg(as).base = reg, asmarg(as).addr = immed)
#define asmri(as, i, reg, immed) (asmri_p(as, i, reg, immed), cgAsm(as))
#define asmrr_p(as, i, reg1, reg2) \
    (cgAsmClear(as), asminst(as, i), asmarg(as).base = reg1, asmarg(as).base = reg2)
#define asmrr(as, i, reg1, reg2) (asmrr_p(as, i, reg1, reg2), cgAsm(as))
#define asmrd_p(as, i, destreg, basereg, idxreg, mult, offset, sz) \
    (cgAsmClear(as),                                               \
     asminst(as, i),                                               \
     asmarg(as).base     = destreg,                                \
     asmargc(as).base    = basereg,                                \
     asmargc(as).idx     = idxreg,                                 \
     asmargc(as).scale   = mult,                                   \
     asmargc(as).ptrsize = sz,                                     \
     asmarg(as).disp     = offset)
#define asmrd(as, i, destreg, basereg, idxreg, mult, offset, sz) \
    (asmrd_p(as, i, destreg, basereg, idxreg, mult, offset, sz), cgAsm(as))
#define asmdr_p(as, i, basereg, idxreg, mult, offset, srcreg, sz) \
    (cgAsmClear(as),                                              \
     asminst(as, i),                                              \
     asmargc(as).base    = basereg,                               \
     asmargc(as).idx     = idxreg,                                \
     asmargc(as).scale   = mult,                                  \
     asmargc(as).ptrsize = sz,                                    \
     asmarg(as).disp = offset asmarg(as).base = srcreg)
#define asmdr(as, i, basereg, idxreg, mult, offset, srcreg, sz) \
    (asmdr_p(as, i, basereg, idxreg, mult, offset, srcreg, sz), cgAsm(as))

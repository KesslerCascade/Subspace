#include "disasmtrace.h"

#include "hook/module.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "disasm.h"

#define MAX_UNWIND 5

typedef struct DisasmTraceState {
    DisasmOp* op;
    addr_t p;
    // current skip
    int skipmin;
    int skipmax;
    t_arg cap[16];        // captured arguments
    bool outset[16];      // is outaddr set for this slot (it may still be zero value)
    addr_t outaddr[16];   // potential outputs
    addr_t labels[16];
} DisasmTraceState;

// Pushes the entire trace state to an unwind stack. This is so we can backtrack there upon failing
// to find a match and see if we should have skipped more instructions
static void pushUnwind(DisasmTraceState* dts, DisasmTraceState* unwind, int* nunwind, int isize)
{
    DisasmTraceState ndts = *dts;

    // if we can't skip anymore or are already maxed out on unwind points, just do nothing.
    // we should never get here if skipmin > 0, but check it anyway.
    if (dts->skipmin > 0 || dts->skipmax < 1 || *nunwind >= MAX_UNWIND)
        return;

    // we want the unwound state to start on the instruction *after* the one we saved it on
    ndts.p += isize;

    unwind[*nunwind] = ndts;
    (*nunwind)++;
}

// Restore the state to a previously saved unwind point
static bool popUnwind(DisasmTraceState* dts, DisasmTraceState* unwind, int* nunwind, int* isize)
{
    if (!(*nunwind > 0))
        return false;

    (*nunwind)--;
    *dts   = unwind[*nunwind];
    *isize = 0;   // we changed dts.p; don't let the loop advance it further
    return true;
}

static bool checkCandidate(addr_t base, DisasmTrace* trace, addr_t start)
{
    DisasmTraceState dts = { 0 };

    // unwind info for backtracking
    DisasmTraceState unwind[MAX_UNWIND];
    int nunwind = 0;

    dts.op = trace->ops;
    SegInfo code;
    SegInfo rdata;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return false;
    if (!getRDataSeg(base, &rdata))
        return false;

    dts.p     = start;
    bool fail = false;

    while (dts.p >= code.start && dts.p < code.end && dts.op->op != DT_FINISH) {
        if (dts.op->op == DT_SKIP) {
            dts.skipmin = dts.op->imin;
            dts.skipmax = dts.op->imax;
            ++dts.op;
            continue;
        } else if (dts.op->op == DT_LABEL) {
            // just mark the current address as a label
            if (dts.op->val > 0 && dts.op->val <= 16)
                dts.labels[dts.op->val - 1] = dts.p;
            ++dts.op;
            continue;
        } else if (dts.op->op == DT_GOTO) {
            // move directly to a saved label with no additional processing
            if (dts.op->val > 0 && dts.op->val <= 16) {
                addr_t addr = dts.labels[dts.op->val - 1];
                if (addr >= code.start && addr < code.end)
                    dts.p = addr;
            }
            ++dts.op;
        }

        // pseudo-ops are done, we're disassembling something!

        ulong isize = Disasm((uint8_t*)dts.p,
                             MIN(MAXCMDSIZE, code.end - dts.p),
                             dts.p,
                             &disasm,
                             DISASM_FILE);

        // if skipmin > 0 then we're skipping this instruction no matter what
        if (dts.skipmin == 0) {
            if (dts.op->op == DT_INST) {
                bool match = (dts.op->inst == disasm.inst);   // is this even the right instruction?

                // check arg filters
                for (int i = 0; match && i < 3; i++) {
                    if (dts.op->argf[i] == ARG_IGNORE)
                        continue;

                    if (dts.op->argsym[i]) {
                        // if we have a symbol, see if the address matches
                        if (disasm.arg[i].addr != _symAddr(base, dts.op->argsym[i]))
                            match = false;
                    } else if (dts.op->argstr[i]) {
                        // for a string we have to check every stringtable match because
                        // duplicate strings exist
                        AddrList* valid = findAllStrings(base, dts.op->argstr[i]);
                        bool strmatch   = false;
                        for (uint32_t j = 0; j < valid->num; j++) {
                            if (disasm.arg[i].addr == valid->addrs[j])
                                strmatch = true;
                        }
                        if (!strmatch)
                            match = false;
                    } else {
                        t_arg comp = dts.op->args[i];   // arg to compare to

                        // check for comparing against a captured argument
                        if (dts.op->argcap[i] & CT_MATCH)
                            comp = dts.cap[dts.op->argcap[i] & CT_ARG_MASK];

                        if (dts.op->argf[i] == ARG_MATCH) {
                            // just a basic register and displacement compare.
                            if (disasm.arg[i].base != comp.base || disasm.arg[i].idx != comp.idx ||
                                disasm.arg[i].disp != comp.disp)
                                match = false;
                        } else if (dts.op->argf[i] == ARG_REG) {
                            // check base register only
                            if (disasm.arg[i].base != comp.base)
                                match = false;
                        } else if (dts.op->argf[i] == ARG_ADDR) {
                            // check address (same as disp) only
                            if (disasm.arg[i].addr != comp.addr)
                                match = false;
                        }
                    }
                }

                if (match) {
                    // is this the instruction we're looking for?
                    if (dts.op->outip > 0) {
                        dts.outaddr[dts.op->outip - 1] = dts.p;
                        dts.outset[dts.op->outip - 1]  = true;
                    }

                    for (int i = 0; i < 3; i++) {
                        // capture any args from this match
                        if (dts.op->argcap[i] & CT_CAPTURE)
                            dts.cap[dts.op->argcap[i] & CT_ARG_MASK] = disasm.arg[i];
                        // and stage them for output if needed
                        if (dts.op->argout[i] > 0) {
                            dts.outaddr[dts.op->argout[i] - 1] = disasm.arg[i].addr;
                            dts.outset[dts.op->argout[i] - 1]  = true;
                        }
                    }

                    pushUnwind(&dts,
                               unwind,
                               &nunwind,
                               isize);   // save state in case it doesn't work out later
                    dts.skipmax = 0;     // stop any skip in progress
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // can't skip; so we backtrack or fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                }
            } else if (dts.op->op == DT_CALL || dts.op->op == DT_JMP) {
                bool instmatch = false;
                // these two are basically the same thing, just different instructions
                if (dts.op->op == DT_CALL) {
                    instmatch = (disasm.inst == I_CALL);
                } else {
                    // use command type to avoid having to check every jump instruction -- we want
                    // both regular and conditional jumps
                    uchar cmdtype = disasm.command->type & C_TYPEMASK;
                    instmatch     = (cmdtype == C_JMP || cmdtype == C_JMC);
                }

                if (instmatch && disasm.arg[0].addr >= code.start &&
                    disasm.arg[0].addr < code.end) {
                    // save unwind state before branching
                    pushUnwind(&dts, unwind, &nunwind, isize);

                    // follow the first argument, which we've verified is a destination within the
                    // same module
                    dts.p       = disasm.arg[0].addr;
                    dts.skipmax = 0;   // stop any skip in progress
                    isize       = 0;   // don't advance pointer
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // it wasn't a call and we're can't skip it so... just fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                }
            } else if (dts.op->op == DT_JMPTBL) {
                uchar cmdtype  = disasm.command->type & C_TYPEMASK;
                addr_t destptr = disasm.arg[0].addr + dts.op->val * 4;
                if ((cmdtype == C_JMP || cmdtype == C_JMC) && disasm.arg[0].scale == 4 &&
                    destptr >= rdata.start && destptr < rdata.end &&
                    *(uint32_t*)destptr >= code.start && *(uint32_t*)destptr < code.end) {
                    // save unwind state before branching
                    pushUnwind(&dts, unwind, &nunwind, isize);

                    // follow the first argument, which we've verified is a destination within the
                    // same module
                    dts.p       = *(uint32_t*)destptr;
                    dts.skipmax = 0;   // stop any skip in progress
                    isize       = 0;   // don't advance pointer
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // it wasn't a valid jump table and we're can't skip it so... just fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                }
            }
        }

        dts.skipmin = MAX(dts.skipmin - 1, 0);
        dts.skipmax = MAX(dts.skipmax - 1, 0);
        dts.p += isize;
    }

    // check if we made it all the way to the end of the sequence
    if (dts.op->op != DT_FINISH)
        return false;

    // save outputs
    for (int i = 0; i < 16; i++) {
        if (dts.outset[i] && trace->out[i]) {
            // set the symbol to resolved since we know its value
            trace->out[i]->addr     = dts.outaddr[i];
            trace->out[i]->resolved = true;
        }
    }

    return true;
}

bool disasmTrace(addr_t base, DisasmTrace* trace)
{
    bool success   = false;
    addr_t saddr   = 0;
    ModuleInfo* mi = moduleInfo(base);
    AddrList* al;
    uint32_t i;

    if (trace->csym) {
        saddr = _symAddr(base, trace->csym);
    }
    if (!saddr && trace->cstr) {
        saddr = findString(base, trace->cstr);
    }
    if (!saddr)   // nowhere to search :(
        return false;

    switch (trace->c) {
    case DTRACE_ADDR:
        return checkCandidate(base, trace, saddr);
    case DTRACE_REFS:
        al = hashtbl_get(&mi->ptrrefhash, saddr);
        break;
    case DTRACE_STRREFS:
        al = hashtbl_get(&mi->stringrefhash, saddr);
        break;
    case DTRACE_CALLS:
        al = hashtbl_get(&mi->funccallhash, saddr);
        break;
    }

    if (al) {
        for (i = 0; i < al->num; i++) {
            if (checkCandidate(base, trace, al->addrs[i]))
                return true;
        }
    }

    return false;
}

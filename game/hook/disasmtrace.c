#include "disasmtrace.h"

#include "hook/module.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "log/gamelog.h"
#include "disasm.h"

#if defined(_DEBUG) && defined(WIN32)
#include <windows.h>
#endif

#define MAX_UNWIND 16

typedef struct DisasmTraceState {
    DisasmOp* op;
    addr_t p;
    // current skip
    int skipmin;
    int skipmax;
    int flow;
    int curlabel;
    t_arg cap[16];        // captured arguments
    bool outset[16];      // is outaddr set for this slot (it may still be zero value)
    addr_t outaddr[16];   // potential outputs
    addr_t labels[16];
} DisasmTraceState;

#if defined(_DEBUG) && defined(WIN32)
#include <debugapi.h>
// Uncomment and set _debug_trace ot trigger a breakpoint when a specific trace is about to be run
// #define DEBUG_TRACE WorldManager_UpdateLocation_trace
#ifdef DEBUG_TRACE
extern DisasmTrace DEBUG_TRACE;
static DisasmTrace* _debug_trace = &DEBUG_TRACE;
#else
static DisasmTrace* _debug_trace = NULL;
#endif
#endif

// Pushes the entire trace state to an unwind stack. This is so we can backtrack there upon failing
// to find a match and see if we should have skipped more instructions
static void pushUnwind(DisasmTraceState* dts, DisasmTraceState* unwind, int* nunwind, int isize)
{
    DisasmTraceState ndts = *dts;

    // if we can't skip anymore or are already maxed out on unwind points, just do nothing.
    if (dts->skipmax < 1 || *nunwind >= MAX_UNWIND) {
#ifdef _DEBUG
        if (*nunwind >= MAX_UNWIND)
            logStr(Verbose, _S"Unwind stack is full!");
#endif
        return;
    }

    // we want the unwound state to start on the instruction *after* the one we saved it on,
    // simulating a skip
    ndts.p += isize;
    // ndts.skipmin = MAX(ndts.skipmin - 1, 0);
    // ndts.skipmax = MAX(ndts.skipmax - 1, 0);

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
    DisasmTraceState* unwind;
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
    unwind    = xaAlloc(sizeof(DisasmTraceState) * MAX_UNWIND, XA_Zero);

#if defined(_DEBUG) && defined(WIN32)
    if (trace == _debug_trace && IsDebuggerPresent()) {
        __debugbreak();
    }
#endif

    while (dts.p >= code.start && dts.p < code.end && dts.op->op != DT_FINISH) {
        bool skip = false;

        // at any time we can save the current IP to an output
        if (dts.op->outip > 0) {
            dts.outaddr[dts.op->outip - 1] = dts.p;
            dts.outset[dts.op->outip - 1]  = true;
        }

        if (dts.op->op == DT_SKIP) {
            dts.skipmin = dts.op->imin;
            dts.skipmax = dts.op->imax;
            dts.flow    = dts.op->flow;
            ++dts.op;
            continue;
        } else if (dts.op->op == DT_LABEL) {
            // set this as current label; it will actually be saved when there's a match
            if (dts.op->val > 0 && dts.op->val <= 16)
                dts.curlabel = dts.op->val;
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
            continue;
        } else if (dts.op->op == DT_NOUNWIND) {
            // clear unwind stack; this pseudo-op is used when we're 100% certain that
            // the trace is correct at that point; to keep that stack from growing too much
            nunwind = 0;
            ++dts.op;
            continue;
        } else if (dts.op->op == DT_NOOP) {
            ++dts.op;
            continue;
        }

        // pseudo-ops are done, we're disassembling something!

        ulong isize = Disasm((uint8_t*)dts.p,
                             min(MAXCMDSIZE, code.end - dts.p),
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
                        AddrList* valid = findAllStrings(base, (strref)dts.op->argstr[i]);
                        bool strmatch   = false;
                        for (uint32_t j = 0; valid && j < saSize(*valid); j++) {
                            if (disasm.arg[i].addr == valid->a[j])
                                strmatch = true;
                        }
                        if (!strmatch)
                            match = false;
                    } else {
                        t_arg comp = dts.op->args[i];   // arg to compare to

                        // check for comparing against a captured argument
                        if (dts.op->argcap[i] & DT_MATCH)
                            comp = dts.cap[dts.op->argcap[i] & DT_CAPTURE_ARG_MASK];

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
                        } else if (dts.op->argf[i] == ARG_DEREF) {
                            // check address (same as disp) only
                            bool inrdata = (disasm.arg[i].addr >= rdata.start) &&
                                (disasm.arg[i].addr + disasm.arg[i].ptrsize < rdata.end);
                            if (disasm.arg[i].ptrsize > 0 && inrdata) {
                                if (memcmp((uint8_t*)disasm.arg[i].addr,
                                           &comp.addr,
                                           disasm.arg[i].ptrsize) != 0)
                                    match = false;
                            } else {
                                match = false;
                            }
                        } else if (dts.op->argf[i] == ARG_PTRSIZE) {
                            // check pointer size only
                            if (disasm.arg[i].ptrsize != comp.ptrsize)
                                match = false;
                        }
                    }
                }

                if (match) {
                    pushUnwind(&dts,
                               unwind,
                               &nunwind,
                               isize);   // save state in case it doesn't work out later

                    for (int i = 0; i < 3; i++) {
                        // capture any args from this match
                        if (dts.op->argcap[i] & DT_CAPTURE)
                            dts.cap[dts.op->argcap[i] & DT_CAPTURE_ARG_MASK] = disasm.arg[i];
                        // and stage them for output if needed
                        if (dts.op->argout[i] > 0) {
                            dts.outaddr[dts.op->argout[i] - 1] = disasm.arg[i].addr;
                            dts.outset[dts.op->argout[i] - 1]  = true;
                        }
                    }

                    // save label if set
                    if (dts.curlabel) {
                        dts.labels[dts.curlabel - 1] = dts.p;
                        dts.curlabel                 = 0;
                    }

                    dts.skipmax = 0;   // stop any skip in progress
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // can't skip; so we backtrack or fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                } else {
                    skip = true;
                }
            } else if (dts.op->op == DT_CALL || dts.op->op == DT_JMP) {
                bool instmatch = false;
                // these two are basically the same thing, just different instructions
                if (dts.op->op == DT_CALL) {
                    instmatch = (disasm.inst == I_CALL);
                } else {
                    // op may want to check for a specific jump instruction
                    if (dts.op->inst != I_NONE) {
                        instmatch = (dts.op->inst == disasm.inst);
                    } else {
                        // use command type to avoid having to check every jump instruction --
                        // we want both regular and conditional jumps
                        uchar cmdtype = disasm.command->type & C_TYPEMASK;
                        instmatch     = (cmdtype == C_JMP || cmdtype == C_JMC);
                    }
                }

                if (instmatch && disasm.arg[0].addr >= code.start &&
                    disasm.arg[0].addr < code.end) {
                    // save unwind state before branching
                    pushUnwind(&dts, unwind, &nunwind, isize);

                    // save label if set
                    if (dts.curlabel) {
                        dts.labels[dts.curlabel - 1] = dts.p;
                        dts.curlabel                 = 0;
                    }

                    // follow the first argument, which we've verified is a destination within
                    // the same module
                    dts.p       = disasm.arg[0].addr;
                    dts.skipmax = 0;   // stop any skip in progress
                    isize       = 0;   // don't advance pointer
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // it wasn't a call and we're can't skip it so... just fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                } else {
                    skip = true;
                }
            } else if (dts.op->op == DT_JMPTBL) {
                uchar cmdtype  = disasm.command->type & C_TYPEMASK;
                addr_t destptr = disasm.arg[0].addr + dts.op->val * 4;
                if ((cmdtype == C_JMP || cmdtype == C_JMC) && disasm.arg[0].scale == 4 &&
                    destptr >= rdata.start && destptr < rdata.end &&
                    *(uint32_t*)destptr >= code.start && *(uint32_t*)destptr < code.end) {
                    // save unwind state before branching
                    pushUnwind(&dts, unwind, &nunwind, isize);

                    // save label if set
                    if (dts.curlabel) {
                        dts.labels[dts.curlabel - 1] = dts.p;
                        dts.curlabel                 = 0;
                    }

                    // follow the first argument, which we've verified is a destination within
                    // the same module
                    dts.p       = *(uint32_t*)destptr;
                    dts.skipmax = 0;   // stop any skip in progress
                    isize       = 0;   // don't advance pointer
                    ++dts.op;
                } else if (dts.skipmax == 0) {
                    // it wasn't a valid jump table and we're can't skip it so... just fail
                    if (!popUnwind(&dts, unwind, &nunwind, &isize))
                        break;
                } else {
                    skip = true;
                }
            }
        } else {
            skip = true;
        }

        if (skip) {
            // if we're skipping, try to follow control flow if requested
            uchar cmdtype  = disasm.command->type & C_TYPEMASK;
            bool instmatch = false;

            if (dts.flow == DT_FLOW_JMP_ALL || dts.flow == DT_FLOW_JMP_BOTH)
                instmatch = (cmdtype == C_JMP || cmdtype == C_JMC);
            else if (dts.flow == DT_FLOW_JMP_UNCOND)
                instmatch = (cmdtype == C_JMP);

            if (instmatch && disasm.arg[0].addr >= code.start && disasm.arg[0].addr < code.end) {
                // follow the first argument, which we've verified is a destination within the
                // same module.
                // Do NOT reset skip, because we're following along during a skip.

                if (dts.flow == DT_FLOW_JMP_BOTH) {
                    // if we're following both branches, create an unwind point which will
                    // resume right after this jump
                    pushUnwind(&dts, unwind, &nunwind, isize);
                }

                dts.p = disasm.arg[0].addr;
                isize = 0;   // don't advance pointer
            }
        }

        dts.skipmin = max(dts.skipmin - 1, 0);
        dts.skipmax = max(dts.skipmax - 1, 0);
        dts.p += isize;
    }

    xaFree(unwind);

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

static void addList(sa_uintptr* tocheck, AddrList* al)
{
    if (!al)
        return;

    for (int i = 0; al && i < saSize(*al); i++) {
        saPush(tocheck, uintptr, al->a[i]);
    }
}

bool disasmTrace(addr_t base, DisasmTrace* trace)
{
    bool success   = false;
    addr_t saddr   = 0;
    ModuleInfo* mi = moduleInfo(base);
    sa_uintptr tocheck;
    uint32_t i;

    saInit(&tocheck, uintptr, 4);

    if (trace->csym)
        saddr = _symAddr(base, trace->csym);
    }
    if (!saddr && trace->cstr) {
        saddr = findString(base, (strref)trace->cstr);
    }
    if (!saddr && trace->c != DTRACE_FUNCS)   // nowhere to search :(
        return false;

    switch (trace->c) {
    case DTRACE_ADDR:
        if (saddr)
            saPush(&tocheck, uintptr, saddr);
        break;
    case DTRACE_REFS:
        if (saddr)
            addList(&tocheck, addrListFindByPtr(mi->ptrrefhash, saddr));
        break;
    case DTRACE_STRREFS: {
        AddrList* sl = findAllStrings(base, (strref)trace->cstr);
        for (i = 0; sl && i < saSize(*sl); i++) {
            addList(&tocheck, addrListFindByPtr(mi->stringrefhash, sl->a[i]));
        }
    } break;
    case DTRACE_CALLS:
        if (saddr)
            addList(&tocheck, addrListFindByPtr(mi->funccallhash, saddr));
        break;
    case DTRACE_FUNCS:
        addList(&tocheck, &mi->funclist);
        break;
    }

    for (i = 0; i < saSize(tocheck); i++) {
        addr_t checkaddr = 0;
        // if (trace == &ShipManager_DamageBeam_trace) {
        //  log_fmt(LOG_Verbose, "  Checking candidate at 0x%08x", al->a[i] - base);
        //}
        if (trace->mod == DTRACE_MOD_FUNCSTART) {
            // this trace wants to look at the start of the function containing the reference
            int checkidx = saFind(mi->funclist, uintptr, tocheck.a[i], SA_Inexact);
            if (checkidx != -1 && checkidx > 0)
                checkaddr = mi->funclist.a[checkidx - 1];   // function start address
            // if (trace == &ShipManager_DamageBeam_trace) {
            //  log_fmt(LOG_Verbose, "  Function start at 0x%08x", checkaddr - base);
            //}
        } else {
            checkaddr = tocheck.a[i];
        }

        if (checkaddr && checkCandidate(base, trace, checkaddr))
            return true;
    }
    saDestroy(&tocheck);

    return false;
}

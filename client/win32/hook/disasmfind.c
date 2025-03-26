#include "disasmfind.h"

#include "hook/module.h"
#include "hook/string.h"
#include "hook/symbol.h"
#include "disasm.h"

typedef struct DisasmFindState {
    // current skip
    int skipmin;
    int skipmax;
    t_arg cap[16];   // captured arguments
} DisasmFindState;

static addr_t checkCandidate(addr_t base, DisasmFind* find, addr_t start)
{
    DisasmFindState df = { 0 };
    DisasmOp* op       = find->ops;
    SegInfo code;
    SegInfo rdata;
    t_disasm disasm;

    if (!getCodeSeg(base, &code))
        return 0;
    if (!getRDataSeg(base, &rdata))
        return 0;

    addr_t p   = start;
    addr_t ret = 0;

    while (p >= code.start && p < code.end && op->type != DISASM_FINISH) {
        if (op->type == DISASM_SKIP) {
            df.skipmin = op->imin;
            df.skipmax = op->imax;
            ++op;
            continue;
        }

        // pseudo-ops are done, we're disassembling something!

        ulong isize = Disasm((uint8_t*)p, MIN(MAXCMDSIZE, code.end - p), p, &disasm, DISASM_FILE);

        // if skipmin > 0 then we're skipping this instruction no matter what
        if (df.skipmin == 0) {
            if (op->type == DISASM_INSTR) {
                bool match = (op->inst == disasm.inst);   // is this even the right instruction?

                // check arg filters
                for (int i = 0; match && i < 3; i++) {
                    if (op->argfilter[i] == ARG_IGNORE)
                        continue;

                    if (op->argsym[i]) {
                        // if we have a symbol, see if the address matches
                        uint32_t matchval = (op->argfilter[i] == ARG_DISP) ?
                            disasm.arg[i].disp :
                            disasm.arg[i].addr;
                        if (matchval != _symResolve(base, op->argsym[i]))
                            match = false;
                    } else if (op->argstr[i]) {
                        // for a string we have to check every stringtable match because
                        // duplicate strings exist
                        AddrList* valid = findAllStrings(base, op->argstr[i]);
                        bool strmatch   = false;
                        for (uint32_t j = 0; j < valid->num; j++) {
                            if (disasm.arg[i].addr == valid->addrs[j])
                                strmatch = true;
                        }
                        if (!strmatch)
                            match = false;
                    } else {
                        t_arg comp = op->args[i];   // arg to compare to

                        // check for comparing against a captured argument
                        if (op->argcap[i] & CT_MATCH)
                            comp = df.cap[op->argcap[i] & CT_ARG_MASK];

                        if (op->argfilter[i] == ARG_MATCH) {
                            // just a basic register and displacement compare.
                            if (disasm.arg[i].base != comp.base || disasm.arg[i].idx != comp.idx ||
                                disasm.arg[i].disp != comp.disp)
                                match = false;
                        } else if (op->argfilter[i] == ARG_REG) {
                            // check base register only
                            if (disasm.arg[i].base != comp.base)
                                match = false;
                        }
                    }
                }

                if (match) {
                    // is this the instruction we're looking for?
                    if (op->mark == MARK_INSTR) {
                        ret = p;
                    } else if (op->mark & MARK_ARG) {
                        ret = disasm.arg[op->mark & 3].addr;
                    }

                    // capture any args from this match
                    for (int i = 0; i < 3; i++) {
                        if (op->argcap[i] & CT_CAPTURE)
                            df.cap[op->argcap[i] & CT_ARG_MASK] = disasm.arg[i];
                    }

                    df.skipmax = 0;   // stop any skip in progress
                    ++op;
                } else if (df.skipmax == 0) {
                    // can't skip; so we fail
                    break;
                }
            } else if (op->type == DISASM_CALL || op->type == DISASM_JMP) {
                bool instmatch = false;
                // these two are basically the same thing, just different instructions
                if (op->type == DISASM_CALL) {
                    instmatch = (disasm.inst == I_CALL);
                } else {
                    // use command type to avoid having to check every jump instruction -- we want
                    // both regular and conditional jumps
                    uchar cmdtype = disasm.command->type & C_TYPEMASK;
                    instmatch     = (cmdtype == C_JMP || cmdtype == C_JMC);
                }

                if (instmatch && disasm.arg[0].addr >= code.start &&
                    disasm.arg[0].addr < code.end) {
                    // follow the first argument, which we've verified is a destination within the
                    // same module
                    p          = disasm.arg[0].addr;
                    df.skipmax = 0;   // stop any skip in progress
                    isize      = 0;   // don't advance pointer
                    ++op;
                } else if (df.skipmax == 0) {
                    // it wasn't a call and we're can't skip it so... just fail
                    break;
                }
            } else if (op->type == DISASM_JMPTBL) {
                uchar cmdtype  = disasm.command->type & C_TYPEMASK;
                addr_t destptr = disasm.arg[0].addr + op->val * 4;
                if ((cmdtype == C_JMP || cmdtype == C_JMC) && disasm.arg[0].scale == 4 &&
                    destptr >= rdata.start && destptr < rdata.end &&
                    *(uint32_t*)destptr >= code.start && *(uint32_t*)destptr < code.end) {
                    // follow the first argument, which we've verified is a destination within the
                    // same module
                    p          = *(uint32_t*)destptr;
                    df.skipmax = 0;   // stop any skip in progress
                    isize      = 0;   // don't advance pointer
                    ++op;
                } else if (df.skipmax == 0) {
                    // it wasn't a valid jump table and we're can't skip it so... just fail
                    break;
                }
            }
        }

        df.skipmin = MAX(df.skipmin - 1, 0);
        df.skipmax = MAX(df.skipmax - 1, 0);
        p += isize;
    }

    // check if we made it all the way to the end of the sequence
    if (op->type != DISASM_FINISH)
        ret = 0;
    return ret;
}

addr_t findByDisasm(addr_t base, DisasmFind* find)
{
    addr_t saddr = 0, ret = 0;
    ModuleInfo* mi = moduleInfo(base);
    AddrList* al;
    uint32_t i;

    if (find->sym) {
        saddr = _symResolve(base, find->sym);
    }
    if (!saddr && find->cstr) {
        saddr = findString(base, find->cstr);
    }
    if (!saddr)   // nowhere to search :(
        return 0;

    switch (find->candidates) {
    case DISASM_SEARCH_ADDR:
        return checkCandidate(base, find, saddr);
    case DISASM_SEARCH_REF:
        al = hashtbl_get(&mi->ptrrefhash, saddr);
        break;
    case DISASM_SEARCH_STRREF:
        al = hashtbl_get(&mi->stringrefhash, saddr);
        break;
    case DISASM_SEARCH_CALL:
        al = hashtbl_get(&mi->funccallhash, saddr);
        break;
    }

    if (!ret && al) {
        for (i = 0; i < al->num; i++) {
            ret = checkCandidate(base, find, al->addrs[i]);
            if (ret)
                return ret;
        }
    }

    return ret;
}

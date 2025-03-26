#pragma once

#include "loader/loader.h"
#include "disasm.h"

typedef struct Symbol Symbol;

typedef enum DisasmOpType {
    DISASM_INSTR = 0,   // Instruction to match
    DISASM_SKIP,        // Instructions to skip
    DISASM_JMP,         // follow a jump or conditional jump instead of disassembling it
    DISASM_CALL,        // follow a call instead of disassembling it (there is no return logic, it's
                        // treated like a jump)
    DISASM_JMPTBL,      // follow entry number [val] of a jump table, i.e. for a switch statement
    DISASM_FINISH,      // end of op list
} DisasmOpType;

typedef enum DisasmArgOp {
    ARG_IGNORE,   // don't care what's in this argument
    ARG_MATCH,    // must exactly match what's in the args
    ARG_REG,      // match register only, ignoring disp
    ARG_DISP,     // match disp value only
} DisasmArgOp;

typedef enum DisasmMarkType {
    MARK_NONE     = 0,
    MARK_INSTR    = 1,      // current instruction
    MARK_ARG      = 0x10,   // Mask out low bits to get which args
    MARK_ARG1     = 0x10,
    MARK_ARG2     = 0x11,
    MARK_ARG3     = 0x12,
    MARK_ARG_MASK = 0x0f
} DisasmMarkType;

typedef enum DisarpArgCaptureType {
    CT_CAPTURE   = 0x10,   // capture the the argument from disassembly to use later - OR with the
                           // capture number
    CT_CAPTURE0  = CT_CAPTURE | 0,
    CT_CAPTURE1  = CT_CAPTURE | 1,
    CT_CAPTURE2  = CT_CAPTURE | 2,
    CT_CAPTURE3  = CT_CAPTURE | 3,
    CT_CAPTURE4  = CT_CAPTURE | 4,
    CT_CAPTURE5  = CT_CAPTURE | 5,
    CT_CAPTURE6  = CT_CAPTURE | 6,
    CT_CAPTURE7  = CT_CAPTURE | 7,
    CT_CAPTURE8  = CT_CAPTURE | 8,
    CT_CAPTURE9  = CT_CAPTURE | 9,
    CT_CAPTURE10 = CT_CAPTURE | 0xa,
    CT_CAPTURE11 = CT_CAPTURE | 0xb,
    CT_CAPTURE12 = CT_CAPTURE | 0xc,
    CT_CAPTURE13 = CT_CAPTURE | 0xd,
    CT_CAPTURE14 = CT_CAPTURE | 0xe,
    CT_CAPTURE15 = CT_CAPTURE | 0xf,
    CT_MATCH     = 0x20,   // use a previously captured argument in place of the op's arg to match
    CT_MATCH0    = CT_MATCH | 0,
    CT_MATCH1    = CT_MATCH | 1,
    CT_MATCH2    = CT_MATCH | 2,
    CT_MATCH3    = CT_MATCH | 3,
    CT_MATCH4    = CT_MATCH | 4,
    CT_MATCH5    = CT_MATCH | 5,
    CT_MATCH6    = CT_MATCH | 6,
    CT_MATCH7    = CT_MATCH | 7,
    CT_MATCH8    = CT_MATCH | 8,
    CT_MATCH9    = CT_MATCH | 9,
    CT_MATCH10   = CT_MATCH | 0xa,
    CT_MATCH11   = CT_MATCH | 0xb,
    CT_MATCH12   = CT_MATCH | 0xc,
    CT_MATCH13   = CT_MATCH | 0xd,
    CT_MATCH14   = CT_MATCH | 0xe,
    CT_MATCH15   = CT_MATCH | 0xf,
    CT_ARG_MASK  = 0xf,
} DisasmArgCaptureType;

typedef struct DisasmOp {
    DisasmOpType type;     // dissembly operation

    int imin, imax;        // min and max number of instructions to skip (before the next match) for
                           // DISASM_SKIP

    e_inst inst;           // instruction to match for DISASM_INSTR
    uint32_t val;          // misc value used for certain ops
    DisasmMarkType mark;   // mark this instruction or an argument as the result
    DisasmArgOp argfilter[3];         // argument filter
    const char* argstr[3];            // if set, overrides the argument with a stringtable address
    Symbol* argsym[3];                // if set, overrides the argument with a symbol address
    DisasmArgCaptureType argcap[3];   // for capturing arguments and using them in a later match
    t_arg args[3];                    // data for arguments
} DisasmOp;

typedef enum DisasmCandidateFindType {
    DISASM_SEARCH_ADDR = 1,   // Check a single fixed address
    DISASM_SEARCH_REF,        // Check all instructions that reference a particular pointer
    DISASM_SEARCH_STRREF,     // Check all instructions that reference a particular string
    DISASM_SEARCH_CALL,       // Check all function calls to a particular address
} DisasmCandidateFindType;

typedef struct DisasmFind {
    DisasmCandidateFindType candidates;
    addr_t addr;        // raw address for candidate search
    const char* cstr;   // string for candidate search, takes precedence over addr
    Symbol* sym;        // symbol for candidate search, takes precedence over cstr and addr
    DisasmOp ops[];
} DisasmFind;

addr_t findByDisasm(addr_t base, DisasmFind* find);

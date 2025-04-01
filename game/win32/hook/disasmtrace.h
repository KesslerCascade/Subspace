#pragma once

#include "loader/loader.h"
#include "disasm.h"

typedef struct Symbol Symbol;

typedef enum DisasmOpType {
    DT_INST = 0,   // Instruction to match
    DT_NOOP,       // Do nothing (but maybe save EIP)
    DT_SKIP,       // Instructions to skip
    DT_JMP,        // follow a jump or conditional jump instead of disassembling it
    DT_CALL,       // follow a call instead of disassembling it (there is no return logic, it's
                   // treated like a jump)
    DT_JMPTBL,     // follow entry number [val] of a jump table, i.e. for a switch statement
    DT_LABEL,      // save the current EIP as label [val] (1-16)
    DT_GOTO,       // move tracing to label [val] (1-16); useful for tracing both parts of a branch
    DT_FINISH,   // if hit, indicates trace is successful and no other candidates should be checked
} DisasmOpType;

typedef enum DisasmArgOp {
    ARG_IGNORE,   // don't care what's in this argument
    ARG_MATCH,    // must exactly match what's in the args
    ARG_REG,      // match register only, ignoring disp
    ARG_ADDR,     // match addr / disp value only
} DisasmArgOp;

typedef enum DisasmOutput {
    DT_OUT_NONE  = 0,
    DT_OUT_SYM1  = 1,
    DT_OUT_SYM2  = 2,
    DT_OUT_SYM3  = 3,
    DT_OUT_SYM4  = 4,
    DT_OUT_SYM5  = 5,
    DT_OUT_SYM6  = 6,
    DT_OUT_SYM7  = 7,
    DT_OUT_SYM8  = 8,
    DT_OUT_SYM9  = 9,
    DT_OUT_SYM10 = 10,
    DT_OUT_SYM11 = 11,
    DT_OUT_SYM12 = 12,
    DT_OUT_SYM13 = 13,
    DT_OUT_SYM14 = 14,
    DT_OUT_SYM15 = 15,
    DT_OUT_SYM16 = 16
} DisasmOutput;

typedef enum DisarpArgCaptureType {
    CT_CAPTURE   = 0x10,   // capture the the argument from disassembly to use later - OR with the
                           // capture number
    CT_CAPTURE1  = CT_CAPTURE | 0,
    CT_CAPTURE2  = CT_CAPTURE | 1,
    CT_CAPTURE3  = CT_CAPTURE | 2,
    CT_CAPTURE4  = CT_CAPTURE | 3,
    CT_CAPTURE5  = CT_CAPTURE | 4,
    CT_CAPTURE6  = CT_CAPTURE | 5,
    CT_CAPTURE7  = CT_CAPTURE | 6,
    CT_CAPTURE8  = CT_CAPTURE | 7,
    CT_CAPTURE9  = CT_CAPTURE | 8,
    CT_CAPTURE10 = CT_CAPTURE | 9,
    CT_CAPTURE11 = CT_CAPTURE | 0xa,
    CT_CAPTURE12 = CT_CAPTURE | 0xb,
    CT_CAPTURE13 = CT_CAPTURE | 0xc,
    CT_CAPTURE14 = CT_CAPTURE | 0xd,
    CT_CAPTURE15 = CT_CAPTURE | 0xe,
    CT_CAPTURE16 = CT_CAPTURE | 0xf,
    CT_MATCH     = 0x20,   // use a previously captured argument in place of the op's arg to match
    CT_MATCH1    = CT_MATCH | 0,
    CT_MATCH2    = CT_MATCH | 1,
    CT_MATCH3    = CT_MATCH | 2,
    CT_MATCH4    = CT_MATCH | 3,
    CT_MATCH5    = CT_MATCH | 4,
    CT_MATCH6    = CT_MATCH | 5,
    CT_MATCH7    = CT_MATCH | 6,
    CT_MATCH8    = CT_MATCH | 7,
    CT_MATCH9    = CT_MATCH | 8,
    CT_MATCH10   = CT_MATCH | 9,
    CT_MATCH11   = CT_MATCH | 0xa,
    CT_MATCH12   = CT_MATCH | 0xb,
    CT_MATCH13   = CT_MATCH | 0xc,
    CT_MATCH14   = CT_MATCH | 0xd,
    CT_MATCH15   = CT_MATCH | 0xe,
    CT_MATCH16   = CT_MATCH | 0xf,
    CT_ARG_MASK  = 0xf,
} DisasmArgCaptureType;

typedef struct DisasmOp {
    e_inst inst;           // instruction to match for DISASM_INST
    DisasmOpType op;       // dissembly operation

    int imin, imax;        // min and max number of instructions to skip (before the next match) for
                           // DISASM_SKIP

    uint32_t val;          // misc value used for certain ops
    DisasmArgOp argf[3];   // argument filter
    t_arg args[3];         // data for arguments
    const char* argstr[3];            // if set, overrides the argument with a stringtable address
    Symbol* argsym[3];                // if set, overrides the argument with a symbol address
    DisasmArgCaptureType argcap[3];   // for capturing arguments and using them in a later match
    DisasmOutput argout[3];   // Outputs the addr/disp from the given argument to the symbol in the
                              // associated slot

    DisasmOutput outip;       // If set, outputs the current EIP to the symbol in the given slot
} DisasmOp;

typedef enum DisasmCandidateTraceType {
    DTRACE_ADDR = 1,   // Trace starting a single fixed address
    DTRACE_REFS,       // Check all instructions that reference a particular pointer
    DTRACE_STRREFS,    // Check all instructions that reference a particular string
    DTRACE_CALLS,      // Check all function calls to a particular address
} DisasmCandidateTraceType;

typedef struct DisasmTrace {
    DisasmCandidateTraceType c;   // How to find candidates for this trace
    addr_t addr;                  // raw address for candidate search
    const char* cstr;             // string for candidate search, takes precedence over addr
    Symbol* csym;      // symbol for candidate search, takes precedence over cstr and addr
    Symbol* out[16];   // Up to 16 symbols can be output from the trace
    DisasmOp ops[];
} DisasmTrace;

#define DT_OP(name) .op = DT_##name

bool disasmTrace(addr_t base, DisasmTrace* trace);

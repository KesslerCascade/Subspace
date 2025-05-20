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
    ARG_IGNORE,    // don't care what's in this argument
    ARG_MATCH,     // must exactly match what's in the args
    ARG_REG,       // match register only, ignoring disp
    ARG_ADDR,      // match addr / disp value only
    ARG_PTRSIZE,   // match pointer size only
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

typedef enum DisarmArgCaptureType {
    DT_CAPTURE   = 0x10,   // capture the the argument from disassembly to use later - OR with the
                           // capture number
    DT_CAPTURE1  = DT_CAPTURE | 0,
    DT_CAPTURE2  = DT_CAPTURE | 1,
    DT_CAPTURE3  = DT_CAPTURE | 2,
    DT_CAPTURE4  = DT_CAPTURE | 3,
    DT_CAPTURE5  = DT_CAPTURE | 4,
    DT_CAPTURE6  = DT_CAPTURE | 5,
    DT_CAPTURE7  = DT_CAPTURE | 6,
    DT_CAPTURE8  = DT_CAPTURE | 7,
    DT_CAPTURE9  = DT_CAPTURE | 8,
    DT_CAPTURE10 = DT_CAPTURE | 9,
    DT_CAPTURE11 = DT_CAPTURE | 0xa,
    DT_CAPTURE12 = DT_CAPTURE | 0xb,
    DT_CAPTURE13 = DT_CAPTURE | 0xc,
    DT_CAPTURE14 = DT_CAPTURE | 0xd,
    DT_CAPTURE15 = DT_CAPTURE | 0xe,
    DT_CAPTURE16 = DT_CAPTURE | 0xf,
    DT_MATCH     = 0x20,   // use a previously captured argument in place of the op's arg to match
    DT_MATCH1    = DT_MATCH | 0,
    DT_MATCH2    = DT_MATCH | 1,
    DT_MATCH3    = DT_MATCH | 2,
    DT_MATCH4    = DT_MATCH | 3,
    DT_MATCH5    = DT_MATCH | 4,
    DT_MATCH6    = DT_MATCH | 5,
    DT_MATCH7    = DT_MATCH | 6,
    DT_MATCH8    = DT_MATCH | 7,
    DT_MATCH9    = DT_MATCH | 8,
    DT_MATCH10   = DT_MATCH | 9,
    DT_MATCH11   = DT_MATCH | 0xa,
    DT_MATCH12   = DT_MATCH | 0xb,
    DT_MATCH13   = DT_MATCH | 0xc,
    DT_MATCH14   = DT_MATCH | 0xd,
    DT_MATCH15   = DT_MATCH | 0xe,
    DT_MATCH16   = DT_MATCH | 0xf,
    DT_CAPTURE_ARG_MASK = 0xf,
} DisasmArgCaptureType;

typedef enum DisasmFlowType {
    DT_FLOW_NONE = 0,     // ignore program flow entirely
    DT_FLOW_JMP_ALL,      // follow all jumps
    DT_FLOW_JMP_UNCOND,   // follow only unconditional jumps
    DT_FLOW_JMP_BOTH,     // follow flow; branch on conditional jumps and follow both
} DisasmFlowType;

typedef struct DisasmOp {
    e_inst inst;           // instruction to match for DISASM_INST
    DisasmOpType op;       // dissembly operation

    int imin, imax;        // min and max number of instructions to skip (before the next match) for
                           // DISASM_SKIP
    int flow;              // the type of program flow following to do during skips

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

typedef enum DisasmCandidateTraceModifier {
    DTRACE_MOD_FUNCSTART = 0x0001,   // Start tracing at the beginning of the function
} DisasmCandidateTraceModifier;

typedef struct DisasmTrace {
    DisasmCandidateTraceType c;         // How to find candidates for this trace
    DisasmCandidateTraceModifier mod;   // modifiers to candidate search
    addr_t addr;                        // raw address for candidate search
    const char* cstr;                   // string for candidate search, takes precedence over addr
    Symbol* csym;      // symbol for candidate search, takes precedence over cstr and addr
    Symbol* out[16];   // Up to 16 symbols can be output from the trace
    DisasmOp ops[];
} DisasmTrace;

#define DT_OP(name) .op = DT_##name

bool disasmTrace(addr_t base, DisasmTrace* trace);

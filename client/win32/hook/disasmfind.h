#pragma once

#include "loader/loader.h"
#include "disasm.h"

typedef struct Symbol Symbol;

typedef enum DisasmOpType {
    DISASM_FINISH = 0,   // end of op list
    DISASM_INSTR,        // Instruction to match
    DISASM_SKIP,         // Instructions to skip
    DISASM_MARK,   // Mark the next matching instruction (or its operand) as the symbol address
    DISASM_JMP,    // follow a jump or conditional jump instead of disassembling it
    DISASM_CALL,   // follow a call instead of disassembling it (there is no return logic, it's
                   // treated like a jump)
} DisasmOpType;

typedef enum DisasmArgOp {
    ARG_IGNORE,   // don't care what's in this argument
    ARG_MATCH,    // must exactly match what's in the args
    ARG_MARK,     // for DISASM_MARK ops, use this argument value rather than the EIP of the
                  // instruction
} DisasmArgOp;

typedef struct DisasmOp {
    DisasmOpType type;   // dissembly operation

    int imin, imax;      // min and max number of instructions to skip (before the next match) for
                         // DISASM_SKIP

    e_inst inst;         // instruction to match for DISASM_INSTR
    DisasmArgOp argfilter[3];   // argument filter
    const char* argstr[3];      // if set, overrides the argument with a stringtable address
    Symbol* argsym[3];          // if set, overrides the argument with a symbol address
    t_arg args[3];              // data for arguments
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

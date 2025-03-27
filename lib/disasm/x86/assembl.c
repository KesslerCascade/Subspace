// Free Disassembler and Assembler -- Assembler
//
// Copyright (C) 2001 Oleh Yuschuk
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// 16.01.2002 - corrected error in processing of immediate constants.
// clang-format off


#define STRICT

#include <ctype.h>
#include "minicrt.h"
#pragma hdrstop

#include "disasm.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ASSEMBLER FUNCTIONS //////////////////////////////

// Scanner modes.
#define SA_NAME        0x0001          // Don't try to decode labels
#define SA_IMPORT      0x0002          // Allow import pseudolabel

// Types of input tokens reported by scanner.
#define SCAN_EOL       0               // End of line
#define SCAN_REG8      1               // 8-bit register
#define SCAN_REG16     2               // 16-bit register
#define SCAN_REG32     3               // 32-bit register
#define SCAN_SEG       4               // Segment register
#define SCAN_FPU       5               // FPU register
#define SCAN_MMX       6               // MMX register
#define SCAN_CR        7               // Control register
#define SCAN_DR        8               // Debug register
#define SCAN_OPSIZE    9               // Operand size modifier
#define SCAN_JMPSIZE   10              // Jump size modifier
#define SCAN_LOCAL     11              // Address on stack in form LOCAL.decimal
#define SCAN_ARG       12              // Address on stack in form ARG.decimal
#define SCAN_PTR       20              // PTR in MASM addressing statements
#define SCAN_REP       21              // REP prefix
#define SCAN_REPE      22              // REPE prefix
#define SCAN_REPNE     23              // REPNE prefix
#define SCAN_LOCK      24              // LOCK prefix
#define SCAN_NAME      25              // Command or label
#define SCAN_ICONST    26              // Hexadecimal constant
#define SCAN_DCONST    27              // Decimal constant
#define SCAN_OFS       28              // Undefined constant
#define SCAN_FCONST    29              // Floating-point constant
#define SCAN_EIP       30              // Register EIP
#define SCAN_SIGNED    31              // Keyword "SIGNED" (in expressions)
#define SCAN_UNSIGNED  32              // Keyword "UNSIGNED" (in expressions)
#define SCAN_CHAR      33              // Keyword "CHAR" (in expressions)
#define SCAN_FLOAT     34              // Keyword "FLOAT" (in expressions)
#define SCAN_DOUBLE    35              // Keyword "DOUBLE" (in expressions)
#define SCAN_FLOAT10   36              // Keyword "FLOAT10" (in expressions)
#define SCAN_STRING    37              // Keyword "STRING" (in expressions)
#define SCAN_UNICODE   38              // Keyword "UNICODE" (in expressions)
#define SCAN_MSG       39              // Pseudovariable MSG (in expressions)

#define SCAN_SYMB      64              // Any other character
#define SCAN_IMPORT    65              // Import pseudolabel
#define SCAN_ERR       255             // Definitely bad item

// Definition used by Assembler to report command matching errors.
#define MA_JMP         0x0001          // Invalid jump size modifier
#define MA_NOP         0x0002          // Wrong number of operands
#define MA_TYP         0x0004          // Bad operand type
#define MA_NOS         0x0008          // Explicit operand size expected
#define MA_SIZ         0x0010          // Bad operand size
#define MA_DIF         0x0020          // Different operand sizes
#define MA_SEG         0x0040          // Invalid segment register
#define MA_RNG         0x0080          // Constant out of expected range

typedef struct t_asmoperand
{
    int            type;                 // Operand type, see beginning of file
    int            size;                 // Operand size or 0 if yet unknown
    int            index;                // Index or other register
    int            scale;                // Scale
    int            base;                 // Base register if present
    long           offset;               // Immediate value or offset
    int            anyoffset;            // Offset is present but undefined
    int            segment;              // Segment in address if present
    int            jmpmode;              // Specified jump size
} t_asmoperand;

static const t_asm *asmcmd;            // Pointer to assembly struct
static int       prio;                 // Priority of operation (0: highest)
static long      idata;                // Last scanned value
static long      double fdata;         // Floating-point number
static char      *asmerror;            // Explanation of last error, or NULL

// Fetches one complete operand from the input and fills in structure op
// with operand's data. Expects that first token of the operand is already
// scanned. Supports operands in generalized form (for example, R32 means any
// of general-purpose 32-bit integer registers).
static void Parseasmoperand(t_asmoperand *op, const t_arg *arg)
{
    int j;
    if (!arg)
        return;                            // No or bad operand
      // Jump or call address may begin with address size modifier(s) SHORT, LONG,
      // NEAR and/or FAR. Not all combinations are allowed. After operand is
      // completely parsed, this function roughly checks whether modifier is
      // allowed. Exact check is done in Assemble().
    if (arg->ptrsize < 0) {
        j = -arg->ptrsize;
        if (
            ((j & 0x03) == 0x03) ||            // Mixed SHORT and LONG
            ((j & 0x0C) == 0x0C) ||            // Mixed NEAR and FAR
            ((j & 0x09) == 0x09)               // Mixed FAR and SHORT
            ) {
            asmerror = "Invalid combination of jump address modifiers";
            return;
        };
        if ((j & 0x08) == 0) j |= 0x04;        // Force NEAR if not FAR
        op->jmpmode = j;
    };
    // Simple operands are either register or constant, their processing is
    // obvious and straightforward.
    op->segment = arg->seg;
    if (arg->base >= REG_EAX && arg->base < REG_BX_SI &&
        arg->idx == REG_UNDEF && arg->addr == 0 && arg->ptrsize <= 0) {
        if (arg->base >= REG_EAX && arg->base <= REG_EDI) {
            op->type = REG;
            op->size = 4;
            op->index = arg->base;
        } else if (arg->base >= REG_AX && arg->base <= REG_DI) {
            op->type = REG;
            op->size = 2;
            op->index = arg->base - REG_AX;
        } else if (arg->base >= REG_AL && arg->base <= REG_BH) {
            op->type = REG;
            op->size = 1;
            op->index = arg->base - REG_AL;
        } else if (arg->base >= REG_ES && arg->base <= REG_GS) {
            op->type = SGM;
            op->index = arg->base - REG_ES;
        } else if (arg->base >= REG_ST0 && arg->base <= REG_ST7) {
            op->type = RST;
            op->index = arg->base - REG_ST0;
        } else if (arg->base >= REG_MM0 && arg->base <= REG_MM7) {
            op->type = RMX;
            op->index = arg->base - REG_MM0;
        } else if (arg->base >= REG_CR0 && arg->base <= REG_CR7) {
            op->type = CRX;
            op->index = arg->base - REG_CR0;
        } else if (arg->base >= REG_DR0 && arg->base <= REG_DR7) {
            op->type = DRX;
            op->index = arg->base - REG_DR0;
        } else if (arg->base >= REG_XMM0 && arg->base <= REG_XMM7) {
            op->type = RXM;
            op->index = arg->base - REG_XMM0;
        }
    } else if (arg->base == REG_UNDEF && arg->idx == REG_UNDEF && arg->ptrsize <= 0) {
        // Immediate value
        if (arg->segaddr > 0) {
            op->type = JMF;
            op->segment = arg->segaddr;
        } else {
            op->type = IMM;
        }
        op->offset = arg->addr;
    } else if (arg->base == REG_EBX && arg->idx == REG_AL) {
        op->type = MXL;
    } else if (arg->ptrsize > 0) {
        op->type = MRG;
        op->base = arg->base;
        op->index = arg->idx;
        op->scale = arg->scale;
        op->offset = arg->disp;
        op->size = arg->ptrsize;
    } else {
        asmerror = "Unrecognized operand"; return;
    };
    // In general, address modifier is allowed only with address expression which
    // is a constant, a far address or a memory expression. More precise check
    // will be done later in Assemble().
    if (op->jmpmode != 0 && op->type != IMM && op->type != JMF && op->type != MRG) {
        asmerror = "Jump address modifier is not allowed";
        return;
    };
};

// Function assembles text into 32-bit 80x86 machine code. It supports imprecise
// operands (for example, R32 stays for any general-purpose 32-bit register).
// This allows to search for incomplete commands. Command is precise when all
// significant bytes in model.mask are 0xFF. Some commands have more than one
// decoding. By calling Assemble() with attempt=0,1... and constsize=0,1,2,3 one
// gets also alternative variants (bit 0x1 of constsize is responsible for size
// of address constant and bit 0x2 - for immediate data). However, only one
// address form is generated ([EAX*2], but not [EAX+EAX]; [EBX+EAX] but not
// [EAX+EBX]; [EAX] will not use SIB byte; no DS: prefix and so on). Returns
// number of bytes in assembled code or non-positive number in case of detected
// error. This number is the negation of the offset in the input text where the
// error encountered. Unfortunately, BC 4.52 is unable to compile the switch
// (arg) in this code when any common subexpression optimization is on. The
// next #pragma statement disables all optimizations.

//#pragma option -Od                     // No optimizations, or BC 4.52 crashes

int Assemble(const t_asm *cmd, ulong ip, t_asmmodel *model, int attempt,
             int constsize, char *errtext)
{
    int i, j, k, nameok, arg, match, datasize, addrsize, bytesize, minop, maxop;
    int segment, jmpsize, jmpmode, longjump;
    int hasrm, hassib, dispsize, immsize;
    int anydisp, anyimm, anyjmp;
    long l, displacement, immediate, jmpoffset;
    uchar tcode[MAXCMDSIZE], tmask[MAXCMDSIZE];
    t_asmoperand aop[3], *op;             // Up to 3 operands allowed
    const t_cmddata *pd;
    if (model != NULL) model->length = 0;
    if (cmd == NULL || model == NULL || errtext == NULL) {
        if (errtext != NULL) strcpy(errtext, "Internal Assembler error");
        return 0;
    };                       // Error in parameters
    asmcmd = cmd;
    errtext[0] = '\0';
    if (!cmd)                  // End of line, nothing to assemble
        return 0;
    // Parse command operands (up to 3). Note: jump address is always the first
    // (and only) operand in actual command set.
    for (i = 0; i < 3; i++) {
        aop[i].type = NNN;                   // No operand
        aop[i].size = 0;                     // Undefined size
        aop[i].index = -1;                   // No index
        aop[i].scale = 0;                    // No scale
        aop[i].base = -1;                    // No base
        aop[i].offset = 0;                   // No offset
        aop[i].anyoffset = 0;                // No offset
        aop[i].segment = SEG_UNDEF;          // No segment
        aop[i].jmpmode = 0;
    };               // No jump size modifier
    for (i = 0; i < cmd->nargs; i++) {
        Parseasmoperand(&aop[i], &cmd->arg[i]);
        if (i == 0) {
            jmpmode = aop[0].jmpmode;
            if (jmpmode != 0) jmpmode |= 0x80;
        }
    }
    // If jump size is not specified, function tries to use short jump. If
    // attempt fails, it retries with long form.
    longjump = 0;                          // Try short jump on the first pass
retrylongjump:
    nameok = 0;
    // Some commands allow different number of operands. Variables minop and
    // maxop accumulate their minimal and maximal counts. The numbers are not
    // used in assembly process but allow for better error diagnostics.
    minop = 3; maxop = 0;
    // Main assembly loop: try to find the command which matches all operands,
    // but do not process operands yet.
    for (pd = cmddata; pd->mask != 0; pd++) {
        if (pd->inst != cmd->inst)
            continue;
        datasize = 0;
        addrsize = 4;

        // For error diagnostics it is important to know whether mnemonic exists.
        nameok++;
        if (pd->arg1 == NNN || pd->arg1 >= PSEUDOOP)
            minop = 0;
        else if (pd->arg2 == NNN || pd->arg2 >= PSEUDOOP) {
            if (minop > 1) minop = 1;
            if (maxop < 1) maxop = 1;
        } else if (pd->arg3 == NNN || pd->arg3 >= PSEUDOOP) {
            if (minop > 2) minop = 2;
            if (maxop < 2) maxop = 2;
        } else
            maxop = 3;
        // Determine default and allowed operand size(s).
        if (pd->bits == FF) datasize = 2;      // Forced 16-bit size
        if (pd->bits == WW || pd->bits == WS || pd->bits == W3 || pd->bits == WP)
            bytesize = 1;                      // 1-byte size allowed
        else
            bytesize = 0;                      // Word/dword size only
          // Check whether command operands match specified. If so, variable match
          // remains zero, otherwise it contains kind of mismatch. This allows for
          // better error diagnostics.
        match = 0;
        for (j = 0; j < 3; j++) {              // Up to 3 operands
            op = aop + j;
            if (j == 0) arg = pd->arg1;
            else if (j == 1) arg = pd->arg2;
            else arg = pd->arg3;
            if (arg == NNN || arg >= PSEUDOOP) {
                if (op->type != NNN)             // No more arguments
                    match |= MA_NOP;
                break;
            };
            if (op->type == NNN) {
                match |= MA_NOP; break;
            };       // No corresponding operand
            switch (arg) {
            case REG:                      // Integer register in Reg field
            case RCM:                      // Integer register in command byte
            case RAC:                      // Accumulator (AL/AX/EAX, implicit)
                if (op->type != REG) match |= MA_TYP;
                if (arg == RAC && op->index != REG_EAX && op->index != 8) match |= MA_TYP;
                if (bytesize == 0 && op->size == 1) match |= MA_SIZ;
                if (datasize == 0) datasize = op->size;
                if (datasize != op->size) match |= MA_DIF;
                break;
            case RG4:                      // Integer 4-byte register in Reg field
                if (op->type != REG) match |= MA_TYP;
                if (op->size != 4) match |= MA_SIZ;
                if (datasize == 0) datasize = op->size;
                if (datasize != op->size) match |= MA_DIF;
                break;
            case RAX:                      // AX (2-byte, implicit)
                if (op->type != REG || (op->index != REG_EAX && op->index != 8))
                    match |= MA_TYP;
                if (op->size != 2) match |= MA_SIZ;
                if (datasize == 0) datasize = op->size;
                if (datasize != op->size) match |= MA_DIF;
                break;
            case RDX:                      // DX (16-bit implicit port address)
                if (op->type != REG || (op->index != REG_EDX && op->index != 8))
                    match |= MA_TYP;
                if (op->size != 2) match |= MA_SIZ; break;
            case RCL:                      // Implicit CL register (for shifts)
                if (op->type != REG || (op->index != REG_ECX && op->index != 8))
                    match |= MA_TYP;
                if (op->size != 1) match |= MA_SIZ;
                break;
            case RS0:                      // Top of FPU stack (ST(0))
                if (op->type != RST || (op->index != 0 && op->index != 8))
                    match |= MA_TYP;
                break;
            case RST:                      // FPU register (ST(i)) in command byte
                if (op->type != RST) match |= MA_TYP; break;
            case RMX:                      // MMX register MMx
            case R3D:                      // 3DNow! register MMx
                if (op->type != RMX) match |= MA_TYP; break;
            case RXM:
                if (op->type != RXM) match |= MA_TYP; break;
            case MRG:                      // Memory/register in ModRM byte
                if (op->type != MRG && op->type != REG) match |= MA_TYP;
                if (bytesize == 0 && op->size == 1) match |= MA_SIZ;
                if (datasize == 0) datasize = op->size;
                if (op->size != 0 && op->size != datasize) match |= MA_DIF;
                break;
            case MR1:                      // 1-byte memory/register in ModRM byte
                if (op->type != MRG && op->type != REG) match |= MA_TYP;
                if (op->size != 0 && op->size != 1) match |= MA_SIZ;
                break;
            case MR2:                      // 2-byte memory/register in ModRM byte
                if (op->type != MRG && op->type != REG) match |= MA_TYP;
                if (op->size != 0 && op->size != 2) match |= MA_SIZ;
                break;
            case MR4:                      // 4-byte memory/register in ModRM byte
                if (op->type != MRG && op->type != REG) match |= MA_TYP;
                if (op->size != 0 && op->size != 4) match |= MA_SIZ;
                break;
            case MX4:
                if (op->type != MRG && op->type != RXM) match |= MA_TYP;
                if (op->size != 0 && op->size != 4) match |= MA_SIZ;
                break;
            case RR4:                      // 4-byte memory/register (register only)
                if (op->type != REG) match |= MA_TYP;
                if (op->size != 0 && op->size != 4) match |= MA_SIZ;
                break;
            case MRJ:                      // Memory/reg in ModRM as JUMP target
                if (op->type != MRG && op->type != REG) match |= MA_TYP;
                if (op->size != 0 && op->size != 4) match |= MA_SIZ;
                if ((jmpmode & 0x09) != 0) match |= MA_JMP;
                jmpmode &= 0x7F; break;
            case MR8:                      // 8-byte memory/MMX register in ModRM
            case MRD:                      // 8-byte memory/3DNow! register in ModRM
                if (op->type != MRG && op->type != RMX) match |= MA_TYP;
                if (op->size != 0 && op->size != 8) match |= MA_SIZ;
                break;
            case MR0:                      // 16-byte memory/SSE register in ModRM byte
                if (op->type != MRG && op->type != RXM) match |= MA_TYP;
                if (op->size != 0 && op->size != 16) match |= MA_SIZ;
                break;
            case RR8:                      // 8-byte MMX register only in ModRM
            case RRD:                      // 8-byte memory/3DNow! (register only)
                if (op->type != RMX) match |= MA_TYP;
                if (op->size != 0 && op->size != 8) match |= MA_SIZ;
                break;
            case MMA:                      // Memory address in ModRM byte for LEA
                if (op->type != MRG) match |= MA_TYP; break;
            case MML:                      // Memory in ModRM byte (for LES)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 6) match |= MA_SIZ;
                if (datasize == 0) datasize = 4; else if (datasize != 4) match |= MA_DIF;
                break;
            case MMS:                      // Memory in ModRM byte (as SEG:OFFS)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 6) match |= MA_SIZ;
                if ((jmpmode & 0x07) != 0) match |= MA_JMP;
                jmpmode &= 0x7F; break;
            case MM6:                      // Memory in ModRm (6-byte descriptor)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 6) match |= MA_SIZ;
                break;
            case MMB:                      // Two adjacent memory locations (BOUND)
                if (op->type != MRG) match |= MA_TYP;
                k = op->size; if (k > 1) k /= 2;
                if (k != 0 && k != datasize) match |= MA_DIF;
                break;
            case MD2:                      // Memory in ModRM byte (16-bit integer)
            case MB2:                      // Memory in ModRM byte (16-bit binary)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 2) match |= MA_SIZ;
                break;
            case MD4:                      // Memory in ModRM byte (32-bit integer)
            case MF4:                      // Memory in ModRM byte (32-bit float)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 4) match |= MA_SIZ;
                break;
            case MD8:                      // Memory in ModRM byte (64-bit integer)
            case MF8:                      // Memory in ModRM byte (64-bit float)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 8) match |= MA_SIZ;
                break;
            case MDA:                      // Memory in ModRM byte (80-bit BCD)
            case MFA:                      // Memory in ModRM byte (80-bit float)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0 && op->size != 10) match |= MA_SIZ;
                break;
            case MFE:                      // Memory in ModRM byte (FPU environment)
            case MFS:                      // Memory in ModRM byte (FPU state)
            case MFX:                      // Memory in ModRM byte (ext. FPU state)
                if (op->type != MRG) match |= MA_TYP;
                if (op->size != 0) match |= MA_SIZ;
                break;
            case MSO:                      // Source in string operands ([ESI])
                if (op->type != MRG || op->base != REG_ESI ||
                    op->index != -1 || op->offset != 0 || op->anyoffset != 0) match |= MA_TYP;
                if (datasize == 0) datasize = op->size;
                if (op->size != 0 && op->size != datasize) match |= MA_DIF;
                break;
            case MDE:                      // Destination in string operands ([EDI])
                if (op->type != MRG || op->base != REG_EDI ||
                    op->index != -1 || op->offset != 0 || op->anyoffset != 0) match |= MA_TYP;
                if (op->segment != SEG_UNDEF && op->segment != SEG_ES) match |= MA_SEG;
                if (datasize == 0) datasize = op->size;
                if (op->size != 0 && op->size != datasize) match |= MA_DIF;
                break;
            case MXL:                      // XLAT operand ([EBX+AL])
                if (op->type != MXL) match |= MA_TYP; break;
            case IMM:                      // Immediate data (8 or 16/32)
            case IMU:                      // Immediate unsigned data (8 or 16/32)
                if (op->type != IMM) match |= MA_TYP;
                break;
            case VXD:                      // VxD service (32-bit only)
                if (op->type != IMM) match |= MA_TYP;
                if (datasize == 0) datasize = 4;
                if (datasize != 4) match |= MA_SIZ;
                break;
            case JMF:                      // Immediate absolute far jump/call addr
                if (op->type != JMF) match |= MA_TYP;
                if ((jmpmode & 0x05) != 0) match |= MA_JMP;
                jmpmode &= 0x7F; break;
            case JOB:                      // Immediate byte offset (for jumps)
                if (op->type != IMM || longjump) match |= MA_TYP;
                if ((jmpmode & 0x0A) != 0) match |= MA_JMP;
                jmpmode &= 0x7F; break;
            case JOW:                      // Immediate full offset (for jumps)
                if (op->type != IMM) match |= MA_TYP;
                if ((jmpmode & 0x09) != 0) match |= MA_JMP;
                jmpmode &= 0x7F; break;
            case IMA:                      // Immediate absolute near data address
                if (op->type != MRG || op->base >= 0 || op->index >= 0) match |= MA_TYP;
                break;
            case IMX:                      // Immediate sign-extendable byte
                if (op->type != IMM) match |= MA_TYP;
                if (op->offset < -128 || op->offset>127) match |= MA_RNG;
                break;
            case C01:                      // Implicit constant 1 (for shifts)
                if (op->type != IMM || (op->offset != 1 && op->anyoffset == 0))
                    match |= MA_TYP;
                break;
            case IMS:                      // Immediate byte (for shifts)
            case IM1:                      // Immediate byte
                if (op->type != IMM) match |= MA_TYP;
                if (op->offset < -128 || op->offset>255) match |= MA_RNG;
                break;
            case IM2:                      // Immediate word (ENTER/RET)
                if (op->type != IMM) match |= MA_TYP;
                if (op->offset < 0 || op->offset>65535) match |= MA_RNG;
                break;
            case SGM:                      // Segment register in ModRM byte
                if (op->type != SGM) match |= MA_TYP;
                if (datasize == 0) datasize = 2;
                if (datasize != 2) match |= MA_DIF;
                break;
            case SCM:                      // Segment register in command byte
                if (op->type != SGM) match |= MA_TYP;
                break;
            case CRX:                      // Control register CRx
            case DRX:                      // Debug register DRx
                if (op->type != arg) match |= MA_TYP;
                if (datasize == 0) datasize = 4;
                if (datasize != 4) match |= MA_DIF;
                break;
            case PRN:                      // Near return address (pseudooperand)
            case PRF:                      // Far return address (pseudooperand)
            case PAC:                      // Accumulator (AL/AX/EAX, pseudooperand)
            case PAH:                      // AH (in LAHF/SAHF, pseudooperand)
            case PFL:                      // Lower byte of flags (pseudooperand)
            case PS0:                      // Top of FPU stack (pseudooperand)
            case PS1:                      // ST(1) (pseudooperand)
            case PCX:                      // CX/ECX (pseudooperand)
            case PDI:                      // EDI (pseudooperand in MMX extentions)
                break;
            default:                       // Undefined type of operand
                strcpy(errtext, "Internal Assembler error");
                goto error;
            };                               // End of switch (arg)
            if ((jmpmode & 0x80) != 0) match |= MA_JMP;
            if (match != 0) break;             // Some of the operands doesn't match
        };                                 // End of operand matching loop
        if (match == 0) {                    // Exact match found
            if (attempt > 0) {
                --attempt; nameok = 0;
            }         // Well, try to find yet another match
            else break;
        };
    };                                   // End of command search loop
    // Check whether some error was detected. If several errors were found
    // similtaneously, report one (roughly in order of significance).
    if (nameok == 0) {                     // Mnemonic unavailable
        strcpy(errtext, "Unrecognized command");
        asmcmd = 0; goto error;
    };
    if (match != 0) {                      // Command not found
        if (minop > 0 && aop[minop - 1].type == NNN)
            strcpy(errtext, "Too few operands");
        else if (maxop < 3 && aop[maxop].type != NNN)
            strcpy(errtext, "Too many operands");
        else if (nameok > 1)                 // More that 1 command
            strcpy(errtext, "Command does not support given operands");
        else if (match & MA_JMP)
            strcpy(errtext, "Invalid jump size modifier");
        else if (match & MA_NOP)
            strcpy(errtext, "Wrong number of operands");
        else if (match & MA_TYP)
            strcpy(errtext, "Command does not support given operands");
        else if (match & MA_NOS)
            strcpy(errtext, "Please specify operand size");
        else if (match & MA_SIZ)
            strcpy(errtext, "Bad operand size");
        else if (match & MA_DIF)
            strcpy(errtext, "Different size of operands");
        else if (match & MA_SEG)
            strcpy(errtext, "Invalid segment register");
        else if (match & MA_RNG)
            strcpy(errtext, "Constant out of expected range");
        else
            strcpy(errtext, "Erroneous command");
        goto error;
    };
    // Exact match found. Now construct the code.
    hasrm = 0;                             // Whether command has ModR/M byte
    hassib = 0;                            // Whether command has SIB byte
    dispsize = 0;                          // Size of displacement (if any)
    immsize = 0;                           // Size of immediate data (if any)
    segment = SEG_UNDEF;                   // Necessary segment prefix
    jmpsize = 0;                           // No relative jumps
    memset(tcode, 0, sizeof(tcode));
    *(ulong *)tcode = pd->code & pd->mask;
    memset(tmask, 0, sizeof(tmask));
    *(ulong *)tmask = pd->mask;
    i = pd->len - 1;                         // Last byte of command itself
    if ((pd->type & C_TYPEMASK) == C_REP) i++;  // REPxx prefixes count as extra byte
    // In some cases at least one operand must have explicit size declaration (as
    // in MOV [EAX],1). This preliminary check does not include all cases.
    if (pd->bits == WW || pd->bits == WS || pd->bits == WP) {
        if (datasize == 0) {
            strcpy(errtext, "Please specify operand size"); goto error;
        } else if (datasize > 1)
            tcode[i] |= 0x01;                  // WORD or DWORD size of operands
        tmask[i] |= 0x01;
    } else if (pd->bits == W3) {
        if (datasize == 0) {
            strcpy(errtext, "Please specify operand size"); goto error;
        } else if (datasize > 1)
            tcode[i] |= 0x08;                  // WORD or DWORD size of operands
        tmask[i] |= 0x08;
    };
    // Present suffix of 3DNow! command as immediate byte operand.
    if ((pd->type & C_TYPEMASK) == C_NOW) {
        immsize = 1;
        immediate = (pd->code >> 16) & 0xFF;
    };
    // Process operands again, this time constructing the code.
    anydisp = anyimm = anyjmp = 0;
    for (j = 0; j < 3; j++) {                // Up to 3 operands
        op = aop + j;
        if (j == 0) arg = pd->arg1;
        else if (j == 1) arg = pd->arg2;
        else arg = pd->arg3;
        if (arg == NNN) break;               // All operands processed
        switch (arg) {
        case REG:                        // Integer register in Reg field
        case RG4:                        // Integer 4-byte register in Reg field
        case RMX:                        // MMX register MMx
        case RXM:                        // SSE register XMMx
        case R3D:                        // 3DNow! register MMx
        case CRX:                        // Control register CRx
        case DRX:                        // Debug register DRx
            hasrm = 1;
            if (op->index < 8) {
                tcode[i + 1] |= (uchar)(op->index << 3); tmask[i + 1] |= 0x38;
            };
            break;
        case RCM:                        // Integer register in command byte
        case RST:                        // FPU register (ST(i)) in command byte
            if (op->index < 8) {
                tcode[i] |= (uchar)op->index; tmask[i] |= 0x07;
            };
            break;
        case RAC:                        // Accumulator (AL/AX/EAX, implicit)
        case RAX:                        // AX (2-byte, implicit)
        case RDX:                        // DX (16-bit implicit port address)
        case RCL:                        // Implicit CL register (for shifts)
        case RS0:                        // Top of FPU stack (ST(0))
        case MDE:                        // Destination in string op's ([EDI])
        case C01:                        // Implicit constant 1 (for shifts)
            break;                         // Simply skip implicit operands
        case MSO:                        // Source in string op's ([ESI])
        case MXL:                        // XLAT operand ([EBX+AL])
            if (op->segment != SEG_UNDEF && op->segment != SEG_DS)
                segment = op->segment;
            break;
        case MRG:                        // Memory/register in ModRM byte
        case MRJ:                        // Memory/reg in ModRM as JUMP target
        case MR1:                        // 1-byte memory/register in ModRM byte
        case MR2:                        // 2-byte memory/register in ModRM byte
        case MR4:                        // 4-byte memory/register in ModRM byte
        case MX4:                        // 4-byte memory/SSE register in ModRM byte
        case RR4:                        // 4-byte memory/register (register only)
        case MR8:                        // 8-byte memory/MMX register in ModRM
        case RR8:                        // 8-byte MMX register only in ModRM
        case MRD:                        // 8-byte memory/3DNow! register in ModRM
        case RRD:                        // 8-byte memory/3DNow! (register only)
        case MR0:                        // 16-byte memory/SSE register in ModRM
            hasrm = 1;
            if (op->type != MRG) {           // Register in ModRM byte
                tcode[i + 1] |= 0xC0; tmask[i + 1] |= 0xC0;
                if (op->index < 8) {
                    tcode[i + 1] |= (uchar)op->index; tmask[i + 1] |= 0x07;
                };
                break;
            };                             // Note: NO BREAK, continue with address
        case MMA:                        // Memory address in ModRM byte for LEA
        case MML:                        // Memory in ModRM byte (for LES)
        case MMS:                        // Memory in ModRM byte (as SEG:OFFS)
        case MM6:                        // Memory in ModRm (6-byte descriptor)
        case MMB:                        // Two adjacent memory locations (BOUND)
        case MD2:                        // Memory in ModRM byte (16-bit integer)
        case MB2:                        // Memory in ModRM byte (16-bit binary)
        case MD4:                        // Memory in ModRM byte (32-bit integer)
        case MD8:                        // Memory in ModRM byte (64-bit integer)
        case MDA:                        // Memory in ModRM byte (80-bit BCD)
        case MF4:                        // Memory in ModRM byte (32-bit float)
        case MF8:                        // Memory in ModRM byte (64-bit float)
        case MFA:                        // Memory in ModRM byte (80-bit float)
        case MFE:                        // Memory in ModRM byte (FPU environment)
        case MFS:                        // Memory in ModRM byte (FPU state)
        case MFX:                        // Memory in ModRM byte (ext. FPU state)
            hasrm = 1; displacement = op->offset; anydisp = op->anyoffset;
            if (op->base < 0 && op->index < 0) {
                dispsize = 4;                  // Special case of immediate address
                if (op->segment != SEG_UNDEF && op->segment != SEG_DS)
                    segment = op->segment;
                tcode[i + 1] |= 0x05;
                tmask[i + 1] |= 0xC7;
            } else if (op->index < 0 && op->base != REG_ESP) {
                tmask[i + 1] |= 0xC0;            // SIB byte unnecessary
                if (op->offset == 0 && op->anyoffset == 0 && op->base != REG_EBP)
                    ;                          // [EBP] always requires offset
                else if ((constsize & 1) != 0 &&
                         ((op->offset >= -128 && op->offset < 128) || op->anyoffset != 0)
                         ) {
                    tcode[i + 1] |= 0x40;          // Disp8
                    dispsize = 1;
                } else {
                    tcode[i + 1] |= 0x80;          // Disp32
                    dispsize = 4;
                };
                if (op->base < 8) {
                    if (op->segment != SEG_UNDEF && op->segment != addr32[op->base].defseg)
                        segment = op->segment;
                    tcode[i + 1] |=
                        (uchar)op->base;          // Note that case [ESP] has base<0.
                    tmask[i + 1] |= 0x07;
                } else segment = op->segment;
            } else {                         // SIB byte necessary
                hassib = 1;
                if (op->base == REG_EBP &&     // EBP as base requires offset, optimize
                    op->index >= 0 && op->scale == 1 && op->offset == 0 && op->anyoffset == 0) {
                    op->base = op->index; op->index = REG_EBP;
                };
                if (op->index == REG_ESP &&    // ESP cannot be an index, reorder
                    op->scale <= 1) {
                    op->index = op->base; op->base = REG_ESP; op->scale = 1;
                };
                if (op->base < 0 &&            // No base means 4-byte offset, optimize
                    op->index >= 0 && op->scale == 2 &&
                    op->offset >= -128 && op->offset < 128 && op->anyoffset == 0) {
                    op->base = op->index; op->scale = 1;
                };
                if (op->index == REG_ESP) {    // Reordering was unsuccessfull
                    strcpy(errtext, "Invalid indexing mode");
                    goto error;
                };
                if (op->base < 0) {
                    tcode[i + 1] |= 0x04;
                    dispsize = 4;
                } else if (op->offset == 0 && op->anyoffset == 0 && op->base != REG_EBP)
                    tcode[i + 1] |= 0x04;          // No displacement
                else if ((constsize & 1) != 0 &&
                         ((op->offset >= -128 && op->offset < 128) || op->anyoffset != 0)
                         ) {
                    tcode[i + 1] |= 0x44;          // Disp8
                    dispsize = 1;
                } else {
                    tcode[i + 1] |= 0x84;          // Disp32
                    dispsize = 4;
                };
                tmask[i + 1] |= 0xC7;            // ModRM completed, proceed with SIB
                if (op->scale == 2) tcode[i + 2] |= 0x40;
                else if (op->scale == 4) tcode[i + 2] |= 0x80;
                else if (op->scale == 8) tcode[i + 2] |= 0xC0;
                tmask[i + 2] |= 0xC0;
                if (op->index < 8) {
                    if (op->index < 0) op->index = 0x04;
                    tcode[i + 2] |= (uchar)(op->index << 3);
                    tmask[i + 2] |= 0x38;
                };
                if (op->base < 8) {
                    if (op->base < 0) op->base = 0x05;
                    if (op->segment != SEG_UNDEF && op->segment != addr32[op->base].defseg)
                        segment = op->segment;
                    tcode[i + 2] |= (uchar)op->base;
                    tmask[i + 2] |= 0x07;
                } else segment = op->segment;
            };
            break;
        case IMM:                        // Immediate data (8 or 16/32)
        case IMU:                        // Immediate unsigned data (8 or 16/32)
        case VXD:                        // VxD service (32-bit only)
            if (datasize == 0 && pd->arg2 == NNN && (pd->bits == SS || pd->bits == WS))
                datasize = 4;
            if (datasize == 0) {
                strcpy(errtext, "Please specify operand size");
                goto error;
            };
            immediate = op->offset; anyimm = op->anyoffset;
            if (pd->bits == SS || pd->bits == WS) {
                if (datasize > 1 && (constsize & 2) != 0 &&
                    ((immediate >= -128 && immediate < 128) || op->anyoffset != 0)) {
                    immsize = 1; tcode[i] |= 0x02;
                } else immsize = datasize;
                tmask[i] |= 0x02;
            } else immsize = datasize;
            break;
        case IMX:                        // Immediate sign-extendable byte
        case IMS:                        // Immediate byte (for shifts)
        case IM1:                        // Immediate byte
            if (immsize == 2)                // To accomodate ENTER instruction
                immediate = (immediate & 0xFFFF) | (op->offset << 16);
            else immediate = op->offset;
            anyimm |= op->anyoffset;
            immsize++; break;
        case IM2:                        // Immediate word (ENTER/RET)
            immediate = op->offset; anyimm = op->anyoffset;
            immsize = 2; break;
        case IMA:                        // Immediate absolute near data address
            if (op->segment != SEG_UNDEF && op->segment != SEG_DS)
                segment = op->segment;
            displacement = op->offset; anydisp = op->anyoffset;
            dispsize = 4; break;
        case JOB:                        // Immediate byte offset (for jumps)
            jmpoffset = op->offset; anyjmp = op->anyoffset;
            jmpsize = 1; break;
        case JOW:                        // Immediate full offset (for jumps)
            jmpoffset = op->offset; anyjmp = op->anyoffset;
            jmpsize = 4; break;
        case JMF:                        // Immediate absolute far jump/call addr
            displacement = op->offset; anydisp = op->anyoffset; dispsize = 4;
            immediate = op->segment; anyimm = op->anyoffset; immsize = 2;
            break;
        case SGM:                        // Segment register in ModRM byte
            hasrm = 1;
            if (op->index < 6) {
                tcode[i + 1] |= (uchar)(op->index << 3); tmask[i + 1] |= 0x38;
            };
            break;
        case SCM:                        // Segment register in command byte
            if (op->index == SEG_FS || op->index == SEG_GS) {
                tcode[0] = 0x0F; tmask[0] = 0xFF;
                i = 1;
                if (cmd->inst == I_PUSH)
                    tcode[i] = (uchar)((op->index << 3) | 0x80);
                else
                    tcode[i] = (uchar)((op->index << 3) | 0x81);
                tmask[i] = 0xFF;
            } else if (op->index < 6) {
                if (op->index == SEG_CS && cmd->inst == I_POP) {
                    strcpy(errtext, "Unable to POP CS");
                    goto error;
                };
                tcode[i] = (uchar)((tcode[i] & 0xC7) | (op->index << 3));
            } else {
                tcode[i] &= 0xC7;
                tmask[i] &= 0xC7;
            };
            break;
        case PRN:                        // Near return address (pseudooperand)
        case PRF:                        // Far return address (pseudooperand)
        case PAC:                        // Accumulator (AL/AX/EAX, pseudooperand)
        case PAH:                        // AH (in LAHF/SAHF, pseudooperand)
        case PFL:                        // Lower byte of flags (pseudooperand)
        case PS0:                        // Top of FPU stack (pseudooperand)
        case PS1:                        // ST(1) (pseudooperand)
        case PCX:                        // CX/ECX (pseudooperand)
        case PDI:                        // EDI (pseudooperand in MMX extentions)
            break;                         // Simply skip preudooperands
        default:                         // Undefined type of operand
            strcpy(errtext, "Internal Assembler error");
            goto error;
        };
    };
    // Gather parts of command together in the complete command.
    j = 0;
    if (cmd->lock != 0) {                  // Lock prefix specified
        model->code[j] = 0xF0;
        model->mask[j] = 0xFF; j++;
    };
    if (datasize == 2 && pd->bits != FF) {   // Data size prefix necessary
        model->code[j] = 0x66;
        model->mask[j] = 0xFF; j++;
    };
    if (addrsize == 2) {                   // Address size prefix necessary
        model->code[j] = 0x67;
        model->mask[j] = 0xFF; j++;
    };
    if (segment != SEG_UNDEF) {            // Segment prefix necessary
        if (segment == SEG_ES) model->code[j] = 0x26;
        else if (segment == SEG_CS) model->code[j] = 0x2E;
        else if (segment == SEG_SS) model->code[j] = 0x36;
        else if (segment == SEG_DS) model->code[j] = 0x3E;
        else if (segment == SEG_FS) model->code[j] = 0x64;
        else if (segment == SEG_GS) model->code[j] = 0x65;
        else { strcpy(errtext, "Internal Assembler error"); goto error; };
        model->mask[j] = 0xFF; j++;
    };
    if (dispsize > 0) {
        memcpy(tcode + i + 1 + hasrm + hassib, &displacement, dispsize);
        if (anydisp == 0) memset(tmask + i + 1 + hasrm + hassib, 0xFF, dispsize);
    };
    if (immsize > 0) {
        if (immsize == 1) l = 0xFFFFFF00L;
        else if (immsize == 2) l = 0xFFFF0000L;
        else l = 0L;
        if ((immediate & l) != 0 && (immediate & l) != l) {
            strcpy(errtext, "Constant does not fit into operand");
            goto error;
        };
        memcpy(tcode + i + 1 + hasrm + hassib + dispsize, &immediate, immsize);
        if (anyimm == 0) memset(tmask + i + 1 + hasrm + hassib + dispsize, 0xFF, immsize);
    };
    i = i + 1 + hasrm + hassib + dispsize + immsize;
    jmpoffset = jmpoffset - (i + j + jmpsize);
    model->jmpsize = jmpsize;
    model->jmpoffset = jmpoffset;
    model->jmppos = i + j;
    if (jmpsize != 0) {
        if (ip != 0) {
            jmpoffset = jmpoffset - ip;
            if (jmpsize == 1 && anyjmp == 0 && (jmpoffset < -128 || jmpoffset >= 128)) {
                if (longjump == 0 && (jmpmode & 0x03) == 0) {
                    longjump = 1;
                    goto retrylongjump;
                };
                strcpy(errtext, "Relative jump out of range, use LONG form");
                goto error;
            };
            memcpy(tcode + i, &jmpoffset, jmpsize);
        };
        if (anyjmp == 0) memset(tmask + i, 0xFF, jmpsize);
        i += jmpsize;
    };
    memcpy(model->code + j, tcode, i);
    memcpy(model->mask + j, tmask, i);
    i += j;
    model->length = i;
    return i;                            // Positive value: length of code
error:
    model->length = 0;
    return -1;                           // Negative value: error
};

//#pragma option -O.                     // Restore old optimization options


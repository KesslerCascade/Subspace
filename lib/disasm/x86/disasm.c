// Free Disassembler and Assembler -- Disassembler
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
// clang-format off


#define STRICT

#include <ctype.h>
#include "minicrt.h"
#pragma hdrstop

#define MAINPROG
#include "disasm.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// DISASSEMBLER FUNCTIONS ////////////////////////////

// Work variables of disassembler
static t_arg    *parg;                 // Current argument
static ulong     datasize;             // Size of data (1,2,4 bytes)
static ulong     addrsize;             // Size of address (2 or 4 bytes)
static int       segprefix;            // Segment override prefix or SEG_UNDEF
static int       hasrm;                // Command has ModR/M byte
static int       hassib;               // Command has SIB byte
static int       dispsize;             // Size of displacement (if any)
static int       immsize;              // Size of immediate data (if any)
static int       softerror;            // Noncritical disassembler error
static int       addcomment;           // Comment value of operand

// Copy of input parameters of function Disasm()
static uchar     *cmd;                 // Pointer to binary data
static uchar     *pfixup;              // Pointer to possible fixups or NULL
static ulong     size;                 // Remaining size of the command buffer
static t_disasm  *da;                  // Pointer to disassembly results
static int       mode;                 // Disassembly mode (DISASM_xxx)

// Defaults for convenience
const t_asm empty_asm = {
    .inst = I_NONE,
    .nargs = 0,
    .arg = {
        { .base = REG_UNDEF,.idx = REG_UNDEF,.seg = SEG_UNDEF,.scale = 1 },
        { .base = REG_UNDEF,.idx = REG_UNDEF,.seg = SEG_UNDEF,.scale = 1 },
        { .base = REG_UNDEF,.idx = REG_UNDEF,.seg = SEG_UNDEF,.scale = 1 },
    }
};

// Disassemble name of 1, 2 or 4-byte general-purpose integer register and, if
// requested and available, dump its contents. Parameter type changes decoding
// of contents for some operand types.
static void DecodeRG(int index, int datasize, int type)
{
    int sizeindex;
    if (mode < DISASM_DATA) return;        // No need to decode
    index &= 0x07;
    if (datasize == 1)
        sizeindex = 0;
    else if (datasize == 2)
        sizeindex = 1;
    else if (datasize == 4)
        sizeindex = 2;
    else {
        da->error = DAE_INTERN; return;
    };
    if (mode >= DISASM_FILE) {
        if (type < PSEUDOOP)               // Not a pseudooperand
            parg->base = regid[sizeindex][index];
        ;
    };
};

// Disassemble name of 80-bit floating-point register and, if available, dump
// its contents.
static void DecodeST(int index, int pseudoop)
{
    if (mode < DISASM_FILE) return;        // No need to decode
    index &= 0x07;
    if (pseudoop == 0) {
        parg->base = REG_ST0 + index;
    };
};

// Disassemble name of 64-bit MMX register.
static void DecodeMX(int index)
{
    if (mode < DISASM_FILE) return;        // No need to decode
    index &= 0x07;
    parg->base = REG_MM0 + index;
};

// Disassemble name of 64-bit 3DNow! register and, if available, dump its
// contents.
static void DecodeNR(int index)
{
    if (mode < DISASM_FILE) return;        // No need to decode
    index &= 0x07;
    parg->base = REG_MM0 + index;
};

// Disassemble name of 128-bit SSE register.
static void DecodeXMM(int index)
{
    if (mode < DISASM_FILE)
        return;   // No need to decode
    index &= 0x07;
    parg->base = REG_XMM0 + index;
};

static void Memadr(e_seg defseg, e_reg base, e_reg idx, int scale, ulong addr, int dsize)
{
    e_seg seg;
    if (segprefix != SEG_UNDEF) seg = segprefix; else seg = defseg;

    parg->base = base;
    parg->idx = idx;
    parg->scale = scale;
    parg->addr = addr;
    parg->ptrsize = dsize;
    if (seg != defseg)
        parg->seg = seg;
}

// Disassemble memory/register from the ModRM/SIB bytes and, if available, dump
// address and contents of memory.
static void DecodeMR(int type)
{
    int j, memonly, inmemory, seg;
    e_reg base, idx;
    int c, sib, scale;
    ulong dsize, regsize, addr;
    if (size < 2) {
        da->error = DAE_CROSS; return;
    };    // ModR/M byte outside the memory block
    hasrm = 1;
    dsize = regsize = datasize;              // Default size of addressed reg/memory
    memonly = 0;                           // Register in ModM field is allowed
    // Size and kind of addressed memory or register in ModM has no influence on
    // the command size, and exact calculations are omitted if only command size
    // is requested. If register is used, optype will be incorrect and we need
    // to correct it later.
    c = cmd[1] & 0xC7;                   // Leave only Mod and M fields
    if (mode >= DISASM_DATA) {
        if ((c & 0xC0) == 0xC0)          // Register operand
            inmemory = 0;
        else                             // Memory operand
            inmemory = 1;
        switch (type) {
        case MRG:                        // Memory/register in ModRM byte
            if (inmemory) {
                if (datasize == 1) da->memtype = DEC_BYTE;
                else if (datasize == 2) da->memtype = DEC_WORD;
                else da->memtype = DEC_DWORD;
            };
            break;
        case MRJ:                        // Memory/reg in ModRM as JUMP target
            if (datasize != 2 && inmemory)
                da->memtype = DEC_DWORD;
            break;
        case MR1:                        // 1-byte memory/register in ModRM byte
            dsize = regsize = 1;
            if (inmemory) da->memtype = DEC_BYTE; break;
        case MR2:                        // 2-byte memory/register in ModRM byte
            dsize = regsize = 2;
            if (inmemory) da->memtype = DEC_WORD; break;
        case MR4:                        // 4-byte memory/register in ModRM byte
        case RR4:                        // 4-byte memory/register (register only)
        case MX4:                        // 4-byte memory/SSE register
            dsize = regsize = 4;
            if (inmemory) da->memtype = DEC_DWORD; break;
        case MR8:                        // 8-byte memory/MMX register in ModRM
        case RR8:                        // 8-byte MMX register only in ModRM
        case MX8:                        // 8-byte SSE register only in ModRM
            dsize = 8;
            if (inmemory) da->memtype = DEC_QWORD; break;
        case MX0:
            dsize = 16;
            if (inmemory) da->memtype = DEC_DQWORD; break;
        case MRD:                        // 8-byte memory/3DNow! register in ModRM
        case RRD:                        // 8-byte memory/3DNow! (register only)
            dsize = 8;
            if (inmemory) da->memtype = DEC_3DNOW; break;
        case MMA:                        // Memory address in ModRM byte for LEA
            memonly = 1; break;
        case MML:                        // Memory in ModRM byte (for LES)
            dsize = datasize + 2; memonly = 1;
            if (datasize == 4 && inmemory)
                da->memtype = DEC_FWORD;
            da->warnings |= DAW_SEGMENT;
            break;
        case MMS:                        // Memory in ModRM byte (as SEG:OFFS)
            dsize = datasize + 2; memonly = 1;
            if (datasize == 4 && inmemory)
                da->memtype = DEC_FWORD;
            break;
        case MM6:                        // Memory in ModRM (6-byte descriptor)
            dsize = 6; memonly = 1;
            if (inmemory) da->memtype = DEC_FWORD; break;
        case MMB:                        // Two adjacent memory locations (BOUND)
            dsize = datasize * 2; memonly = 1; break;
        case MD2:                        // Memory in ModRM byte (16-bit integer)
        case MB2:                        // Memory in ModRM byte (16-bit binary)
            dsize = 2; memonly = 1;
            if (inmemory) da->memtype = DEC_WORD; break;
        case MD4:                        // Memory in ModRM byte (32-bit integer)
            dsize = 4; memonly = 1;
            if (inmemory) da->memtype = DEC_DWORD; break;
        case MD8:                        // Memory in ModRM byte (64-bit integer)
            dsize = 8; memonly = 1;
            if (inmemory) da->memtype = DEC_QWORD; break;
        case MDA:                        // Memory in ModRM byte (80-bit BCD)
            dsize = 10; memonly = 1;
            if (inmemory) da->memtype = DEC_TBYTE; break;
        case MF4:                        // Memory in ModRM byte (32-bit float)
            dsize = 4; memonly = 1;
            if (inmemory) da->memtype = DEC_FLOAT4; break;
        case MF8:                        // Memory in ModRM byte (64-bit float)
            dsize = 8; memonly = 1;
            if (inmemory) da->memtype = DEC_FLOAT8; break;
        case MFA:                        // Memory in ModRM byte (80-bit float)
            dsize = 10; memonly = 1;
            if (inmemory) da->memtype = DEC_FLOAT10; break;
        case MFE:                        // Memory in ModRM byte (FPU environment)
            dsize = 28; memonly = 1; break;
        case MFS:                        // Memory in ModRM byte (FPU state)
            dsize = 108; memonly = 1; break;
        case MFX:                        // Memory in ModRM byte (ext. FPU state)
            dsize = 512; memonly = 1; break;
        default:                         // Operand is not in ModM!
            da->error = DAE_INTERN;
            break;
        };
    };
    addr = 0;
    base = REG_UNDEF;
    idx = REG_UNDEF;
    scale = 1;
    // There are many possibilities to decode ModM/SIB address. The first
    // possibility is register in ModM - general-purpose, MMX or 3DNow!
    if ((c & 0xC0) == 0xC0) {                  // Decode register operand
        if (type == MR8 || type == RR8)
            DecodeMX(c);                       // MMX register
        else if (type == MX0 || type == MX4 || type == MX8)
            DecodeXMM(c);                      // SSE register
        else if (type == MRD || type == RRD)
            DecodeNR(c);                       // 3DNow! register
        else
            DecodeRG(c, regsize, type);        // General-purpose register
        if (memonly != 0)
            softerror = DAE_MEMORY;            // Register where only memory allowed
        return;
    };
    // Next possibility: 16-bit addressing mode, very seldom in 32-bit flat model
    // but still supported by processor. SIB byte is never used here.
    if (addrsize == 2) {
        if (c == 0x06) {                       // Special case of immediate address
            dispsize = 2;
            if (size < 4)
                da->error = DAE_CROSS;         // Disp16 outside the memory block
            else if (mode >= DISASM_DATA) {
                da->adrconst = addr = *(ushort *)(cmd + 2);
                if (addr == 0) da->zeroconst = 1;
                seg = SEG_DS;
                Memadr(seg, REG_UNDEF, REG_UNDEF, 1, addr, dsize);
            };
        } else {
            da->indexed = 1;
            if ((c & 0xC0) == 0x40) {          // 8-bit signed displacement
                if (size < 3) da->error = DAE_CROSS;
                else addr = (signed char)cmd[2] & 0xFFFF;
                dispsize = 1;
            } else if ((c & 0xC0) == 0x80) {   // 16-bit unsigned displacement
                if (size < 4) da->error = DAE_CROSS;
                else addr = *(ushort *)(cmd + 2);
                dispsize = 2;
            };
            if (mode >= DISASM_DATA && da->error == DAE_NOERR) {
                da->adrconst = addr;
                if (addr == 0) da->zeroconst = 1;
                seg = addr16[c & 0x07].defseg;
                Memadr(seg, addr16[c & 0x07].id, REG_UNDEF, 1, addr, dsize);
            };
        };
    }
    // Next possibility: immediate 32-bit address.
    else if (c == 0x05) {                      // Special case of immediate address
        dispsize = 4;
        if (size < 6)
            da->error = DAE_CROSS;             // Disp32 outside the memory block
        else if (mode >= DISASM_DATA) {
            da->adrconst = addr = *(ulong *)(cmd + 2);
            if (pfixup == NULL) pfixup = cmd + 2;
            da->fixupsize += 4;
            if (addr == 0) da->zeroconst = 1;
            seg = SEG_DS;
            Memadr(seg, REG_UNDEF, REG_UNDEF, 1, addr, dsize);
        };
    }
    // Next possibility: 32-bit address with SIB byte.
    else if ((c & 0x07) == 0x04) {             // SIB addresation
        sib = cmd[2]; hassib = 1;
        if (c == 0x04 && (sib & 0x07) == 0x05) {
            dispsize = 4;                      // Immediate address without base
            if (size < 7)
                da->error = DAE_CROSS;         // Disp32 outside the memory block
            else {
                da->adrconst = addr = *(ulong *)(cmd + 3);
                if (pfixup == NULL) pfixup = cmd + 3;
                da->fixupsize += 4;
                if (addr == 0) da->zeroconst = 1;
                if ((sib & 0x38) != 0x20) {    // Index register present
                    da->indexed = 1;
                    if (type == MRJ) da->jmptable = addr;
                };
                seg = SEG_DS;
            };
        } else {                               // Base and, eventually, displacement
            if ((c & 0xC0) == 0x40) {          // 8-bit displacement
                dispsize = 1;
                if (size < 4) da->error = DAE_CROSS;
                else {
                    da->adrconst = addr = (signed char)cmd[3];
                    if (addr == 0) da->zeroconst = 1;
                };
            } else if ((c & 0xC0) == 0x80) {     // 32-bit displacement
                dispsize = 4;
                if (size < 7)
                    da->error = DAE_CROSS;         // Disp32 outside the memory block
                else {
                    da->adrconst = addr = *(ulong *)(cmd + 3);
                    if (pfixup == NULL) pfixup = cmd + 3;
                    da->fixupsize += 4;
                    if (addr == 0) da->zeroconst = 1;
                    // Most compilers use address of type [index*4+displacement] to
                    // address jump table (switch). But, for completeness, I allow all
                    // cases which include index with scale 1 or 4, base or both.
                    if (type == MRJ) da->jmptable = addr;
                };
            };
            da->indexed = 1;
            j = sib & 0x07;
            if (mode >= DISASM_FILE) {
                base = regid[2][j];
                seg = addr32[j].defseg;
            };
        };
        if ((sib & 0x38) != 0x20) {          // Scaled index present
            if ((sib & 0xC0) == 0x40) da->indexed = 2;
            else if ((sib & 0xC0) == 0x80) da->indexed = 4;
            else if ((sib & 0xC0) == 0xC0) da->indexed = 8;
            else da->indexed = 1;
        };
        if (mode >= DISASM_FILE && da->error == DAE_NOERR) {
            if ((sib & 0x38) != 0x20) {        // Scaled index present
                idx = addr32[(sib >> 3) & 0x07].id;
                if ((sib & 0xC0) == 0x40) {
                    da->jmptable = 0;              // Hardly a switch!
                    scale = 2;
                } else if ((sib & 0xC0) == 0x80)
                    scale = 4;
                else if ((sib & 0xC0) == 0xC0) {
                    da->jmptable = 0;              // Hardly a switch!
                    scale = 8;
                };
            };
            Memadr(seg, base, idx, scale, addr, dsize);
        };
    }
    // Last possibility: 32-bit address without SIB byte.
    else {                               // No SIB
        if ((c & 0xC0) == 0x40) {
            dispsize = 1;
            if (size < 3) da->error = DAE_CROSS; // Disp8 outside the memory block
            else {
                da->adrconst = addr = (signed char)cmd[2];
                if (addr == 0) da->zeroconst = 1;
            };
        } else if ((c & 0xC0) == 0x80) {
            dispsize = 4;
            if (size < 6)
                da->error = DAE_CROSS;           // Disp32 outside the memory block
            else {
                da->adrconst = addr = *(ulong *)(cmd + 2);
                if (pfixup == NULL) pfixup = cmd + 2;
                da->fixupsize += 4;
                if (addr == 0) da->zeroconst = 1;
                if (type == MRJ) da->jmptable = addr;
            };
        };
        da->indexed = 1;
        if (mode >= DISASM_FILE && da->error == DAE_NOERR) {
            seg = addr32[c & 0x07].defseg;
            Memadr(seg, addr32[c & 0x07].id, REG_UNDEF, 1, addr, dsize);
        };
    };
};

// Disassemble implicit source of string operations and, if available, dump
// address and contents.
static void DecodeSO(void)
{
    if (mode < DISASM_FILE) return;        // No need to decode
    if (datasize == 1) da->memtype = DEC_BYTE;
    else if (datasize == 2) da->memtype = DEC_WORD;
    else if (datasize == 4) da->memtype = DEC_DWORD;
    da->indexed = 1;
    Memadr(SEG_DS, regid[addrsize == 2 ? 1 : 2][REG_ESI], REG_UNDEF, 1, 0L, datasize);
};

// Disassemble implicit destination of string operations and, if available,
// dump address and contents. Destination always uses segment ES, and this
// setting cannot be overridden.
static void DecodeDE(void)
{
    int seg;
    if (mode < DISASM_FILE) return;        // No need to decode
    if (datasize == 1) da->memtype = DEC_BYTE;
    else if (datasize == 2) da->memtype = DEC_WORD;
    else if (datasize == 4) da->memtype = DEC_DWORD;
    da->indexed = 1;
    seg = segprefix; segprefix = SEG_ES;     // Fake Memadr by changing segment prefix
    Memadr(SEG_DS, regid[addrsize == 2 ? 1 : 2][REG_EDI], REG_UNDEF, 1, 0L, datasize);
    segprefix = seg;                       // Restore segment prefix
};

// Decode XLAT operand and, if available, dump address and contents.
static void DecodeXL(void)
{
    if (mode < DISASM_FILE) return;        // No need to decode
    da->memtype = DEC_BYTE;
    da->indexed = 1;
    Memadr(SEG_DS, (addrsize == 2 ? REG_BX : REG_EBX), REG_AL, 1, 0L, 1);
};

// Decode immediate operand of size constsize. If sxt is non-zero, byte operand
// should be sign-extended to sxt bytes. If type of immediate constant assumes
// this, small negative operands may be displayed as signed negative numbers.
// Note that in most cases immediate operands are not shown in comment window.
static void DecodeIM(int constsize, int sxt, int type)
{
    signed long data;
    ulong l;
    immsize += constsize;                    // Allows several immediate operands
    if (mode < DISASM_DATA) return;
    l = 1 + hasrm + hassib + dispsize + (immsize - constsize);
    data = 0;
    if (size < l + constsize)
        da->error = DAE_CROSS;
    else if (constsize == 1) {
        if (sxt == 0) data = (uchar)cmd[l];
        else data = (signed char)cmd[l];
        if (type == IMS && ((data & 0xE0) != 0 || data == 0)) {
            da->warnings |= DAW_SHIFT;
            da->cmdtype |= C_RARE;
        };
    } else if (constsize == 2) {
        if (sxt == 0) data = *(ushort *)(cmd + l);
        else data = *(short *)(cmd + l);
    } else {
        data = *(long *)(cmd + l);
        if (pfixup == NULL) pfixup = cmd + l;
        da->fixupsize += 4;
    };
    if (sxt == 2) data &= 0x0000FFFF;
    if (data == 0 && da->error == 0) da->zeroconst = 1;
    // Command ENTER, as an exception from Intel's rules, has two immediate
    // constants. As the second constant is rarely used, I exclude it from
    // search if the first constant is non-zero (which is usually the case).
    if (da->immconst == 0)
        da->immconst = data;
    if (mode >= DISASM_FILE && da->error == DAE_NOERR) {
        parg->addr = data;
    };
};

// Decode VxD service name (always 4-byte).
static void DecodeVX(void)
{
    ulong l, data;
    immsize += 4;                          // Allows several immediate operands
    if (mode < DISASM_DATA) return;
    l = 1 + hasrm + hassib + dispsize + (immsize - 4);
    if (size < l + 4) {
        da->error = DAE_CROSS;
        return;
    };
    data = *(long *)(cmd + l);
    if (data == 0 && da->error == 0) da->zeroconst = 1;
    if (da->immconst == 0)
        da->immconst = data;
    if (mode >= DISASM_FILE && da->error == DAE_NOERR) {
        parg->addr = data;
    };
};

// Decode implicit constant 1 (used in shift commands). This operand is so
// insignificant that it is never shown in comment window.
static void DecodeC1(void)
{
    if (mode < DISASM_DATA) return;
    da->immconst = 1;
    if (mode >= DISASM_FILE) parg->addr = 1;
};

// Decode immediate absolute data address. This operand is used in 8080-
// compatible commands which allow to move data from memory to accumulator and
// back. Note that bytes ModRM and SIB never appear in commands with IA operand.
static void DecodeIA(void)
{
    ulong addr;
    if (size < 1 + addrsize) {
        da->error = DAE_CROSS; return;
    };
    dispsize = addrsize;
    if (mode < DISASM_DATA) return;
    if (datasize == 1) da->memtype = DEC_BYTE;
    else if (datasize == 2) da->memtype = DEC_WORD;
    else if (datasize == 4) da->memtype = DEC_DWORD;
    if (addrsize == 2)
        addr = *(ushort *)(cmd + 1);
    else {
        addr = *(ulong *)(cmd + 1);
        if (pfixup == NULL) pfixup = cmd + 1;
        da->fixupsize += 4;
    };
    da->adrconst = addr;
    if (addr == 0) da->zeroconst = 1;
    if (mode >= DISASM_FILE) {
        Memadr(SEG_DS, REG_UNDEF, REG_UNDEF, 1, addr, datasize);
    };
};

// Decodes jump relative to nextip of size offsize.
static void DecodeRJ(ulong offsize, ulong nextip)
{
    ulong addr;
    if (size < offsize + 1) {
        da->error = DAE_CROSS; return;
    };
    dispsize = offsize;                    // Interpret offset as displacement
    if (mode < DISASM_DATA) return;
    if (offsize == 1)
        addr = (signed char)cmd[1] + nextip;
    else if (offsize == 2)
        addr = *(signed short *)(cmd + 1) + nextip;
    else
        addr = *(ulong *)(cmd + 1) + nextip;
    if (datasize == 2)
        addr &= 0xFFFF;
    da->jmpconst = addr;
    if (addr == 0) da->zeroconst = 1;
    if (mode >= DISASM_FILE) {
        parg->addr = addr;
        parg->ptrsize = -(long)(offsize == 1 ? 1 : 2);      // SHORT (-1) or LONG (-2)
    };
};

// Decode immediate absolute far jump address. In flat model, such addresses
// are not used (mostly because selector is specified directly in the command),
// so I neither decode as symbol nor comment it. To allow search for selector
// by value, I interprete it as an immediate constant.
static void DecodeJF(void)
{
    ulong addr, seg;
    if (size < 1 + addrsize + 2) {
        da->error = DAE_CROSS; return;
    };
    dispsize = addrsize; immsize = 2;        // Non-trivial but allowed interpretation
    if (mode < DISASM_DATA) return;
    if (addrsize == 2) {
        addr = *(ushort *)(cmd + 1);
        seg = *(ushort *)(cmd + 3);
    } else {
        addr = *(ulong *)(cmd + 1);
        seg = *(ushort *)(cmd + 5);
    };
    da->jmpconst = addr;
    da->immconst = seg;
    if (addr == 0 || seg == 0) da->zeroconst = 1;
    if (mode >= DISASM_FILE) {
        parg->segaddr = (ushort)seg;
        parg->addr = addr;
        parg->ptrsize = -8;               // FAR
    };
};

// Decode segment register. In flat model, operands of this type are seldom.
static void DecodeSG(int index)
{
    if (mode < DISASM_DATA) return;
    index &= 0x07;
    if (index >= 6) softerror = DAE_BADSEG;  // Undefined segment register
    if (mode >= DISASM_FILE) {
        parg->base = REG_ES + index;
    };
};

// Decode control register addressed in R part of ModRM byte. Operands of
// this type are extremely rare. Contents of control registers are accessible
// only from privilege level 0, so I cannot dump them here.
static void DecodeCR(int index)
{
    hasrm = 1;
    if (mode >= DISASM_FILE) {
        index = (index >> 3) & 0x07;
        parg->base = REG_CR0 + index;
    };
};

// Decode debug register addressed in R part of ModRM byte. Operands of
// this type are extremely rare. I can dump only those debug registers
// available in CONTEXT structure.
static void DecodeDR(int index)
{
    hasrm = 1;
    if (mode >= DISASM_FILE) {
        index = (index >> 3) & 0x07;
        parg->base = REG_DR0 + index;
    };
};

// Skips 3DNow! operands and extracts command suffix. Returns suffix or -1 if
// suffix lies outside the memory block. This subroutine assumes that cmd still
// points to the beginning of 3DNow! command (i.e. to the sequence of two bytes
// 0F, 0F).
static int Get3dnowsuffix(void)
{
    int c, sib;
    ulong offset;
    if (size < 3) return -1;                 // Suffix outside the memory block
    offset = 3;
    c = cmd[2] & 0xC7;                       // Leave only Mod and M fields
    // Register in ModM - general-purpose, MMX or 3DNow!
    if ((c & 0xC0) == 0xC0)
        ;
    // 16-bit addressing mode, SIB byte is never used here.
    else if (addrsize == 2) {
        if (c == 0x06)                       // Special case of immediate address
            offset += 2;
        else if ((c & 0xC0) == 0x40)         // 8-bit signed displacement
            offset++;
        else if ((c & 0xC0) == 0x80)         // 16-bit unsigned displacement
            offset += 2;
        ;
    }
    // Immediate 32-bit address.
    else if (c == 0x05)                      // Special case of immediate address
        offset += 4;
    // 32-bit address with SIB byte.
    else if ((c & 0x07) == 0x04) {           // SIB addresation
        if (size < 4) return -1;             // Suffix outside the memory block
        sib = cmd[3]; offset++;
        if (c == 0x04 && (sib & 0x07) == 0x05)
            offset += 4;                     // Immediate address without base
        else if ((c & 0xC0) == 0x40)         // 8-bit displacement
            offset += 1;
        else if ((c & 0xC0) == 0x80)         // 32-bit dislacement
            offset += 4;
        ;
    }
    // 32-bit address without SIB byte
    else if ((c & 0xC0) == 0x40)
        offset += 1;
    else if ((c & 0xC0) == 0x80)
        offset += 4;
    if (offset >= size) return -1;         // Suffix outside the memory block
    return cmd[offset];
};

// Function checks whether 80x86 flags meet condition set in the command.
// Returns 1 if condition is met, 0 if not and -1 in case of error (which is
// not possible).
int Checkcondition(int code, ulong flags)
{
    ulong cond, temp;
    switch (code & 0x0E) {
    case 0:                            // If overflow
        cond = flags & 0x0800; break;
    case 2:                            // If below
        cond = flags & 0x0001; break;
    case 4:                            // If equal
        cond = flags & 0x0040; break;
    case 6:                            // If below or equal
        cond = flags & 0x0041; break;
    case 8:                            // If sign
        cond = flags & 0x0080; break;
    case 10:                           // If parity
        cond = flags & 0x0004; break;
    case 12:                           // If less
        temp = flags & 0x0880;
        cond = (temp == 0x0800 || temp == 0x0080); break;
    case 14:                           // If less or equal
        temp = flags & 0x0880;
        cond = (temp == 0x0800 || temp == 0x0080 || (flags & 0x0040) != 0); break;
    default: return -1;                // Internal error, not possible!
    };
    if ((code & 0x01) == 0) return (cond != 0);
    else return (cond == 0);           // Invert condition
};

ulong Disasm(uchar *src, ulong srcsize, ulong srcip,
             t_disasm *disasm, int disasmmode)
{
    int i, j, isprefix, is3dnow, repeated, operand, arg;
    ulong u, code;
    int lockprefix;                      // Non-zero if lock prefix present
    int repprefix;                       // REPxxx prefix or 0
    int tryprefix;                       // Prefix that may indicate a special command encoding
    int cxsize;
    const t_cmddata *pd, *pdan;
    // Prepare disassembler variables and initialize structure disasm.
    datasize = addrsize = 4;                 // 32-bit code and data segments only!
    segprefix = SEG_UNDEF;
    hasrm = hassib = 0; dispsize = immsize = 0;
    lockprefix = 0; repprefix = 0; tryprefix = 0;
    cmd = src; size = srcsize; pfixup = NULL;
    softerror = 0; is3dnow = 0;
    da = disasm;
    da->ip = srcip;
    da->cmdtype = C_BAD; da->nprefix = 0;
    da->memtype = DEC_UNKNOWN; da->indexed = 0;
    da->jmpconst = 0; da->jmptable = 0;
    da->adrconst = 0; da->immconst = 0;
    da->zeroconst = 0;
    da->fixupoffset = 0; da->fixupsize = 0;
    da->warnings = 0; da->lock = 0;
    da->error = DAE_NOERR;
    mode = disasmmode;                     // No need to use register contents
    if (mode >= DISASM_FILE) {
        for (i = 0; i < 3; ++i) {
            da->arg[i].base = REG_UNDEF;
            da->arg[i].idx = REG_UNDEF;
            da->arg[i].seg = SEG_UNDEF;
            da->arg[i].scale = 1;
            da->arg[i].addr = 0;
            da->arg[i].ptrsize = 0;
            da->arg[i].segaddr = 0;
        }
    }
    // Correct 80x86 command may theoretically contain up to 4 prefixes belonging
    // to different prefix groups. This limits maximal possible size of the
    // command to MAXCMDSIZE=16 bytes. In order to maintain this limit, if
    // Disasm() detects second prefix from the same group, it flushes first
    // prefix in the sequence as a pseudocommand.
    u = 0; repeated = 0;
    while (size > 0) {
        isprefix = 1;                        // Assume that there is some prefix
        switch (*cmd) {
        case 0x26: if (segprefix == SEG_UNDEF) segprefix = SEG_ES;
                   else repeated = 1; break;
        case 0x2E: if (segprefix == SEG_UNDEF) segprefix = SEG_CS;
                   else repeated = 1; break;
        case 0x36: if (segprefix == SEG_UNDEF) segprefix = SEG_SS;
                   else repeated = 1; break;
        case 0x3E: if (segprefix == SEG_UNDEF) segprefix = SEG_DS;
                   else repeated = 1; break;
        case 0x64: if (segprefix == SEG_UNDEF) segprefix = SEG_FS;
                   else repeated = 1; break;
        case 0x65: if (segprefix == SEG_UNDEF) segprefix = SEG_GS;
                   else repeated = 1; break;
        case 0x66: if (datasize == 4) { tryprefix = 0x66; datasize = 2; }
                   else repeated = 1; break;
        case 0x67: if (addrsize == 4) addrsize = 2;
                   else repeated = 1; break;
        case 0xF0: if (lockprefix == 0) lockprefix = 0xF0;
                   else repeated = 1; break;
        case 0xF2: if (repprefix == 0) repprefix = 0xF2;
                   else repeated = 1; break;
        case 0xF3: if (repprefix == 0) repprefix = 0xF3;
                   else repeated = 1; break;
        default: isprefix = 0; break;
        };
        if (isprefix == 0 || repeated != 0)
            break;                           // No more prefixes or duplicated prefix
        da->nprefix++;
        cmd++; srcip++; size--; u++;
    };
    // We do have repeated prefix. Flush first prefix from the sequence.
    if (repeated) {
        if (mode >= DISASM_FILE) {
            da->nprefix = 1;
        };
        da->warnings |= DAW_PREFIX;
        if (lockprefix) {
            da->warnings |= DAW_LOCK;
        };
        da->cmdtype = C_RARE;
        return 1;                            // Any prefix is 1 byte long
    };
    // If lock prefix available, display it and forget, because it has no
    // influence on decoding of rest of the command.
    if (lockprefix != 0) {
        da->lock = 1;
        da->warnings |= DAW_LOCK;
    };
    // Fetch (if available) first 3 bytes of the command, add repeat prefix and
    // find command in the command table.
    code = 0;
    if (size > 0) *(((uchar *)&code) + 0) = cmd[0];
    if (size > 1) *(((uchar *)&code) + 1) = cmd[1];
    if (size > 2) *(((uchar *)&code) + 2) = cmd[2];

    if (tryprefix != 0) {
        ulong trycode = (code << 8) | tryprefix;
        for (pd = cmddata; pd->mask != 0; pd++) {
            if (((trycode^pd->code) & pd->mask) != 0) continue;
            if (mode >= DISASM_FILE && shortstringcmds &&
                (pd->arg1 == MSO || pd->arg1 == MDE || pd->arg2 == MSO || pd->arg2 == MDE))
                continue;                    // Search short form of string command
            break;
        };

        if (pd->mask == 0)
            tryprefix = 0;                   // Didn't find anything, process normally
    }

    if (tryprefix == 0) {
        if (repprefix != 0)                      // REP/REPE/REPNE is considered to be
            code = (code << 8) | repprefix;      // part of command.
        if (decodevxd && (code & 0xFFFF) == 0x20CD)
            pd = &vxdcmd;                        // Decode VxD call (Win95/98)
        else {
            for (pd = cmddata; pd->mask != 0; pd++) {
                if (((code^pd->code) & pd->mask) != 0) continue;
                if (mode >= DISASM_FILE && shortstringcmds &&
                    (pd->arg1 == MSO || pd->arg1 == MDE || pd->arg2 == MSO || pd->arg2 == MDE))
                    continue;                    // Search short form of string command
                break;
            };
        };
    }
    if ((pd->type & C_TYPEMASK) == C_NOW) {
        // 3DNow! commands require additional search.
        is3dnow = 1;
        j = Get3dnowsuffix();
        if (j < 0)
            da->error = DAE_CROSS;
        else {
            for (; pd->mask != 0; pd++) {
                if (((code^pd->code) & pd->mask) != 0) continue;
                if (((uchar *)&(pd->code))[2] == j) break;
            };
        };
    };
    if (pd->mask == 0) {                   // Command not found
        da->cmdtype = C_BAD;
        if (size < 2) da->error = DAE_CROSS;
        else da->error = DAE_BADCMD;
    } else {                               // Command recognized, decode it
        da->cmdtype = pd->type;
        cxsize = datasize;                   // Default size of ECX used as counter
        if (segprefix == SEG_FS || segprefix == SEG_GS || lockprefix != 0)
            da->cmdtype |= C_RARE;             // These prefixes are rare
        if (pd->bits == PR)
            da->warnings |= DAW_PRIV;          // Privileged command (ring 0)
        else if (pd->bits == WP)
            da->warnings |= DAW_IO;            // I/O command
          // Win32 programs usually try to keep stack dword-aligned, so INC ESP
          // (44) and DEC ESP (4C) usually don't appear in real code. Also check for
          // ADD ESP,imm and SUB ESP,imm (81,C4,imm32; 83,C4,imm8; 81,EC,imm32;
          // 83,EC,imm8).
        if (cmd[0] == 0x44 || cmd[0] == 0x4C ||
            (size >= 3 && (cmd[0] == 0x81 || cmd[0] == 0x83) &&
             (cmd[1] == 0xC4 || cmd[1] == 0xEC) && (cmd[2] & 0x03) != 0)
            ) {
            da->warnings |= DAW_STACK;
            da->cmdtype |= C_RARE;
        };
        // Warn also on MOV SEG,... (8E...). Win32 works in flat mode.
        if (cmd[0] == 0x8E)
            da->warnings |= DAW_SEGMENT;
        // If opcode is 2-byte, adjust command.
        if (pd->len == 2) {
            if (size == 0) da->error = DAE_CROSS;
            else {
                cmd++; srcip++; size--;
            };
        };
        if (size == 0) da->error = DAE_CROSS;
        // Some commands either feature non-standard data size or have bit which
        // allowes to select data size.
        if ((pd->bits & WW) != 0 && (*cmd & WW) == 0)
            datasize = 1;                      // Bit W in command set to 0
        else if ((pd->bits & W3) != 0 && (*cmd & W3) == 0)
            datasize = 1;                      // Another position of bit W
        else if ((pd->bits & FF) != 0)
            datasize = 2;                      // Forced word (2-byte) size
          // Some commands either have mnemonics which depend on data size (8/16 bits
          // or 32 bits, like CWD/CDQ), or have several different mnemonics (like
          // JNZ/JNE). First case is marked by either '&' (mnemonic depends on
          // operand size) or '$' (depends on address size). In the second case,
          // there is no special marker and disassembler selects main mnemonic.
        if (mode >= DISASM_FILE) {
            da->inst = pd->inst;
            da->command = pd;
        };
        // Decode operands (explicit - encoded in command, implicit - present in
        // mmemonic or assumed - used or modified by command). Assumed operands
        // must stay after all explicit and implicit operands. Up to 3 operands
        // are allowed.
        for (operand = 0; operand < 3; operand++) {
            if (da->error) break;            // Error - no sense to continue
            // If command contains both source and destination, one usually must not
            // decode destination to comment because it will be overwritten on the
            // next step. Global addcomment takes care of this. Decoding routines,
            // however, may ignore this flag.
            if (operand == 0 && pd->arg2 != NNN && pd->arg2 < PSEUDOOP)
                addcomment = 0;
            else
                addcomment = 1;
            // Get type of next argument.
            if (operand == 0) {
                arg = pd->arg1;
                parg = &da->arg[0];
            } else if (operand == 1) {
                arg = pd->arg2;
                parg = &da->arg[1];
            } else {
                arg = pd->arg3;
                parg = &da->arg[2];
            }
            if (arg == NNN) break;         // No more operands
            da->nargs = operand + 1;
            // Decode, analyse and comment next operand of the command.
            switch (arg) {
            case REG:                      // Integer register in Reg field
                if (size < 2) da->error = DAE_CROSS;
                else DecodeRG(cmd[1] >> 3, datasize, REG);
                hasrm = 1; break;
            case RCM:                      // Integer register in command byte
                DecodeRG(cmd[0], datasize, RCM); break;
            case RG4:                      // Integer 4-byte register in Reg field
                if (size < 2) da->error = DAE_CROSS;
                else DecodeRG(cmd[1] >> 3, 4, RG4);
                hasrm = 1; break;
            case RAC:                      // Accumulator (AL/AX/EAX, implicit)
                DecodeRG(REG_EAX, datasize, RAC); break;
            case RAX:                      // AX (2-byte, implicit)
                DecodeRG(REG_EAX, 2, RAX); break;
            case RDX:                      // DX (16-bit implicit port address)
                DecodeRG(REG_EDX, 2, RDX); break;
            case RCL:                      // Implicit CL register (for shifts)
                DecodeRG(REG_ECX, 1, RCL); break;
            case RS0:                      // Top of FPU stack (ST(0))
                DecodeST(0, 0); break;
            case RST:                      // FPU register (ST(i)) in command byte
                DecodeST(cmd[0], 0); break;
            case RMX:                      // MMX register MMx
                if (size < 2) da->error = DAE_CROSS;
                else DecodeMX(cmd[1] >> 3);
                hasrm = 1; break;
            case RXM:   // MMX register MMx
                if (size < 2)
                    da->error = DAE_CROSS;
                else
                    DecodeXMM(cmd[1] >> 3);
                hasrm = 1;
                break;
            case R3D:                      // 3DNow! register MMx
                if (size < 2) da->error = DAE_CROSS;
                else DecodeNR(cmd[1] >> 3);
                hasrm = 1; break;
            case MRG:                      // Memory/register in ModRM byte
            case MRJ:                      // Memory/reg in ModRM as JUMP target
            case MR1:                      // 1-byte memory/register in ModRM byte
            case MR2:                      // 2-byte memory/register in ModRM byte
            case MR4:                      // 4-byte memory/register in ModRM byte
            case MR8:                      // 8-byte memory/MMX register in ModRM
            case MRD:                      // 8-byte memory/3DNow! register in ModRM
            case MMA:                      // Memory address in ModRM byte for LEA
            case MML:                      // Memory in ModRM byte (for LES)
            case MM6:                      // Memory in ModRm (6-byte descriptor)
            case MMB:                      // Two adjacent memory locations (BOUND)
            case MD2:                      // Memory in ModRM byte (16-bit integer)
            case MB2:                      // Memory in ModRM byte (16-bit binary)
            case MD4:                      // Memory in ModRM byte (32-bit integer)
            case MD8:                      // Memory in ModRM byte (64-bit integer)
            case MDA:                      // Memory in ModRM byte (80-bit BCD)
            case MF4:                      // Memory in ModRM byte (32-bit float)
            case MF8:                      // Memory in ModRM byte (64-bit float)
            case MFA:                      // Memory in ModRM byte (80-bit float)
            case MFE:                      // Memory in ModRM byte (FPU environment)
            case MFS:                      // Memory in ModRM byte (FPU state)
            case MFX:                      // Memory in ModRM byte (ext. FPU state)
            case MX4:                      // 4-byte memory/SSE register in ModRM
            case MX8:                      // 8-byte memory/SSE register in ModRM
            case MX0:                      // 16-byte memory/SSE register in ModRM
                DecodeMR(arg); break;
            case MMS:                      // Memory in ModRM byte (as SEG:OFFS)
                DecodeMR(arg);
                da->warnings |= DAW_FARADDR; break;
            case RR4:                      // 4-byte memory/register (register only)
            case RR8:                      // 8-byte MMX register only in ModRM
            case RRD:                      // 8-byte memory/3DNow! (register only)
                if ((cmd[1] & 0xC0) != 0xC0) softerror = DAE_REGISTER;
                DecodeMR(arg); break;
            case MSO:                      // Source in string op's ([ESI])
                DecodeSO(); break;
            case MDE:                      // Destination in string op's ([EDI])
                DecodeDE(); break;
            case MXL:                      // XLAT operand ([EBX+AL])
                DecodeXL(); break;
            case IMM:                      // Immediate data (8 or 16/32)
            case IMU:                      // Immediate unsigned data (8 or 16/32)
                if ((pd->bits & SS) != 0 && (*cmd & 0x02) != 0)
                    DecodeIM(1, datasize, arg);
                else
                    DecodeIM(datasize, 0, arg);
                break;
            case VXD:                      // VxD service (32-bit only)
                DecodeVX(); break;
            case IMX:                      // Immediate sign-extendable byte
                DecodeIM(1, datasize, arg); break;
            case C01:                      // Implicit constant 1 (for shifts)
                DecodeC1(); break;
            case IMS:                      // Immediate byte (for shifts)
            case IM1:                      // Immediate byte
                DecodeIM(1, 0, arg); break;
            case IM2:                      // Immediate word (ENTER/RET)
                DecodeIM(2, 0, arg);
                if ((da->immconst & 0x03) != 0) da->warnings |= DAW_STACK;
                break;
            case IMA:                      // Immediate absolute near data address
                DecodeIA(); break;
            case JOB:                      // Immediate byte offset (for jumps)
                DecodeRJ(1, srcip + 2); break;
            case JOW:                      // Immediate full offset (for jumps)
                DecodeRJ(datasize, srcip + datasize + 1); break;
            case JMF:                      // Immediate absolute far jump/call addr
                DecodeJF();
                da->warnings |= DAW_FARADDR; break;
            case SGM:                      // Segment register in ModRM byte
                if (size < 2) da->error = DAE_CROSS;
                DecodeSG(cmd[1] >> 3); hasrm = 1; break;
            case SCM:                      // Segment register in command byte
                DecodeSG(cmd[0] >> 3);
                if ((da->cmdtype & C_TYPEMASK) == C_POP) da->warnings |= DAW_SEGMENT;
                break;
            case CRX:                      // Control register CRx
                if ((cmd[1] & 0xC0) != 0xC0) da->error = DAE_REGISTER;
                DecodeCR(cmd[1]); break;
            case DRX:                      // Debug register DRx
                if ((cmd[1] & 0xC0) != 0xC0) da->error = DAE_REGISTER;
                DecodeDR(cmd[1]); break;
            case PRN:                      // Near return address (pseudooperand)
                break;
            case PRF:                      // Far return address (pseudooperand)
                da->warnings |= DAW_FARADDR; break;
            case PAC:                      // Accumulator (AL/AX/EAX, pseudooperand)
                DecodeRG(REG_EAX, datasize, PAC); break;
            case PAH:                      // AH (in LAHF/SAHF, pseudooperand)
            case PFL:                      // Lower byte of flags (pseudooperand)
                break;
            case PS0:                      // Top of FPU stack (pseudooperand)
                DecodeST(0, 1); break;
            case PS1:                      // ST(1) (pseudooperand)
                DecodeST(1, 1); break;
            case PCX:                      // CX/ECX (pseudooperand)
                DecodeRG(REG_ECX, cxsize, PCX); break;
            case PDI:                      // EDI (pseudooperand in MMX extentions)
                DecodeRG(REG_EDI, 4, PDI); break;
            default:
                da->error = DAE_INTERN;        // Unknown argument type
                break;
            };
        };
        // Check whether command may possibly contain fixups.
        if (pfixup != NULL && da->fixupsize > 0)
            da->fixupoffset = pfixup - src;
        // Segment prefix and address size prefix are superfluous for command which
        // does not access memory. If this the case, mark command as rare to help
        // in analysis.
        if (da->memtype == DEC_UNKNOWN &&
            (segprefix != SEG_UNDEF)) {
            da->warnings |= DAW_PREFIX;
            da->cmdtype |= C_RARE;
        };
        // 16-bit addressing is rare in 32-bit programs. If this is the case,
        // mark command as rare to help in analysis.
        if (addrsize != 4) da->cmdtype |= C_RARE;
    };
    // Suffix of 3DNow! command is accounted best by assuming it immediate byte
    // constant.
    if (is3dnow) {
        if (immsize != 0) da->error = DAE_BADCMD;
        else immsize = 1;
    };
    // Right or wrong, command decoded. Now dump it.
    if (da->error != 0) {                  // Hard error in command detected
        if (da->error == DAE_BADCMD &&
            (*cmd == 0x0F || *cmd == 0xFF) && size > 0
            ) {
            cmd++; size--;
        };
        if (size > 0) {
            cmd++; size--;
        };
    } else {                               // No hard error, dump command
        cmd += 1 + hasrm + hassib + dispsize + immsize;
        size -= 1 + hasrm + hassib + dispsize + immsize;
    };
    // Check that command is not a dangerous one.
    if (mode >= DISASM_DATA) {
        for (pdan = dangerous; pdan->mask != 0; pdan++) {
            if (((code^pdan->code) & pdan->mask) != 0)
                continue;
            if (pdan->type == C_DANGERLOCK && lockprefix == 0)
                break;                         // Command harmless without LOCK prefix
            if (iswindowsnt && pdan->type == C_DANGER95)
                break;                         // Command harmless under Windows NT
              // Dangerous command!
            if (pdan->type == C_DANGER95) da->warnings |= DAW_DANGER95;
            else da->warnings |= DAW_DANGEROUS;
            break;
        };
    };
    if (da->error == 0 && softerror != 0)
        da->error = softerror;             // Error, but still display command
    return (srcsize - size);               // Returns number of recognized bytes
};


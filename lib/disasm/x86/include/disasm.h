// Free Disassembler and Assembler -- Header file
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

#pragma once

#ifndef MAINPROG
#define unique extern
#else
#define unique
#endif

#define NEGLIMIT       (-16384)        // Limit to display constans as signed
#define PSEUDOOP       128             // Base for pseudooperands
#define TEXTLEN        256             // Maximal length of text string

// Special command features.
#define WW             0x01            // Bit W (size of operand)
#define SS             0x02            // Bit S (sign extention of immediate)
#define WS             0x03            // Bits W and S
#define W3             0x08            // Bit W at position 3
#define CC             0x10            // Conditional jump
#define FF             0x20            // Forced 16-bit size
#define LL             0x40            // Conditional loop
#define PR             0x80            // Protected command
#define WP             0x81            // I/O command with bit W

// All possible types of operands in 80x86. A bit more than you expected, he?
#define NNN            0               // No operand
#define REG            1               // Integer register in Reg field
#define RCM            2               // Integer register in command byte
#define RG4            3               // Integer 4-byte register in Reg field
#define RAC            4               // Accumulator (AL/AX/EAX, implicit)
#define RAX            5               // AX (2-byte, implicit)
#define RDX            6               // DX (16-bit implicit port address)
#define RCL            7               // Implicit CL register (for shifts)
#define RS0            8               // Top of FPU stack (ST(0), implicit)
#define RST            9               // FPU register (ST(i)) in command byte
#define RMX            10              // MMX register MMx
#define R3D            11              // 3DNow! register MMx
#define MRG            12              // Memory/register in ModRM byte
#define MR1            13              // 1-byte memory/register in ModRM byte
#define MR2            14              // 2-byte memory/register in ModRM byte
#define MR4            15              // 4-byte memory/register in ModRM byte
#define RR4            16              // 4-byte memory/register (register only)
#define MR8            17              // 8-byte memory/MMX register in ModRM
#define RR8            18              // 8-byte MMX register only in ModRM
#define MRD            19              // 8-byte memory/3DNow! register in ModRM
#define RRD            20              // 8-byte memory/3DNow! (register only)
#define MRJ            21              // Memory/reg in ModRM as JUMP target
#define MMA            22              // Memory address in ModRM byte for LEA
#define MML            23              // Memory in ModRM byte (for LES)
#define MMS            24              // Memory in ModRM byte (as SEG:OFFS)
#define MM6            25              // Memory in ModRm (6-byte descriptor)
#define MMB            26              // Two adjacent memory locations (BOUND)
#define MD2            27              // Memory in ModRM (16-bit integer)
#define MB2            28              // Memory in ModRM (16-bit binary)
#define MD4            29              // Memory in ModRM byte (32-bit integer)
#define MD8            30              // Memory in ModRM byte (64-bit integer)
#define MDA            31              // Memory in ModRM byte (80-bit BCD)
#define MF4            32              // Memory in ModRM byte (32-bit float)
#define MF8            33              // Memory in ModRM byte (64-bit float)
#define MFA            34              // Memory in ModRM byte (80-bit float)
#define MFE            35              // Memory in ModRM byte (FPU environment)
#define MFS            36              // Memory in ModRM byte (FPU state)
#define MFX            37              // Memory in ModRM byte (ext. FPU state)
#define MSO            38              // Source in string op's ([ESI])
#define MDE            39              // Destination in string op's ([EDI])
#define MXL            40              // XLAT operand ([EBX+AL])
#define IMM            41              // Immediate data (8 or 16/32)
#define IMU            42              // Immediate unsigned data (8 or 16/32)
#define VXD            43              // VxD service
#define IMX            44              // Immediate sign-extendable byte
#define C01            45              // Implicit constant 1 (for shifts)
#define IMS            46              // Immediate byte (for shifts)
#define IM1            47              // Immediate byte
#define IM2            48              // Immediate word (ENTER/RET)
#define IMA            49              // Immediate absolute near data address
#define JOB            50              // Immediate byte offset (for jumps)
#define JOW            51              // Immediate full offset (for jumps)
#define JMF            52              // Immediate absolute far jump/call addr
#define SGM            53              // Segment register in ModRM byte
#define SCM            54              // Segment register in command byte
#define CRX            55              // Control register CRx
#define DRX            56              // Debug register DRx
#define RXM            57              // SSE register XMMx
#define MX4            58              // 4-byte memory/SSE register in ModRM byte
#define MX8            59              // 8-byte memory/SSE register in ModRM byte
#define MX0            60              // 16-byte memory/SSE register in ModRM

// Pseudooperands (implicit operands, never appear in assembler commands). Must
// have index equal to or exceeding PSEUDOOP.
#define PRN            (PSEUDOOP+0)    // Near return address
#define PRF            (PSEUDOOP+1)    // Far return address
#define PAC            (PSEUDOOP+2)    // Accumulator (AL/AX/EAX)
#define PAH            (PSEUDOOP+3)    // AH (in LAHF/SAHF commands)
#define PFL            (PSEUDOOP+4)    // Lower byte of flags (in LAHF/SAHF)
#define PS0            (PSEUDOOP+5)    // Top of FPU stack (ST(0))
#define PS1            (PSEUDOOP+6)    // ST(1)
#define PCX            (PSEUDOOP+7)    // CX/ECX
#define PDI            (PSEUDOOP+8)    // EDI (in MMX extentions)

// Errors detected during command disassembling.
#define DAE_NOERR      0               // No error
#define DAE_BADCMD     1               // Unrecognized command
#define DAE_CROSS      2               // Command crosses end of memory block
#define DAE_BADSEG     3               // Undefined segment register
#define DAE_MEMORY     4               // Register where only memory allowed
#define DAE_REGISTER   5               // Memory where only register allowed
#define DAE_INTERN     6               // Internal error

typedef unsigned char  uchar;          // Unsigned character (byte)
typedef unsigned short ushort;         // Unsigned short
typedef unsigned int   uint;           // Unsigned integer
typedef unsigned long  ulong;          // Unsigned long

////////////////////////////////////////////////////////////////////////////////
//////////////////// ASSEMBLER, DISASSEMBLER AND EXPRESSIONS ///////////////////

#define MAXCMDSIZE     16              // Maximal length of 80x86 command
#define MAXCALSIZE     8               // Max length of CALL without prefixes
#define NMODELS        8               // Number of assembler search models

#define INT3           0xCC            // Code of 1-byte breakpoint
#define NOP            0x90            // Code of 1-byte NOP command
#define TRAPFLAG       0x00000100      // Trap flag in CPU flag register

// Indexes of general-purpose registers in t_reg.
typedef enum e_reg
{
    REG_UNDEF = -1,
    REG_EAX = 0,
    REG_ECX,
    REG_EDX,
    REG_EBX,
    REG_ESP,
    REG_EBP,
    REG_ESI,
    REG_EDI,
    REG_AX,
    REG_CX,
    REG_DX,
    REG_BX,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,
    REG_AL,
    REG_CL,
    REG_DL,
    REG_BL,
    REG_AH,
    REG_CH,
    REG_DH,
    REG_BH,

    // Segment registers
    REG_ES,
    REG_CS,
    REG_SS,
    REG_DS,
    REG_FS,
    REG_GS,

    // FPU registers
    REG_ST0,
    REG_ST1,
    REG_ST2,
    REG_ST3,
    REG_ST4,
    REG_ST5,
    REG_ST6,
    REG_ST7,

    // MMX / 3DNow registers
    REG_MM0,
    REG_MM1,
    REG_MM2,
    REG_MM3,
    REG_MM4,
    REG_MM5,
    REG_MM6,
    REG_MM7,

    // SSE registeres
    REG_XMM0,
    REG_XMM1,
    REG_XMM2,
    REG_XMM3,
    REG_XMM4,
    REG_XMM5,
    REG_XMM6,
    REG_XMM7,

    // Ring 0 control registers
    REG_CR0,
    REG_CR1,
    REG_CR2,
    REG_CR3,
    REG_CR4,
    REG_CR5,
    REG_CR6,
    REG_CR7,

    // Debug registers
    REG_DR0,
    REG_DR1,
    REG_DR2,
    REG_DR3,
    REG_DR4,
    REG_DR5,
    REG_DR6,
    REG_DR7,

    // Not real registers, but valid disp types
    REG_BX_SI,      // BX+SI
    REG_BP_SI,      // BP+SI
    REG_BX_DI,      // BX+DI
    REG_BP_DI,      // BP+DI
} e_reg;

// Indexes of segment/selector registers
typedef enum e_seg
{
    SEG_UNDEF = -1,
    SEG_ES = 0,
    SEG_CS,
    SEG_SS,
    SEG_DS,
    SEG_FS,
    SEG_GS
} e_seg;

typedef enum e_inst
{
    I_NONE,
    I_AAA,
    I_AAS,
    I_AAM,
    I_AAD,
    I_ADC,
    I_ADD,
    I_AND,
    I_ARPL,
    I_BOUND,
    I_BSF,
    I_BSR,
    I_BSWAP,
    I_BT,
    I_BTC,
    I_BTR,
    I_BTS,
    I_CALL,
    I_CLC,
    I_CLD,
    I_CLI,
    I_CLTS,
    I_CMC,
    I_CMOVA,
    I_CMOVBE,
    I_CMOVC,
    I_CMOVG,
    I_CMOVGE,
    I_CMOVL,
    I_CMOVLE,
    I_CMOVNC,
    I_CMOVNO,
    I_CMOVNP,
    I_CMOVNS,
    I_CMOVNZ,
    I_CMOVO,
    I_CMOVP,
    I_CMOVS,
    I_CMOVZ,
    I_CMP,
    I_CMPS,
    I_CMPXCHG,
    I_CMPXCHG8B,
    I_CPUID,
    I_CVT,  // Actually CBW/CWDE/CWD/CDQ
    I_DAA,
    I_DAS,
    I_DEC,
    I_DIV,
    I_ENTER,
    I_HLT,
    I_IDIV,
    I_IMUL,
    I_IN,
    I_INC,
    I_INS,
    I_INT,
    I_INT3,
    I_INTO,
    I_INVD,
    I_INVLPG,
    I_IRET,
    I_JA,
    I_JBE,
    I_JC,
    I_JCXZ,
    I_JG,
    I_JGE,
    I_JL,
    I_JLE,
    I_JMP,
    I_JNC,
    I_JNO,
    I_JNP,
    I_JNS,
    I_JNZ,
    I_JO,
    I_JP,
    I_JS,
    I_JZ,
    I_LAHF,
    I_LAR,
    I_LDS,
    I_LES,
    I_LFS,
    I_LGS,
    I_LSL,
    I_LSS,
    I_LEA,
    I_LEAVE,
    I_LGDT,
    I_LIDT,
    I_LLDT,
    I_LMSW,
    I_LTR,
    I_LODS,
    I_LOOP,
    I_LOOPE,
    I_LOOPNE,
    I_MOV,
    I_MOVS,
    I_MOVSX,
    I_MOVZX,
    I_MUL,
    I_NEG,
    I_NOP,
    I_NOT,
    I_OR,
    I_OUT,
    I_OUTS,
    I_POP,
    I_POPA,
    I_POPF,
    I_PUSH,
    I_PUSHA,
    I_PUSHF,
    I_REP_INS,
    I_REP_LODS,
    I_REP_MOVS,
    I_REP_OUTS,
    I_REP_STOS,
    I_REPE_CMPS,
    I_REPE_SCAS,
    I_REPNE_CMPS,
    I_REPNE_SCAS,
    I_RET,
    I_RETF,
    I_RETN,
    I_RCL,
    I_RCR,
    I_RDTSC,
    I_RDMSR,
    I_RDPMC,
    I_ROL,
    I_ROR,
    I_RSM,
    I_SAHF,
    I_SALC,
    I_SAR,
    I_SBB,
    I_SCAS,
    I_SETA,
    I_SETBE,
    I_SETC,
    I_SETG,
    I_SETGE,
    I_SETL,
    I_SETLE,
    I_SETNC,
    I_SETNO,
    I_SETNP,
    I_SETNS,
    I_SETNZ,
    I_SETO,
    I_SETP,
    I_SETS,
    I_SETZ,
    I_SGDT,
    I_SHL,
    I_SHLD,
    I_SHR,
    I_SHRD,
    I_SIDT,
    I_SMSW,
    I_SLDT,
    I_STC,
    I_STD,
    I_STI,
    I_STOS,
    I_STR,
    I_SUB,
    I_SYSENTER,
    I_SYSEXIT,
    I_TEST,
    I_UD2,
    I_VERR,
    I_VERW,
    I_WAIT,
    I_WBINVD,
    I_WRMSR,
    I_XADD,
    I_XCHG,
    I_XLAT,
    I_XOR,

    // FPU instructions
    I_F2XM1,
    I_FABS,
    I_FADD,
    I_FADDP,
    I_FBLD,
    I_FBSTP,
    I_FCHS,
    I_FCLEX,
    I_FCMOVB,
    I_FCMOVBE,
    I_FCMOVE,
    I_FCMOVNB,
    I_FCMOVNBE,
    I_FCMOVNE,
    I_FCMOVNU,
    I_FCMOVU,
    I_FCOM,
    I_FCOMI,
    I_FCOMIP,
    I_FCOMP,
    I_FCOMPP,
    I_FCOS,
    I_FDECSTP,
    I_FDISI,
    I_FDIV,
    I_FDIVP,
    I_FDIVR,
    I_FDIVRP,
    I_FENI,
    I_FFREE,
    I_FIADD,
    I_FICOM,
    I_FICOMP,
    I_FIDIV,
    I_FIDIVR,
    I_FILD,
    I_FIMUL,
    I_FINCSTP,
    I_FINIT,
    I_FIST,
    I_FISTP,
    I_FISUB,
    I_FISUBR,
    I_FLD,
    I_FLD1,
    I_FLDCW,
    I_FLDENV,
    I_FLDL2E,
    I_FLDL2T,
    I_FLDLG2,
    I_FLDLN2,
    I_FLDPI,
    I_FLDZ,
    I_FMUL,
    I_FMULP,
    I_FNOP,
    I_FPATAN,
    I_FPREM,
    I_FPREM1,
    I_FPTAN,
    I_FRNDINT,
    I_FRSTOR,
    I_FSAVE,
    I_FSCALE,
    I_FSIN,
    I_FSINCOS,
    I_FSQRT,
    I_FST,
    I_FSTCW,
    I_FSTENV,
    I_FSTP,
    I_FSTSW,
    I_FSUB,
    I_FSUBP,
    I_FSUBR,
    I_FSUBRP,
    I_FTST,
    I_FUCOM,
    I_FUCOMI,
    I_FUCOMIP,
    I_FUCOMP,
    I_FUCOMPP,
    I_FXAM,
    I_FXCH,
    I_FXRSTOR,
    I_FXSAVE,
    I_FXTRACT,
    I_FYL2X,
    I_FYL2XP1,

    // MMX instructions
    I_EMMS,
    I_MOVD,
    I_MOVQ,
    I_PACKSSDW,
    I_PACKSSWB,
    I_PACKUSWB,
    I_PADDB,
    I_PADDD,
    I_PADDSB,
    I_PADDSW,
    I_PADDUSB,
    I_PADDUSW,
    I_PADDW,
    I_PAND,
    I_PANDN,
    I_PCMPEQB,
    I_PCMPEQD,
    I_PCMPEQW,
    I_PCMPGTB,
    I_PCMPGTD,
    I_PCMPGTW,
    I_PMADDWD,
    I_PMULHW,
    I_PMULLW,
    I_POR,
    I_PSLLD,
    I_PSLLQ,
    I_PSLLW,
    I_PSRAD,
    I_PSRAW,
    I_PSRLD,
    I_PSRLQ,
    I_PSRLW,
    I_PSUBB,
    I_PSUBD,
    I_PSUBSB,
    I_PSUBSW,
    I_PSUBUSB,
    I_PSUBUSW,
    I_PSUBW,
    I_PUNPCKHBW,
    I_PUNPCKHDQ,
    I_PUNPCKHWD,
    I_PUNPCKLBW,
    I_PUNPCKLDQ,
    I_PUNPCKLWD,
    I_PXOR,

    // AMD MMX
    I_FEMMS,
    I_MASKMOVQ,
    I_MOVNTQ,
    I_PAVGB,
    I_PAVGW,
    I_PEXTRW,
    I_PINSRW,
    I_PMAXSW,
    I_PMAXUB,
    I_PMINSW,
    I_PMINUB,
    I_PMOVMSKB,
    I_PMULHUW,
    I_PREFETCH,
    I_PREFETCHNTA,
    I_PREFETCHT0,
    I_PREFETCHT1,
    I_PREFETCHT2,
    I_PREFETCHW,
    I_PSADBW,
    I_PSHUFW,
    I_SFENCE,

    // 3DNow
    I_PAVGUSB,
    I_PF2ID,
    I_PF2IW,
    I_PFACC,
    I_PFADD,
    I_PFCMPEQ,
    I_PFCMPGE,
    I_PFCMPGT,
    I_PFMAX,
    I_PFMIN,
    I_PFMUL,
    I_PFNACC,
    I_PFPNACC,
    I_PFRCP,
    I_PFRCPIT1,
    I_PFRCPIT2,
    I_PFRSQIT1,
    I_PFRSQRT,
    I_PFSUB,
    I_PFSUBR,
    I_PI2FD,
    I_PI2FW,
    I_PMULHRW,
    I_PSWAPD,
    I_VXDCALL,

    // SSE
    I_CVTSI2SS,
    I_CVTSI2SD,
    I_CVTSS2SD,
    I_CVTSD2SS,
    I_CVTTSS2SI,
    I_CVTTSD2SI,
    I_MOVSS,
    I_MOVSD,
    I_ADDSS,
    I_ADDSD,
    I_SUBSS,
    I_SUBSD,
    I_MULSS,
    I_MULSD,
    I_DIVSS,
    I_DIVSD,
    I_UCOMISS,
    I_UCOMISD,
    I_MOVAPS,
    I_MOVAPD
} e_inst;

#define C_TYPEMASK     0x1F0            // Mask for command type
#define   C_CMD        0x000            // Ordinary instruction
#define   C_PSH        0x010            // 1-word PUSH instruction
#define   C_POP        0x020            // 1-word POP instruction
#define   C_MMX        0x030            // MMX instruction
#define   C_FLT        0x040            // FPU instruction
#define   C_JMP        0x050            // JUMP instruction
#define   C_JMC        0x060            // Conditional JUMP instruction
#define   C_CAL        0x070            // CALL instruction
#define   C_RET        0x080            // RET instruction
#define   C_FLG        0x090            // Changes system flags
#define   C_RTF        0x0A0            // C_JMP and C_FLG simultaneously
#define   C_REP        0x0B0            // Instruction with REPxx prefix
#define   C_PRI        0x0C0            // Privileged instruction
#define   C_DAT        0x0D0            // Data (address) doubleword
#define   C_NOW        0x0E0            // 3DNow! instruction
#define   C_BAD        0x0F0            // Unrecognized command
#define   C_SSE        0x110            // SSE instruction
#define C_RARE         0x08            // Rare command, seldom used in programs
#define C_SIZEMASK     0x07            // MMX data size or special flag
#define   C_EXPL       0x01            // (non-MMX) Specify explicit memory size

#define C_DANGER95     0x01            // Command is dangerous under Win95/98
#define C_DANGER       0x03            // Command is dangerous everywhere
#define C_DANGERLOCK   0x07            // Dangerous with LOCK prefix

#define DEC_TYPEMASK   0x1F            // Type of memory byte
#define   DEC_UNKNOWN  0x00            // Unknown type
#define   DEC_BYTE     0x01            // Accessed as byte
#define   DEC_WORD     0x02            // Accessed as short
#define   DEC_NEXTDATA 0x03            // Subsequent byte of code or data
#define   DEC_DWORD    0x04            // Accessed as long
#define   DEC_FLOAT4   0x05            // Accessed as float
#define   DEC_FWORD    0x06            // Accessed as descriptor/long pointer
#define   DEC_FLOAT8   0x07            // Accessed as double
#define   DEC_QWORD    0x08            // Accessed as 8-byte integer
#define   DEC_FLOAT10  0x09            // Accessed as long double
#define   DEC_TBYTE    0x0A            // Accessed as 10-byte integer
#define   DEC_STRING   0x0B            // Zero-terminated ASCII string
#define   DEC_UNICODE  0x0C            // Zero-terminated UNICODE string
#define   DEC_3DNOW    0x0D            // Accessed as 3Dnow operand
#define   DEC_DQWORD   0x0E            // Accessed as 16-byte integer
#define   DEC_BYTESW   0x11            // Accessed as byte index to switch
#define   DEC_NEXTCODE 0x13            // Subsequent byte of command
#define   DEC_COMMAND  0x1D            // First byte of command
#define   DEC_JMPDEST  0x1E            // Jump destination
#define   DEC_CALLDEST 0x1F            // Call (and maybe jump) destination
#define DEC_PROCMASK   0x60            // Procedure analysis
#define   DEC_PROC     0x20            // Start of procedure
#define   DEC_PBODY    0x40            // Body of procedure
#define   DEC_PEND     0x60            // End of procedure
#define DEC_CHECKED    0x80            // Byte was analysed

#define DECR_TYPEMASK  0x3F            // Type of register or memory
#define   DECR_BYTE    0x21            // Byte register
#define   DECR_WORD    0x22            // Short integer register
#define   DECR_DWORD   0x24            // Long integer register
#define   DECR_QWORD   0x28            // MMX register
#define   DECR_FLOAT10 0x29            // Floating-point register
#define   DECR_SEG     0x2A            // Segment register
#define   DECR_3DNOW   0x2D            // 3Dnow! register
#define DECR_ISREG     0x20            // Mask to check that operand is register

#define DISASM_SIZE    0               // Determine command size only
#define DISASM_DATA    1               // Determine size and analysis data
#define DISASM_FILE    3               // Disassembly, no symbols
#define DISASM_CODE    4               // Full disassembly

// Warnings issued by Disasm():
#define DAW_FARADDR    0x0001          // Command is a far jump, call or return
#define DAW_SEGMENT    0x0002          // Command loads segment register
#define DAW_PRIV       0x0004          // Privileged command
#define DAW_IO         0x0008          // I/O command
#define DAW_SHIFT      0x0010          // Shift constant out of range 1..31
#define DAW_PREFIX     0x0020          // Superfluous prefix
#define DAW_LOCK       0x0040          // Command has LOCK prefix
#define DAW_STACK      0x0080          // Unaligned stack operation
#define DAW_DANGER95   0x1000          // May mess up Win95 if executed
#define DAW_DANGEROUS  0x3000          // May mess up any OS if executed

typedef struct t_addrdec
{
    int            defseg;
    e_reg          id;
} t_addrdec;

typedef struct t_cmddata
{
    ulong          mask;                 // Mask for first 4 bytes of the command
    ulong          code;                 // Compare masked bytes with this
    uchar          len;                  // Length of the main command code
    uchar          bits;                 // Special bits within the command
    uchar          arg1, arg2, arg3;     // Types of possible arguments
    ushort         type;                 // C_xxx + additional information
    e_inst         inst;                 // Symbolic name for this command
} t_cmddata;

typedef struct t_arg
{
    e_reg base;                          // Base register
    e_reg idx;                           // Index register
    union
    {
        long disp;                       // Displacement, if idx is not REG_UNDEF
        ulong addr;                      // Otherwise linear address
    };
    int ptrsize;                         // Size of pointer, 0 for register arguments
    int scale;                           // Scale factor for index register
    e_seg seg;                           // Segment override
                                         // Negative numbers indicate specific jump types
    ushort segaddr;                      // Absolute segment address for 16-bit mode
} t_arg;

typedef struct t_disasm
{              // Results of disassembling
    // Included t_asm structure BEGIN
    e_inst         inst;                 // Instruction itself
    int            nargs;                // Number of (real) arguments
    t_arg          arg[3];               // Instruction arguments, see cmddata for types
    int            lock;                 // Instruction has a lock prefix
    // Included t_asm structure END

    // Code analysis data
    ulong          ip;                   // Instrucion pointer
    int            cmdtype;              // One of C_xxx
    int            memtype;              // Type of addressed variable in memory
    int            nprefix;              // Number of prefixes
    int            indexed;              // Address contains register(s)
    ulong          jmpconst;             // Constant jump address
    ulong          jmptable;             // Possible address of switch table
    ulong          adrconst;             // Constant part of address
    ulong          immconst;             // Immediate constant
    int            zeroconst;            // Whether contains zero constant
    int            fixupoffset;          // Possible offset of 32-bit fixups
    int            fixupsize;            // Possible total size of fixups or 0
    int            error;                // Error while disassembling command
    int            warnings;             // Combination of DAW_xxx
    const t_cmddata *command;            // Full command data
} t_disasm;

typedef struct t_asm
{              // Command to assemble
    e_inst         inst;                 // Instruction itself
    int            nargs;                // Number of (real) arguments
    t_arg          arg[3];               // Instruction arguments
    int            lock;                 // Instruction has a lock prefix
} t_asm;

typedef struct t_asmmodel
{              // Model to search for assembler command
    uchar          code[MAXCMDSIZE];     // Binary code
    uchar          mask[MAXCMDSIZE];     // Mask for binary code (0: bit ignored)
    int            length;               // Length of code, bytes (0: empty)
    int            jmpsize;              // Offset size if relative jump
    int            jmpoffset;            // Offset relative to IP
    int            jmppos;               // Position of jump offset in command
} t_asmmodel;

// Initialized constant data structures used by all programs from assembler
// package. Contain names of register, register combinations or commands and
// their properties.
extern const e_reg     regid[3][8];
extern const t_addrdec addr16[8];
extern const t_addrdec addr32[8];
extern const t_cmddata cmddata[];
extern const t_cmddata vxdcmd;
extern const t_cmddata dangerous[];

extern const t_asm empty_asm;

unique int       shortstringcmds;      // Use short form of string commands
unique int       decodevxd;            // Decode VxD calls (Win95/98)
unique int       iswindowsnt;          // When checking for dangers, assume NT

int    Assemble(const t_asm *cmd, ulong ip, t_asmmodel *model, int attempt,
                int constsize, char *errtext);
int    Checkcondition(int code, ulong flags);
ulong  Disasm(uchar *src, ulong srcsize, ulong srcip,
              t_disasm *disasm, int disasmmode);
ulong  Disassembleback(uchar *block, ulong base, ulong size, ulong ip, int n);
ulong  Disassembleforward(uchar *block, ulong base, ulong size, ulong ip, int n);
int    Isfilling(ulong addr, uchar *data, ulong size, ulong align);


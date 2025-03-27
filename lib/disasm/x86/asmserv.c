// Free Disassembler and Assembler -- Command data and service routines
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
//
// 05.03.2002: Corrected error, FSTSW AX assembled with data size prefix
// clang-format off

#define STRICT

#include <ctype.h>
#include "minicrt.h"
#pragma hdrstop

#include "disasm.h"

const e_reg regid[3][8] = {
  { REG_AL, REG_CL, REG_DL, REG_BL, REG_AH, REG_CH, REG_DH, REG_BH },
  { REG_AX, REG_CX, REG_DX, REG_BX, REG_SP, REG_BP, REG_SI, REG_DI },
  { REG_EAX,REG_ECX,REG_EDX,REG_EBX,REG_ESP,REG_EBP,REG_ESI,REG_EDI } };

const t_addrdec addr16[8] = {
  { SEG_DS,REG_BX_SI },{ SEG_DS,REG_BX_DI },
  { SEG_SS,REG_BP_SI },{ SEG_SS,REG_BP_DI },
  { SEG_DS,REG_SI },{ SEG_DS,REG_DI },
  { SEG_SS,REG_BP },{ SEG_DS,REG_BX } };

const t_addrdec addr32[8] = {
  { SEG_DS,REG_EAX },{ SEG_DS,REG_ECX },
  { SEG_DS,REG_EDX },{ SEG_DS,REG_EBX },
  { SEG_SS,REG_UNDEF },{ SEG_SS,REG_EBP },
  { SEG_DS,REG_ESI },{ SEG_DS,REG_EDI } };

// List of available processor commands with decoding, types of parameters and
// other useful information. Last element has field mask=0. If mnemonic begins
// with ampersand ('&'), its mnemonic decodes differently depending on operand
// size (16 or 32 bits). If mnemonic begins with dollar ('$'), this mnemonic
// depends on address size. Semicolon (':') separates 16-bit form from 32-bit,
// asterisk ('*') will be substituted by either W (16), D (32) or none (16/32)
// character. If command is of type C_MMX or C_NOW, or if type contains C_EXPL
// (=0x01), Disassembler must specify explicit size of memory operand.
const t_cmddata cmddata[] = {
    { 0x0000FF, 0x000090, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_NOP },
    { 0x0000FE, 0x00008A, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_MOV },
    { 0x0000F8, 0x000050, 1,00,  RCM,NNN,NNN, C_PSH + 0,          I_PUSH },
    { 0x0000FE, 0x000088, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_MOV },
    { 0x0000FF, 0x0000E8, 1,00,  JOW,NNN,NNN, C_CAL + 0,          I_CALL },
    { 0x0000FD, 0x000068, 1,SS,  IMM,NNN,NNN, C_PSH + 0,          I_PUSH },
    { 0x0000FF, 0x00008D, 1,00,  REG,MMA,NNN, C_CMD + 0,          I_LEA },
    { 0x0000FF, 0x000074, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JZ },
    { 0x0000F8, 0x000058, 1,00,  RCM,NNN,NNN, C_POP + 0,          I_POP },
    { 0x0038FC, 0x000080, 1,WS,  MRG,IMM,NNN, C_CMD + 1,          I_ADD },
    { 0x0000FF, 0x000075, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JNZ },
    { 0x0000FF, 0x0000EB, 1,00,  JOB,NNN,NNN, C_JMP + 0,          I_JMP },
    { 0x0000FF, 0x0000E9, 1,00,  JOW,NNN,NNN, C_JMP + 0,          I_JMP },
    { 0x0000FE, 0x000084, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_TEST },
    { 0x0038FE, 0x0000C6, 1,WW,  MRG,IMM,NNN, C_CMD + 1,          I_MOV },
    { 0x0000FE, 0x000032, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_XOR },
    { 0x0000FE, 0x00003A, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_CMP },
    { 0x0038FC, 0x003880, 1,WS,  MRG,IMM,NNN, C_CMD + 1,          I_CMP },
    { 0x0038FF, 0x0010FF, 1,00,  MRJ,NNN,NNN, C_CAL + 0,          I_CALL },
    { 0x0000FF, 0x0000C3, 1,00,  PRN,NNN,NNN, C_RET + 0,          I_RET },
    { 0x0000F0, 0x0000B0, 1,W3,  RCM,IMM,NNN, C_CMD + 0,          I_MOV },
    { 0x0000FE, 0x0000A0, 1,WW,  RAC,IMA,NNN, C_CMD + 0,          I_MOV },
    { 0x00FFFF, 0x00840F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JZ },
    { 0x0000F8, 0x000040, 1,00,  RCM,NNN,NNN, C_CMD + 0,          I_INC },
    { 0x0038FE, 0x0000F6, 1,WW,  MRG,IMU,NNN, C_CMD + 1,          I_TEST },
    { 0x0000FE, 0x0000A2, 1,WW,  IMA,RAC,NNN, C_CMD + 0,          I_MOV },
    { 0x0000FE, 0x00002A, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_SUB },
    { 0x0000FF, 0x00007E, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JLE },
    { 0x00FFFF, 0x00850F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JNZ },
    { 0x0000FF, 0x0000C2, 1,00,  IM2,PRN,NNN, C_RET + 0,          I_RETN },
    { 0x0038FF, 0x0030FF, 1,00,  MRG,NNN,NNN, C_PSH + 1,          I_PUSH },
    { 0x0038FC, 0x000880, 1,WS,  MRG,IMU,NNN, C_CMD + 1,          I_OR },
    { 0x0038FC, 0x002880, 1,WS,  MRG,IMM,NNN, C_CMD + 1,          I_SUB },
    { 0x0000F8, 0x000048, 1,00,  RCM,NNN,NNN, C_CMD + 0,          I_DEC },
    { 0x00FFFF, 0x00BF0F, 2,00,  REG,MR2,NNN, C_CMD + 1,          I_MOVSX },
    { 0x0000FF, 0x00007C, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JL },
    { 0x0000FE, 0x000002, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_ADD },
    { 0x0038FC, 0x002080, 1,WS,  MRG,IMU,NNN, C_CMD + 1,          I_AND },
    { 0x0000FE, 0x00003C, 1,WW,  RAC,IMM,NNN, C_CMD + 0,          I_CMP },
    { 0x0038FF, 0x0020FF, 1,00,  MRJ,NNN,NNN, C_JMP + 0,          I_JMP },
    { 0x0038FE, 0x0010F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_NOT },
    { 0x0038FE, 0x0028C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_SHR },
    { 0x0000FE, 0x000038, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_CMP },
    { 0x0000FF, 0x00007D, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JGE },
    { 0x0000FF, 0x00007F, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JG },
    { 0x0038FE, 0x0020C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_SHL },
    { 0x0000FE, 0x00001A, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_SBB },
    { 0x0038FE, 0x0018F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_NEG },
    { 0x0000FF, 0x0000C9, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_LEAVE },
    { 0x0000FF, 0x000060, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_PUSHA },
    { 0x0038FF, 0x00008F, 1,00,  MRG,NNN,NNN, C_POP + 1,          I_POP },
    { 0x0000FF, 0x000061, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_POPA },
    { 0x0000F8, 0x000090, 1,00,  RAC,RCM,NNN, C_CMD + 0,          I_XCHG },
    { 0x0000FE, 0x000086, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_XCHG },
    { 0x0000FE, 0x000000, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_ADD },
    { 0x0000FE, 0x000010, 1,WW,  MRG,REG,NNN, C_CMD + C_RARE + 0, I_ADC },
    { 0x0000FE, 0x000012, 1,WW,  REG,MRG,NNN, C_CMD + C_RARE + 0, I_ADC },
    { 0x0000FE, 0x000020, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_AND },
    { 0x0000FE, 0x000022, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_AND },
    { 0x0000FE, 0x000008, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_OR },
    { 0x0000FE, 0x00000A, 1,WW,  REG,MRG,NNN, C_CMD + 0,          I_OR },
    { 0x0000FE, 0x000028, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_SUB },
    { 0x0000FE, 0x000018, 1,WW,  MRG,REG,NNN, C_CMD + C_RARE + 0, I_SBB },
    { 0x0000FE, 0x000030, 1,WW,  MRG,REG,NNN, C_CMD + 0,          I_XOR },
    { 0x0038FC, 0x001080, 1,WS,  MRG,IMM,NNN, C_CMD + C_RARE + 1, I_ADC },
    { 0x0038FC, 0x001880, 1,WS,  MRG,IMM,NNN, C_CMD + C_RARE + 1, I_SBB },
    { 0x0038FC, 0x003080, 1,WS,  MRG,IMU,NNN, C_CMD + 1,          I_XOR },
    { 0x0000FE, 0x000004, 1,WW,  RAC,IMM,NNN, C_CMD + 0,          I_ADD },
    { 0x0000FE, 0x000014, 1,WW,  RAC,IMM,NNN, C_CMD + C_RARE + 0, I_ADC },
    { 0x0000FE, 0x000024, 1,WW,  RAC,IMU,NNN, C_CMD + 0,          I_AND },
    { 0x0000FE, 0x00000C, 1,WW,  RAC,IMU,NNN, C_CMD + 0,          I_OR },
    { 0x0000FE, 0x00002C, 1,WW,  RAC,IMM,NNN, C_CMD + 0,          I_SUB },
    { 0x0000FE, 0x00001C, 1,WW,  RAC,IMM,NNN, C_CMD + C_RARE + 0, I_SBB },
    { 0x0000FE, 0x000034, 1,WW,  RAC,IMU,NNN, C_CMD + 0,          I_XOR },
    { 0x0038FE, 0x0000FE, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_INC },
    { 0x0038FE, 0x0008FE, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_DEC },
    { 0x0000FE, 0x0000A8, 1,WW,  RAC,IMU,NNN, C_CMD + 0,          I_TEST },
    { 0x0038FE, 0x0020F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_MUL },
    { 0x0038FE, 0x0028F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_IMUL },
    { 0x00FFFF, 0x00AF0F, 2,00,  REG,MRG,NNN, C_CMD + 0,          I_IMUL },
    { 0x0000FF, 0x00006B, 1,00,  REG,MRG,IMX, C_CMD + C_RARE + 0, I_IMUL },
    { 0x0000FF, 0x000069, 1,00,  REG,MRG,IMM, C_CMD + C_RARE + 0, I_IMUL },
    { 0x0038FE, 0x0030F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_DIV },
    { 0x0038FE, 0x0038F6, 1,WW,  MRG,NNN,NNN, C_CMD + 1,          I_IDIV },
    { 0x0000FF, 0x000098, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_CVT },
    { 0x0000FF, 0x000099, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_CVT },
    { 0x0038FE, 0x0000D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_ROL },
    { 0x0038FE, 0x0008D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_ROR },
    { 0x0038FE, 0x0010D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_RCL },
    { 0x0038FE, 0x0018D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_RCR },
    { 0x0038FE, 0x0020D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_SHL },
    { 0x0038FE, 0x0028D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_SHR },
    { 0x0038FE, 0x0038D0, 1,WW,  MRG,C01,NNN, C_CMD + 1,          I_SAR },
    { 0x0038FE, 0x0000D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_ROL },
    { 0x0038FE, 0x0008D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_ROR },
    { 0x0038FE, 0x0010D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_RCL },
    { 0x0038FE, 0x0018D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_RCR },
    { 0x0038FE, 0x0020D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_SHL },
    { 0x0038FE, 0x0028D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_SHR },
    { 0x0038FE, 0x0038D2, 1,WW,  MRG,RCL,NNN, C_CMD + 1,          I_SAR },
    { 0x0038FE, 0x0000C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_ROL },
    { 0x0038FE, 0x0008C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_ROR },
    { 0x0038FE, 0x0010C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_RCL },
    { 0x0038FE, 0x0018C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_RCR },
    { 0x0038FE, 0x0038C0, 1,WW,  MRG,IMS,NNN, C_CMD + 1,          I_SAR },
    { 0x0000FF, 0x000070, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JO },
    { 0x0000FF, 0x000071, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JNO },
    { 0x0000FF, 0x000072, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JC },
    { 0x0000FF, 0x000073, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JNC },
    { 0x0000FF, 0x000076, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JBE },
    { 0x0000FF, 0x000077, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JA },
    { 0x0000FF, 0x000078, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JS },
    { 0x0000FF, 0x000079, 1,CC,  JOB,NNN,NNN, C_JMC + 0,          I_JNS },
    { 0x0000FF, 0x00007A, 1,CC,  JOB,NNN,NNN, C_JMC + C_RARE + 0, I_JP },
    { 0x0000FF, 0x00007B, 1,CC,  JOB,NNN,NNN, C_JMC + C_RARE + 0, I_JNP },
    { 0x0000FF, 0x0000E3, 1,00,  JOB,NNN,NNN, C_JMC + C_RARE + 0, I_JCXZ },
    { 0x00FFFF, 0x00800F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JO },
    { 0x00FFFF, 0x00810F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JNO },
    { 0x00FFFF, 0x00820F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JC },
    { 0x00FFFF, 0x00830F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JNC },
    { 0x00FFFF, 0x00860F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JBE },
    { 0x00FFFF, 0x00870F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JA },
    { 0x00FFFF, 0x00880F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JS },
    { 0x00FFFF, 0x00890F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JNS },
    { 0x00FFFF, 0x008A0F, 2,CC,  JOW,NNN,NNN, C_JMC + C_RARE + 0, I_JP },
    { 0x00FFFF, 0x008B0F, 2,CC,  JOW,NNN,NNN, C_JMC + C_RARE + 0, I_JNP },
    { 0x00FFFF, 0x008C0F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JL },
    { 0x00FFFF, 0x008D0F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JGE },
    { 0x00FFFF, 0x008E0F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JLE },
    { 0x00FFFF, 0x008F0F, 2,CC,  JOW,NNN,NNN, C_JMC + 0,          I_JG },
    { 0x0000FF, 0x0000F8, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_CLC },
    { 0x0000FF, 0x0000F9, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_STC },
    { 0x0000FF, 0x0000F5, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_CMC },
    { 0x0000FF, 0x0000FC, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_CLD },
    { 0x0000FF, 0x0000FD, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_STD },
    { 0x0000FF, 0x0000FA, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_CLI },
    { 0x0000FF, 0x0000FB, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_STI },
    { 0x0000FF, 0x00008C, 1,FF,  MRG,SGM,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0x0000FF, 0x00008E, 1,FF,  SGM,MRG,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0x0000FE, 0x0000A6, 1,WW,  MSO,MDE,NNN, C_CMD + 1,          I_CMPS },
    { 0x0000FE, 0x0000AC, 1,WW,  MSO,NNN,NNN, C_CMD + 1,          I_LODS },
    { 0x0000FE, 0x0000A4, 1,WW,  MDE,MSO,NNN, C_CMD + 1,          I_MOVS },
    { 0x0000FE, 0x0000AE, 1,WW,  MDE,PAC,NNN, C_CMD + 1,          I_SCAS },
    { 0x0000FE, 0x0000AA, 1,WW,  MDE,PAC,NNN, C_CMD + 1,          I_STOS },
    { 0x00FEFF, 0x00A4F3, 1,WW,  MDE,MSO,PCX, C_REP + 1,          I_REP_MOVS },
    { 0x00FEFF, 0x00ACF3, 1,WW,  MSO,PAC,PCX, C_REP + C_RARE + 1, I_REP_LODS },
    { 0x00FEFF, 0x00AAF3, 1,WW,  MDE,PAC,PCX, C_REP + 1,          I_REP_STOS },
    { 0x00FEFF, 0x00A6F3, 1,WW,  MDE,MSO,PCX, C_REP + 1,          I_REPE_CMPS },
    { 0x00FEFF, 0x00AEF3, 1,WW,  MDE,PAC,PCX, C_REP + 1,          I_REPE_SCAS },
    { 0x00FEFF, 0x00A6F2, 1,WW,  MDE,MSO,PCX, C_REP + 1,          I_REPNE_CMPS },
    { 0x00FEFF, 0x00AEF2, 1,WW,  MDE,PAC,PCX, C_REP + 1,          I_REPNE_SCAS },
    { 0x0000FF, 0x0000EA, 1,00,  JMF,NNN,NNN, C_JMP + C_RARE + 0, I_JMP },
    { 0x0038FF, 0x0028FF, 1,00,  MMS,NNN,NNN, C_JMP + C_RARE + 1, I_JMP },
    { 0x0000FF, 0x00009A, 1,00,  JMF,NNN,NNN, C_CAL + C_RARE + 0, I_CALL },
    { 0x0038FF, 0x0018FF, 1,00,  MMS,NNN,NNN, C_CAL + C_RARE + 1, I_CALL },
    { 0x0000FF, 0x0000CB, 1,00,  PRF,NNN,NNN, C_RET + C_RARE + 0, I_RETF },
    { 0x0000FF, 0x0000CA, 1,00,  IM2,PRF,NNN, C_RET + C_RARE + 0, I_RETF },
    { 0x00FFFF, 0x00A40F, 2,00,  MRG,REG,IMS, C_CMD + 0,          I_SHLD },
    { 0x00FFFF, 0x00AC0F, 2,00,  MRG,REG,IMS, C_CMD + 0,          I_SHRD },
    { 0x00FFFF, 0x00A50F, 2,00,  MRG,REG,RCL, C_CMD + 0,          I_SHLD },
    { 0x00FFFF, 0x00AD0F, 2,00,  MRG,REG,RCL, C_CMD + 0,          I_SHRD },
    { 0x00F8FF, 0x00C80F, 2,00,  RCM,NNN,NNN, C_CMD + C_RARE + 0, I_BSWAP },
    { 0x00FEFF, 0x00C00F, 2,WW,  MRG,REG,NNN, C_CMD + C_RARE + 0, I_XADD },
    { 0x0000FF, 0x0000E2, 1,LL,  JOB,PCX,NNN, C_JMC + 0,          I_LOOP },
    { 0x0000FF, 0x0000E1, 1,LL,  JOB,PCX,NNN, C_JMC + 0,          I_LOOPE },
    { 0x0000FF, 0x0000E0, 1,LL,  JOB,PCX,NNN, C_JMC + 0,          I_LOOPNE },
    { 0x0000FF, 0x0000C8, 1,00,  IM2,IM1,NNN, C_CMD + 0,          I_ENTER },
    { 0x0000FE, 0x0000E4, 1,WP,  RAC,IM1,NNN, C_CMD + C_RARE + 0, I_IN },
    { 0x0000FE, 0x0000EC, 1,WP,  RAC,RDX,NNN, C_CMD + C_RARE + 0, I_IN },
    { 0x0000FE, 0x0000E6, 1,WP,  IM1,RAC,NNN, C_CMD + C_RARE + 0, I_OUT },
    { 0x0000FE, 0x0000EE, 1,WP,  RDX,RAC,NNN, C_CMD + C_RARE + 0, I_OUT },
    { 0x0000FE, 0x00006C, 1,WP,  MDE,RDX,NNN, C_CMD + C_RARE + 1, I_INS },
    { 0x0000FE, 0x00006E, 1,WP,  RDX,MDE,NNN, C_CMD + C_RARE + 1, I_OUTS },
    { 0x00FEFF, 0x006CF3, 1,WP,  MDE,RDX,PCX, C_REP + C_RARE + 1, I_REP_INS },
    { 0x00FEFF, 0x006EF3, 1,WP,  RDX,MDE,PCX, C_REP + C_RARE + 1, I_REP_OUTS },
    { 0x0000FF, 0x000037, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_AAA },
    { 0x0000FF, 0x00003F, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_AAS },
    { 0x00FFFF, 0x000AD4, 2,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_AAM },
    { 0x0000FF, 0x0000D4, 1,00,  IM1,NNN,NNN, C_CMD + C_RARE + 0, I_AAM },
    { 0x00FFFF, 0x000AD5, 2,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_AAD },
    { 0x0000FF, 0x0000D5, 1,00,  IM1,NNN,NNN, C_CMD + C_RARE + 0, I_AAD },
    { 0x0000FF, 0x000027, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_DAA },
    { 0x0000FF, 0x00002F, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_DAS },
    { 0x0000FF, 0x0000F4, 1,PR,  NNN,NNN,NNN, C_PRI + C_RARE + 0, I_HLT },
    { 0x0000FF, 0x00000E, 1,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x0000FF, 0x000016, 1,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x0000FF, 0x00001E, 1,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x0000FF, 0x000006, 1,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x00FFFF, 0x00A00F, 2,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x00FFFF, 0x00A80F, 2,00,  SCM,NNN,NNN, C_PSH + C_RARE + 0, I_PUSH },
    { 0x0000FF, 0x00001F, 1,00,  SCM,NNN,NNN, C_POP + C_RARE + 0, I_POP },
    { 0x0000FF, 0x000007, 1,00,  SCM,NNN,NNN, C_POP + C_RARE + 0, I_POP },
    { 0x0000FF, 0x000017, 1,00,  SCM,NNN,NNN, C_POP + C_RARE + 0, I_POP },
    { 0x00FFFF, 0x00A10F, 2,00,  SCM,NNN,NNN, C_POP + C_RARE + 0, I_POP },
    { 0x00FFFF, 0x00A90F, 2,00,  SCM,NNN,NNN, C_POP + C_RARE + 0, I_POP },
    { 0x0000FF, 0x0000D7, 1,00,  MXL,NNN,NNN, C_CMD + C_RARE + 1, I_XLAT },
    { 0x00FFFF, 0x00BE0F, 2,00,  REG,MR1,NNN, C_CMD + 1,          I_MOVSX },
    { 0x00FFFF, 0x00B60F, 2,00,  REG,MR1,NNN, C_CMD + 1,          I_MOVZX },
    { 0x00FFFF, 0x00B70F, 2,00,  REG,MR2,NNN, C_CMD + 1,          I_MOVZX },
    { 0x0000FF, 0x00009B, 1,00,  NNN,NNN,NNN, C_CMD + 0,          I_WAIT },
    { 0x0000FF, 0x00009F, 1,00,  PAH,PFL,NNN, C_CMD + C_RARE + 0, I_LAHF },
    { 0x0000FF, 0x00009E, 1,00,  PFL,PAH,NNN, C_CMD + C_RARE + 0, I_SAHF },
    { 0x0000FF, 0x00009C, 1,00,  NNN,NNN,NNN, C_PSH + 0,          I_PUSHF },
    { 0x0000FF, 0x00009D, 1,00,  NNN,NNN,NNN, C_FLG + 0,          I_POPF },
    { 0x0000FF, 0x0000CD, 1,00,  IM1,NNN,NNN, C_CAL + C_RARE + 0, I_INT },
    { 0x0000FF, 0x0000CC, 1,00,  NNN,NNN,NNN, C_CAL + C_RARE + 0, I_INT3 },
    { 0x0000FF, 0x0000CE, 1,00,  NNN,NNN,NNN, C_CAL + C_RARE + 0, I_INTO },
    { 0x0000FF, 0x0000CF, 1,00,  NNN,NNN,NNN, C_RTF + C_RARE + 0, I_IRET },
    { 0x00FFFF, 0x00900F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETO },
    { 0x00FFFF, 0x00910F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETNO },
    { 0x00FFFF, 0x00920F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETC },
    { 0x00FFFF, 0x00930F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETNC },
    { 0x00FFFF, 0x00940F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETZ },
    { 0x00FFFF, 0x00950F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETNZ },
    { 0x00FFFF, 0x00960F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETBE },
    { 0x00FFFF, 0x00970F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETA },
    { 0x00FFFF, 0x00980F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETS },
    { 0x00FFFF, 0x00990F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETNS },
    { 0x00FFFF, 0x009A0F, 2,CC,  MR1,NNN,NNN, C_CMD + C_RARE + 0, I_SETP },
    { 0x00FFFF, 0x009B0F, 2,CC,  MR1,NNN,NNN, C_CMD + C_RARE + 0, I_SETNP },
    { 0x00FFFF, 0x009C0F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETL },
    { 0x00FFFF, 0x009D0F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETGE },
    { 0x00FFFF, 0x009E0F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETLE },
    { 0x00FFFF, 0x009F0F, 2,CC,  MR1,NNN,NNN, C_CMD + 0,          I_SETG },
    { 0x38FFFF, 0x20BA0F, 2,00,  MRG,IM1,NNN, C_CMD + C_RARE + 1, I_BT },
    { 0x38FFFF, 0x28BA0F, 2,00,  MRG,IM1,NNN, C_CMD + C_RARE + 1, I_BTS },
    { 0x38FFFF, 0x30BA0F, 2,00,  MRG,IM1,NNN, C_CMD + C_RARE + 1, I_BTR },
    { 0x38FFFF, 0x38BA0F, 2,00,  MRG,IM1,NNN, C_CMD + C_RARE + 1, I_BTC },
    { 0x00FFFF, 0x00A30F, 2,00,  MRG,REG,NNN, C_CMD + C_RARE + 1, I_BT },
    { 0x00FFFF, 0x00AB0F, 2,00,  MRG,REG,NNN, C_CMD + C_RARE + 1, I_BTS },
    { 0x00FFFF, 0x00B30F, 2,00,  MRG,REG,NNN, C_CMD + C_RARE + 1, I_BTR },
    { 0x00FFFF, 0x00BB0F, 2,00,  MRG,REG,NNN, C_CMD + C_RARE + 1, I_BTC },
    { 0x0000FF, 0x0000C5, 1,00,  REG,MML,NNN, C_CMD + C_RARE + 0, I_LDS },
    { 0x0000FF, 0x0000C4, 1,00,  REG,MML,NNN, C_CMD + C_RARE + 0, I_LES },
    { 0x00FFFF, 0x00B40F, 2,00,  REG,MML,NNN, C_CMD + C_RARE + 0, I_LFS },
    { 0x00FFFF, 0x00B50F, 2,00,  REG,MML,NNN, C_CMD + C_RARE + 0, I_LGS },
    { 0x00FFFF, 0x00B20F, 2,00,  REG,MML,NNN, C_CMD + C_RARE + 0, I_LSS },
    { 0x0000FF, 0x000063, 1,00,  MRG,REG,NNN, C_CMD + C_RARE + 0, I_ARPL },
    { 0x0000FF, 0x000062, 1,00,  REG,MMB,NNN, C_CMD + C_RARE + 0, I_BOUND },
    { 0x00FFFF, 0x00BC0F, 2,00,  REG,MRG,NNN, C_CMD + C_RARE + 0, I_BSF },
    { 0x00FFFF, 0x00BD0F, 2,00,  REG,MRG,NNN, C_CMD + C_RARE + 0, I_BSR },
    { 0x00FFFF, 0x00060F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_CLTS },
    { 0x00FFFF, 0x00400F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVO },
    { 0x00FFFF, 0x00410F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVNO },
    { 0x00FFFF, 0x00420F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVC },
    { 0x00FFFF, 0x00430F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVNC },
    { 0x00FFFF, 0x00440F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVZ },
    { 0x00FFFF, 0x00450F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVNZ },
    { 0x00FFFF, 0x00460F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVBE },
    { 0x00FFFF, 0x00470F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVA },
    { 0x00FFFF, 0x00480F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVS },
    { 0x00FFFF, 0x00490F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVNS },
    { 0x00FFFF, 0x004A0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVP },
    { 0x00FFFF, 0x004B0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVNP },
    { 0x00FFFF, 0x004C0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVL },
    { 0x00FFFF, 0x004D0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVGE },
    { 0x00FFFF, 0x004E0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVLE },
    { 0x00FFFF, 0x004F0F, 2,CC,  REG,MRG,NNN, C_CMD + 0,          I_CMOVG },
    { 0x00FEFF, 0x00B00F, 2,WW,  MRG,REG,NNN, C_CMD + C_RARE + 0, I_CMPXCHG },
    { 0x38FFFF, 0x08C70F, 2,00,  MD8,NNN,NNN, C_CMD + C_RARE + 1, I_CMPXCHG8B },
    { 0x00FFFF, 0x00A20F, 2,00,  NNN,NNN,NNN, C_CMD + 0,          I_CPUID },
    { 0x00FFFF, 0x00080F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_INVD },
    { 0x00FFFF, 0x00020F, 2,00,  REG,MRG,NNN, C_CMD + C_RARE + 0, I_LAR },
    { 0x00FFFF, 0x00030F, 2,00,  REG,MRG,NNN, C_CMD + C_RARE + 0, I_LSL },
    { 0x38FFFF, 0x38010F, 2,PR,  MR1,NNN,NNN, C_CMD + C_RARE + 0, I_INVLPG },
    { 0x00FFFF, 0x00090F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_WBINVD },
    { 0x38FFFF, 0x10010F, 2,PR,  MM6,NNN,NNN, C_CMD + C_RARE + 0, I_LGDT },
    { 0x38FFFF, 0x00010F, 2,00,  MM6,NNN,NNN, C_CMD + C_RARE + 0, I_SGDT },
    { 0x38FFFF, 0x18010F, 2,PR,  MM6,NNN,NNN, C_CMD + C_RARE + 0, I_LIDT },
    { 0x38FFFF, 0x08010F, 2,00,  MM6,NNN,NNN, C_CMD + C_RARE + 0, I_SIDT },
    { 0x38FFFF, 0x10000F, 2,PR,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_LLDT },
    { 0x38FFFF, 0x00000F, 2,00,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_SLDT },
    { 0x38FFFF, 0x18000F, 2,PR,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_LTR },
    { 0x38FFFF, 0x08000F, 2,00,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_STR },
    { 0x38FFFF, 0x30010F, 2,PR,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_LMSW },
    { 0x38FFFF, 0x20010F, 2,00,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_SMSW },
    { 0x38FFFF, 0x20000F, 2,00,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_VERR },
    { 0x38FFFF, 0x28000F, 2,00,  MR2,NNN,NNN, C_CMD + C_RARE + 0, I_VERW },
    { 0xC0FFFF, 0xC0220F, 2,PR,  CRX,RR4,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0xC0FFFF, 0xC0200F, 2,00,  RR4,CRX,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0xC0FFFF, 0xC0230F, 2,PR,  DRX,RR4,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0xC0FFFF, 0xC0210F, 2,PR,  RR4,DRX,NNN, C_CMD + C_RARE + 0, I_MOV },
    { 0x00FFFF, 0x00310F, 2,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_RDTSC },
    { 0x00FFFF, 0x00320F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_RDMSR },
    { 0x00FFFF, 0x00300F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_WRMSR },
    { 0x00FFFF, 0x00330F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_RDPMC },
    { 0x00FFFF, 0x00AA0F, 2,PR,  NNN,NNN,NNN, C_RTF + C_RARE + 0, I_RSM },
    { 0x00FFFF, 0x000B0F, 2,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_UD2 },
    { 0x00FFFF, 0x00340F, 2,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_SYSENTER },
    { 0x00FFFF, 0x00350F, 2,PR,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_SYSEXIT },
    { 0x0000FF, 0x0000D6, 1,00,  NNN,NNN,NNN, C_CMD + C_RARE + 0, I_SALC },
    // FPU instructions. Never change the order of instructions!
    { 0x00FFFF, 0x00F0D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_F2XM1 },
    { 0x00FFFF, 0x00E0D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FCHS },
    { 0x00FFFF, 0x00E1D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FABS },
    { 0x00FFFF, 0x00E2DB, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FCLEX },
    { 0x00FFFF, 0x00E3DB, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FINIT },
    { 0x00FFFF, 0x00F6D9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FDECSTP },
    { 0x00FFFF, 0x00F7D9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FINCSTP },
    { 0x00FFFF, 0x00E4D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FTST },
    { 0x00FFFF, 0x00FAD9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FSQRT },
    { 0x00FFFF, 0x00FED9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FSIN },
    { 0x00FFFF, 0x00FFD9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FCOS },
    { 0x00FFFF, 0x00FBD9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FSINCOS },
    { 0x00FFFF, 0x00F2D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FPTAN },
    { 0x00FFFF, 0x00F3D9, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FPATAN },
    { 0x00FFFF, 0x00F8D9, 2,00,  PS1,PS0,NNN, C_FLT + 0,          I_FPREM },
    { 0x00FFFF, 0x00F5D9, 2,00,  PS1,PS0,NNN, C_FLT + 0,          I_FPREM1 },
    { 0x00FFFF, 0x00F1D9, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FYL2X },
    { 0x00FFFF, 0x00F9D9, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FYL2XP1 },
    { 0x00FFFF, 0x00FCD9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FRNDINT },
    { 0x00FFFF, 0x00E8D9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLD1 },
    { 0x00FFFF, 0x00E9D9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDL2T },
    { 0x00FFFF, 0x00EAD9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDL2E },
    { 0x00FFFF, 0x00EBD9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDPI },
    { 0x00FFFF, 0x00ECD9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDLG2 },
    { 0x00FFFF, 0x00EDD9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDLN2 },
    { 0x00FFFF, 0x00EED9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FLDZ },
    { 0x00FFFF, 0x00FDD9, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FSCALE },
    { 0x00FFFF, 0x00D0D9, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FNOP },
    { 0x00FFFF, 0x00E0DF, 2,FF,  RAX,NNN,NNN, C_FLT + 0,          I_FSTSW },
    { 0x00FFFF, 0x00E5D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FXAM },
    { 0x00FFFF, 0x00F4D9, 2,00,  PS0,NNN,NNN, C_FLT + 0,          I_FXTRACT },
    { 0x00FFFF, 0x00D9DE, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FCOMPP },
    { 0x00FFFF, 0x00E9DA, 2,00,  PS0,PS1,NNN, C_FLT + 0,          I_FUCOMPP },
    { 0x00F8FF, 0x00C0DD, 2,00,  RST,NNN,NNN, C_FLT + 0,          I_FFREE },
    { 0x00F8FF, 0x00C0DA, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVB },
    { 0x00F8FF, 0x00C8DA, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVE },
    { 0x00F8FF, 0x00D0DA, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVBE },
    { 0x00F8FF, 0x00D8DA, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVU },
    { 0x00F8FF, 0x00C0DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVNB },
    { 0x00F8FF, 0x00C8DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVNE },
    { 0x00F8FF, 0x00D0DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVNBE },
    { 0x00F8FF, 0x00D8DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCMOVNU },
    { 0x00F8FF, 0x00F0DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCOMI },
    { 0x00F8FF, 0x00F0DF, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FCOMIP },
    { 0x00F8FF, 0x00E8DB, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FUCOMI },
    { 0x00F8FF, 0x00E8DF, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FUCOMIP },
    { 0x00F8FF, 0x00C0D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FADD },
    { 0x00F8FF, 0x00C0DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FADD },
    { 0x00F8FF, 0x00C0DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FADDP },
    { 0x00F8FF, 0x00E0D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FSUB },
    { 0x00F8FF, 0x00E8DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FSUB },
    { 0x00F8FF, 0x00E8DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FSUBP },
    { 0x00F8FF, 0x00E8D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FSUBR },
    { 0x00F8FF, 0x00E0DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FSUBR },
    { 0x00F8FF, 0x00E0DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FSUBRP },
    { 0x00F8FF, 0x00C8D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FMUL },
    { 0x00F8FF, 0x00C8DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FMUL },
    { 0x00F8FF, 0x00C8DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FMULP },
    { 0x00F8FF, 0x00D0D8, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FCOM },
    { 0x00F8FF, 0x00D8D8, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FCOMP },
    { 0x00F8FF, 0x00E0DD, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FUCOM },
    { 0x00F8FF, 0x00E8DD, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FUCOMP },
    { 0x00F8FF, 0x00F0D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FDIV },
    { 0x00F8FF, 0x00F8DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FDIV },
    { 0x00F8FF, 0x00F8DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FDIVP },
    { 0x00F8FF, 0x00F8D8, 2,00,  RS0,RST,NNN, C_FLT + 0,          I_FDIVR },
    { 0x00F8FF, 0x00F0DC, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FDIVR },
    { 0x00F8FF, 0x00F0DE, 2,00,  RST,RS0,NNN, C_FLT + 0,          I_FDIVRP },
    { 0x00F8FF, 0x00C0D9, 2,00,  RST,NNN,NNN, C_FLT + 0,          I_FLD },
    { 0x00F8FF, 0x00D0DD, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FST },
    { 0x00F8FF, 0x00D8DD, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FSTP },
    { 0x00F8FF, 0x00C8D9, 2,00,  RST,PS0,NNN, C_FLT + 0,          I_FXCH },
    { 0x0038FF, 0x0000D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FADD },
    { 0x0038FF, 0x0000DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FADD },
    { 0x0038FF, 0x0000DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FIADD },
    { 0x0038FF, 0x0000DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FIADD },
    { 0x0038FF, 0x0020D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FSUB },
    { 0x0038FF, 0x0020DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FSUB },
    { 0x0038FF, 0x0020DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FISUB },
    { 0x0038FF, 0x0020DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FISUB },
    { 0x0038FF, 0x0028D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FSUBR },
    { 0x0038FF, 0x0028DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FSUBR },
    { 0x0038FF, 0x0028DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FISUBR },
    { 0x0038FF, 0x0028DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FISUBR },
    { 0x0038FF, 0x0008D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FMUL },
    { 0x0038FF, 0x0008DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FMUL },
    { 0x0038FF, 0x0008DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FIMUL },
    { 0x0038FF, 0x0008DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FIMUL },
    { 0x0038FF, 0x0010D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FCOM },
    { 0x0038FF, 0x0010DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FCOM },
    { 0x0038FF, 0x0018D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FCOMP },
    { 0x0038FF, 0x0018DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FCOMP },
    { 0x0038FF, 0x0030D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FDIV },
    { 0x0038FF, 0x0030DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FDIV },
    { 0x0038FF, 0x0030DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FIDIV },
    { 0x0038FF, 0x0030DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FIDIV },
    { 0x0038FF, 0x0038D8, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FDIVR },
    { 0x0038FF, 0x0038DC, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FDIVR },
    { 0x0038FF, 0x0038DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FIDIVR },
    { 0x0038FF, 0x0038DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FIDIVR },
    { 0x0038FF, 0x0020DF, 1,00,  MDA,NNN,NNN, C_FLT + C_RARE + 1, I_FBLD },
    { 0x0038FF, 0x0030DF, 1,00,  MDA,PS0,NNN, C_FLT + C_RARE + 1, I_FBSTP },
    { 0x0038FF, 0x0010DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FICOM },
    { 0x0038FF, 0x0010DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FICOM },
    { 0x0038FF, 0x0018DE, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FICOMP },
    { 0x0038FF, 0x0018DA, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FICOMP },
    { 0x0038FF, 0x0000DF, 1,00,  MD2,NNN,NNN, C_FLT + 1,          I_FILD },
    { 0x0038FF, 0x0000DB, 1,00,  MD4,NNN,NNN, C_FLT + 1,          I_FILD },
    { 0x0038FF, 0x0028DF, 1,00,  MD8,NNN,NNN, C_FLT + 1,          I_FILD },
    { 0x0038FF, 0x0010DF, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FIST },
    { 0x0038FF, 0x0010DB, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FIST },
    { 0x0038FF, 0x0018DF, 1,00,  MD2,PS0,NNN, C_FLT + 1,          I_FISTP },
    { 0x0038FF, 0x0018DB, 1,00,  MD4,PS0,NNN, C_FLT + 1,          I_FISTP },
    { 0x0038FF, 0x0038DF, 1,00,  MD8,PS0,NNN, C_FLT + 1,          I_FISTP },
    { 0x0038FF, 0x0000D9, 1,00,  MF4,NNN,NNN, C_FLT + 1,          I_FLD },
    { 0x0038FF, 0x0000DD, 1,00,  MF8,NNN,NNN, C_FLT + 1,          I_FLD },
    { 0x0038FF, 0x0028DB, 1,00,  MFA,NNN,NNN, C_FLT + 1,          I_FLD },
    { 0x0038FF, 0x0010D9, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FST },
    { 0x0038FF, 0x0010DD, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FST },
    { 0x0038FF, 0x0018D9, 1,00,  MF4,PS0,NNN, C_FLT + 1,          I_FSTP },
    { 0x0038FF, 0x0018DD, 1,00,  MF8,PS0,NNN, C_FLT + 1,          I_FSTP },
    { 0x0038FF, 0x0038DB, 1,00,  MFA,PS0,NNN, C_FLT + 1,          I_FSTP },
    { 0x0038FF, 0x0028D9, 1,00,  MB2,NNN,NNN, C_FLT + 0,          I_FLDCW },
    { 0x0038FF, 0x0038D9, 1,00,  MB2,NNN,NNN, C_FLT + 0,          I_FSTCW },
    { 0x0038FF, 0x0020D9, 1,00,  MFE,NNN,NNN, C_FLT + 0,          I_FLDENV },
    { 0x0038FF, 0x0030D9, 1,00,  MFE,NNN,NNN, C_FLT + 0,          I_FSTENV },
    { 0x0038FF, 0x0020DD, 1,00,  MFS,NNN,NNN, C_FLT + 0,          I_FRSTOR },
    { 0x0038FF, 0x0030DD, 1,00,  MFS,NNN,NNN, C_FLT + 0,          I_FSAVE },
    { 0x0038FF, 0x0038DD, 1,00,  MB2,NNN,NNN, C_FLT + 0,          I_FSTSW },
    { 0x38FFFF, 0x08AE0F, 2,00,  MFX,NNN,NNN, C_FLT + 0,          I_FXRSTOR },
    { 0x38FFFF, 0x00AE0F, 2,00,  MFX,NNN,NNN, C_FLT + 0,          I_FXSAVE },
    { 0x00FFFF, 0x00E0DB, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FENI },
    { 0x00FFFF, 0x00E1DB, 2,00,  NNN,NNN,NNN, C_FLT + 0,          I_FDISI },
    // MMX instructions. Length of MMX operand fields (in bytes) is added to the
    // type, length of 0 means 8-byte MMX operand.
    { 0x00FFFF, 0x00770F, 2,00,  NNN,NNN,NNN, C_MMX + 0,          I_EMMS },
    { 0x00FFFF, 0x006E0F, 2,00,  RMX,MR4,NNN, C_MMX + 0,          I_MOVD },
    { 0x00FFFF, 0x007E0F, 2,00,  MR4,RMX,NNN, C_MMX + 0,          I_MOVD },
    { 0x00FFFF, 0x006F0F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_MOVQ },
    { 0x00FFFF, 0x007F0F, 2,00,  MR8,RMX,NNN, C_MMX + 0,          I_MOVQ },
    { 0x00FFFF, 0x00630F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PACKSSWB },
    { 0x00FFFF, 0x006B0F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PACKSSDW },
    { 0x00FFFF, 0x00670F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PACKUSWB },
    { 0x00FFFF, 0x00FC0F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PADDB },
    { 0x00FFFF, 0x00FD0F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PADDW },
    { 0x00FFFF, 0x00FE0F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PADDD },
    { 0x00FFFF, 0x00F80F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PSUBB },
    { 0x00FFFF, 0x00F90F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSUBW },
    { 0x00FFFF, 0x00FA0F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PSUBD },
    { 0x00FFFF, 0x00EC0F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PADDSB },
    { 0x00FFFF, 0x00ED0F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PADDSW },
    { 0x00FFFF, 0x00E80F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PSUBSB },
    { 0x00FFFF, 0x00E90F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSUBSW },
    { 0x00FFFF, 0x00DC0F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PADDUSB },
    { 0x00FFFF, 0x00DD0F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PADDUSW },
    { 0x00FFFF, 0x00D80F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PSUBUSB },
    { 0x00FFFF, 0x00D90F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSUBUSW },
    { 0x00FFFF, 0x00DB0F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_PAND },
    { 0x00FFFF, 0x00DF0F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_PANDN },
    { 0x00FFFF, 0x00740F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PCMPEQB },
    { 0x00FFFF, 0x00750F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PCMPEQW },
    { 0x00FFFF, 0x00760F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PCMPEQD },
    { 0x00FFFF, 0x00640F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PCMPGTB },
    { 0x00FFFF, 0x00650F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PCMPGTW },
    { 0x00FFFF, 0x00660F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PCMPGTD },
    { 0x00FFFF, 0x00F50F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMADDWD },
    { 0x00FFFF, 0x00E50F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMULHW },
    { 0x00FFFF, 0x00D50F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMULLW },
    { 0x00FFFF, 0x00EB0F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_POR },
    { 0x00FFFF, 0x00F10F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSLLW },
    { 0x38FFFF, 0x30710F, 2,00,  MR8,IM1,NNN, C_MMX + 2,          I_PSLLW },
    { 0x00FFFF, 0x00F20F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PSLLD },
    { 0x38FFFF, 0x30720F, 2,00,  MR8,IM1,NNN, C_MMX + 4,          I_PSLLD },
    { 0x00FFFF, 0x00F30F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_PSLLQ },
    { 0x38FFFF, 0x30730F, 2,00,  MR8,IM1,NNN, C_MMX + 0,          I_PSLLQ },
    { 0x00FFFF, 0x00E10F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSRAW },
    { 0x38FFFF, 0x20710F, 2,00,  MR8,IM1,NNN, C_MMX + 2,          I_PSRAW },
    { 0x00FFFF, 0x00E20F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PSRAD },
    { 0x38FFFF, 0x20720F, 2,00,  MR8,IM1,NNN, C_MMX + 4,          I_PSRAD },
    { 0x00FFFF, 0x00D10F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PSRLW },
    { 0x38FFFF, 0x10710F, 2,00,  MR8,IM1,NNN, C_MMX + 2,          I_PSRLW },
    { 0x00FFFF, 0x00D20F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PSRLD },
    { 0x38FFFF, 0x10720F, 2,00,  MR8,IM1,NNN, C_MMX + 4,          I_PSRLD },
    { 0x00FFFF, 0x00D30F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_PSRLQ },
    { 0x38FFFF, 0x10730F, 2,00,  MR8,IM1,NNN, C_MMX + 0,          I_PSRLQ },
    { 0x00FFFF, 0x00680F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PUNPCKHBW },
    { 0x00FFFF, 0x00690F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PUNPCKHWD },
    { 0x00FFFF, 0x006A0F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PUNPCKHDQ },
    { 0x00FFFF, 0x00600F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PUNPCKLBW },
    { 0x00FFFF, 0x00610F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PUNPCKLWD },
    { 0x00FFFF, 0x00620F, 2,00,  RMX,MR8,NNN, C_MMX + 4,          I_PUNPCKLDQ },
    { 0x00FFFF, 0x00EF0F, 2,00,  RMX,MR8,NNN, C_MMX + 0,          I_PXOR },
    // AMD extentions to MMX command set (including Athlon/PIII extentions).
    { 0x00FFFF, 0x000E0F, 2,00,  NNN,NNN,NNN, C_MMX + 0,          I_FEMMS },
    { 0x38FFFF, 0x000D0F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCH },
    { 0x38FFFF, 0x080D0F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCHW },
    { 0x00FFFF, 0x00F70F, 2,00,  RMX,RR8,PDI, C_MMX + 1,          I_MASKMOVQ },
    { 0x00FFFF, 0x00E70F, 2,00,  MD8,RMX,NNN, C_MMX + 0,          I_MOVNTQ },
    { 0x00FFFF, 0x00E00F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PAVGB },
    { 0x00FFFF, 0x00E30F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PAVGW },
    { 0x00FFFF, 0x00C50F, 2,00,  RR4,RMX,IM1, C_MMX + 2,          I_PEXTRW },
    { 0x00FFFF, 0x00C40F, 2,00,  RMX,MR2,IM1, C_MMX + 2,          I_PINSRW },
    { 0x00FFFF, 0x00EE0F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMAXSW },
    { 0x00FFFF, 0x00DE0F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PMAXUB },
    { 0x00FFFF, 0x00EA0F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMINSW },
    { 0x00FFFF, 0x00DA0F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PMINUB },
    { 0x00FFFF, 0x00D70F, 2,00,  RG4,RR8,NNN, C_MMX + 1,          I_PMOVMSKB },
    { 0x00FFFF, 0x00E40F, 2,00,  RMX,MR8,NNN, C_MMX + 2,          I_PMULHUW },
    { 0x38FFFF, 0x00180F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCHNTA },
    { 0x38FFFF, 0x08180F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCHT0 },
    { 0x38FFFF, 0x10180F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCHT1 },
    { 0x38FFFF, 0x18180F, 2,00,  MD8,NNN,NNN, C_MMX + 0,          I_PREFETCHT2 },
    { 0x00FFFF, 0x00F60F, 2,00,  RMX,MR8,NNN, C_MMX + 1,          I_PSADBW },
    { 0x00FFFF, 0x00700F, 2,00,  RMX,MR8,IM1, C_MMX + 2,          I_PSHUFW },
    { 0xFFFFFF, 0xF8AE0F, 2,00,  NNN,NNN,NNN, C_MMX + 0,          I_SFENCE },
    // AMD 3DNow! instructions (including Athlon extentions).
    { 0x00FFFF, 0xBF0F0F, 2,00,  RMX,MR8,NNN, C_NOW + 1,          I_PAVGUSB },
    { 0x00FFFF, 0x9E0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFADD },
    { 0x00FFFF, 0x9A0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFSUB },
    { 0x00FFFF, 0xAA0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFSUBR },
    { 0x00FFFF, 0xAE0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFACC },
    { 0x00FFFF, 0x900F0F, 2,00,  RMX,MRD,NNN, C_NOW + 4,          I_PFCMPGE },
    { 0x00FFFF, 0xA00F0F, 2,00,  RMX,MRD,NNN, C_NOW + 4,          I_PFCMPGT },
    { 0x00FFFF, 0xB00F0F, 2,00,  RMX,MRD,NNN, C_NOW + 4,          I_PFCMPEQ },
    { 0x00FFFF, 0x940F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFMIN },
    { 0x00FFFF, 0xA40F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFMAX },
    { 0x00FFFF, 0x0D0F0F, 2,00,  R3D,MR8,NNN, C_NOW + 4,          I_PI2FD },
    { 0x00FFFF, 0x1D0F0F, 2,00,  RMX,MRD,NNN, C_NOW + 4,          I_PF2ID },
    { 0x00FFFF, 0x960F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFRCP },
    { 0x00FFFF, 0x970F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFRSQRT },
    { 0x00FFFF, 0xB40F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFMUL },
    { 0x00FFFF, 0xA60F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFRCPIT1 },
    { 0x00FFFF, 0xA70F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFRSQIT1 },
    { 0x00FFFF, 0xB60F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFRCPIT2 },
    { 0x00FFFF, 0xB70F0F, 2,00,  RMX,MR8,NNN, C_NOW + 2,          I_PMULHRW },
    { 0x00FFFF, 0x1C0F0F, 2,00,  RMX,MRD,NNN, C_NOW + 4,          I_PF2IW },
    { 0x00FFFF, 0x8A0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFNACC },
    { 0x00FFFF, 0x8E0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PFPNACC },
    { 0x00FFFF, 0x0C0F0F, 2,00,  R3D,MR8,NNN, C_NOW + 4,          I_PI2FW },
    { 0x00FFFF, 0xBB0F0F, 2,00,  R3D,MRD,NNN, C_NOW + 4,          I_PSWAPD },
    // SSE instructions
    { 0xFFFFFF, 0x100FF3, 2,00,  RXM,MX4,NNN, C_SSE + 2,          I_MOVSS },
    { 0xFFFFFF, 0x110FF3, 2,00,  MX4,RXM,NNN, C_SSE + 2,          I_MOVSS },
    { 0xFFFFFF, 0x2A0FF3, 2,00,  RXM,MR4,NNN, C_SSE + 2,          I_CVTSI2SS },
    { 0xFFFFFF, 0xEF0F66, 2,00,  RXM,MR0,NNN, C_SSE + 0,          I_PXOR },    
    // End of command table.
    { 0x000000, 0x000000, 0,00,  NNN,NNN,NNN, C_CMD + 0,          I_NONE }
};

const t_cmddata vxdcmd =               // Decoding of VxD calls (Win95/98)
    { 0x00FFFF, 0x0020CD, 2,00,  VXD,NNN,NNN, C_CAL + C_RARE + 0, I_VXDCALL };

// Bit combinations that can be potentially dangerous when executed:
const t_cmddata dangerous[] = {
  { 0x00FFFF, 0x00DCF7, 0,0,0,0,0,C_DANGER95, I_NONE },
  { 0x00FFFF, 0x00D4F7, 0,0,0,0,0,C_DANGER95, I_NONE },
  { 0x00FFFF, 0x0020CD, 0,0,0,0,0,C_DANGER95, I_NONE },
  { 0xF8FFFF, 0xC8C70F, 0,0,0,0,1,C_DANGERLOCK, I_NONE },
  { 0x000000, 0x000000, 0,0,0,0,0,0, I_NONE }
};

// Function attempts to calculate address of assembler instruction which is n
// lines back in the listing. Maximal stepback is limited to 127. In general,
// this is rather non-trivial task. Proposed solution may cause problems which
// however are not critical here.
ulong Disassembleback(uchar *block, ulong base, ulong size, ulong ip, int n)
{
    int i;
    ulong abuf[131], addr, back, cmdsize;
    uchar *pdata;
    t_disasm da;
    if (block == NULL) return 0;           // Error, no code!
    if (n < 0) n = 0; else if (n>127) n = 127; // Try to correct obvious errors
    if (ip > base + size) ip = base + size;
    if (n == 0) return ip;                 // Obvious answers
    if (ip <= base + n) return base;
    back = MAXCMDSIZE*(n + 3);               // Command length limited to MAXCMDSIZE
    if (ip < base + back) back = ip - base;
    addr = ip - back;
    pdata = block + (addr - base);
    for (i = 0; addr < ip; i++) {
        abuf[i % 128] = addr;
        cmdsize = Disasm(pdata, back, addr, &da, DISASM_SIZE);
        pdata += cmdsize;
        addr += cmdsize;
        back -= cmdsize;
    };
    if (i < n) return abuf[0];
    else return abuf[(i - n + 128) % 128];
};

// Function attempts to calculate address of assembler instruction which is n
// lines forward in the listing.
ulong Disassembleforward(uchar *block, ulong base, ulong size, ulong ip, int n)
{
    int i;
    ulong cmdsize;
    uchar *pdata;
    t_disasm da;
    if (block == NULL) return 0;           // Error, no code!
    if (ip < base) ip = base;                // Try to correct obvious errors
    if (ip > base + size) ip = base + size;
    if (n <= 0) return ip;
    pdata = block + (ip - base);
    size -= (ip - base);
    for (i = 0; i < n && size>0; i++) {
        cmdsize = Disasm(pdata, size, ip, &da, DISASM_SIZE);
        pdata += cmdsize;
        ip += cmdsize;
        size -= cmdsize;
    };
    return ip;
};

// Service function, checks whether command at offset addr in data is a valid
// filling command (usually some kind of NOP) used to align code to a specified
// (align=power of 2, 0 means no alignment) border. Returns length of filling
// command in bytes or 0 if command is not a recognized filling.
int Isfilling(ulong addr, uchar *data, ulong size, ulong align)
{
    if (data == NULL) return 0;            // Invalid parameters
    // Convert power of 2 to bitmask.
    align--;
    // Many compilers and assemblers use NOP or INT3 as filling:
    if (addr < size && (data[addr] == NOP || data[addr] == INT3) &&
        (addr & align) != 0)
        return 1;
    // Borland compilers use XCHG EBX,EBX (87,DB) to fill holes. For the sake of
    // completeness, allow any XCHG or MOV of register with self.
    if (addr + 1 < size &&
        ((data[addr] & 0xFE) == 0x86 || (data[addr] & 0xFC) == 0x88) &&
        (data[addr + 1] & 0xC0) == 0xC0 &&
        (((data[addr + 1] >> 3) ^ data[addr + 1]) & 0x07) == 0 &&
        (addr & align) != 0x0F && (addr & align) != 0x00)
        return 2;
    // Some other programs use LEA EAX,[EAX] (8D,40,00). For completeness, allow
    // any register except ESP (here address is constructed differently):
    if (addr + 2 < size &&
        data[addr] == 0x8D && (data[addr + 1] & 0xC0) == 0x40 && data[addr + 2] == 0x00 &&
        (data[addr + 1] & 0x07) != REG_ESP &&
        (((data[addr + 1] >> 3) ^ data[addr + 1]) & 0x07) == 0)
        return 3;
    // WATCOM compilers use LEA EAX,[EAX] with SIB and 8-bit zero (8D,44,20,00)
    // and without SIB but with 32-bit immediate zero (8D,80,00,00,00,00) and
    // alike:
    if (addr + 3 < size &&
        data[addr] == 0x8D && (data[addr + 1] & 0xC0) == 0x40 && data[addr + 3] == 0x00 &&
        (((data[addr + 1] >> 3) ^ data[addr + 2]) & 0x07) == 0)
        return 4;
    if (addr + 5 < size && data[addr] == 0x8D &&
        (data[addr + 1] & 0xC0) == 0x80 && *(ulong *)(data + addr + 2) == 0 &&
        (data[addr + 1] & 0x07) != REG_ESP &&
        (((data[addr + 1] >> 3) ^ data[addr + 1]) & 0x07) == 0)
        return 6;
    // Unable to recognize this code as a valid filling.
    return 0;
};


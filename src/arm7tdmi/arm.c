// NOTE: this entire code is wrong. i was writing it whilst reading the arm
// data sheet, but should've finished it before starting the arm
// decoding.
// the barrel shifting is more complicated than a simple shift as it
// can update the carry(?) flag as well as having the shift type
// encoded, so it could be a shl, shr, rol, ror...

#include "arm7tdmi.h"

#include <assert.h>

// branch, copies reg[Rn] into PC
// causes a pipeline flush (wat)
// bit-0 of Rn determines if instruction stream is decoded as arm or thumb (wat)
//  - 1: thumb
//  - 0: arm
// can't use reg15 (PC) as oprand???
void arm7tdmi_instr_bx(arm7tdmi_t* arm, uint32_t op) {
    const uint8_t rn = op & 0xF;
    assert(rn != 15 && "using reg15 is UB!");

}

void arm7tdmi_instr_and(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_eor(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_sub(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_rsb(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_add(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_adc(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_sbc(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_rsc(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_tst(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_teq(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_cmp(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_cmn(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_orr(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_mov(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_bic(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

void arm7tdmi_instr_mvn(arm7tdmi_t* arm, uint32_t op1, uint32_t op2, uint8_t dst, uint8_t s) {

}

// NOTE:
// looking at the data processing instructions, im not sure if it would be
// best to have 2 diff functions that do the same thing but
// fetch the oprand 2 differently, ie, imm or Rm
// or can pass the oprands in and just split the imm, Rm by
// switch cases.
// however i should also pass the S bit for if i should alter
// the condition codes (cpsr)

// actually, i might make 4 functions, 1 for each *actual* instruction.
// - mov (normal)
// - mov_s (alter cond codes)
// - mov_imm (immediate val)
// - mov_imm_s (immediate val, cpsr)

// or maybe 2 functions and pass operand params:
// - mov (normal / imm)
// - mov_s (normal / imm, alter cpsr)

// in both approaches, theres one less branch (if s == 1)

// NOTE: the following shifts / rotate are wrong (i should've read ahead before typing)
// the shifts are lower 2-bit encoded for the type of shift performed
// the shift amount is the upper 5-bits
#define DATA_PROCESS_Rn(opcode) ((opcode >> 16) & 0xF)
#define DATA_PROCESS_Rd(opcode) ((opcode >> 12) & 0xF)

#define DATA_PROCESS_Shift(opcode) ((opcode >> 4) & 0xFF)
#define DATA_PROCESS_Rm(opcode) (opcode & 0xF)

#define DATA_PROCESS_Rotate(opcode) ((opcode >> 8) & 0xF)
#define DATA_PROCESS_Imm(opcode) (opcode & 0xFF)

#define DATA_PROCESS_OP1(opcode) arm->registers[DATA_PROCESS_Rn(opcode)]
#define DATA_PROCESS_OP2(opcode) arm->registers[DATA_PROCESS_Rm(opcode)] << DATA_PROCESS_Shift(opcode)
// TODO: actually rotate (not just shift)
#define DATA_PROCESS_OP2_IMM(opcode) DATA_PROCESS_Imm(opcode) << DATA_PROCESS_Rotate(opcode)
#define DATA_PROCESS_DST(opcode) DATA_PROCESS_Rd(opcode)

#define DATA_PROCESS_S(opcode) ((opcode & 0x10000) > 0)
#define REGISTERS arm->registers

void decode_test(arm7tdmi_t* arm, uint32_t op) {
    // i think only bits 27-4 matter
    // bits 31-28 are cond
    // bits 3-0 are usually Rn / Rm

    // the '' between numbers are just to make it more clear
    // for me. shame its not valid C.

    // 0b00'0'0000'0'1111111111111111
    // 0b00'1'0000'0'0000000000000000

    //   00'I'xxxx'S'xxxxxxxxxxxxxxxx
    // 0b00'0'0000'0'0000000000000000
    switch ((op >> 4) & 0xFFFFFF) {
        // and
        case 0b00'0'0000'0'0000000000000000 ... 0b00'0'0000'0'1111111111111111:
        case 0b00'0'0000'1'0000000000000000 ... 0b00'0'0000'1'1111111111111111:
            arm7tdmi_instr_and(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // and imm 
        case 0b00'1'0000'0'0000000000000000 ... 0b00'1'0000'0'1111111111111111:
        case 0b00'1'0000'1'0000000000000000 ... 0b00'1'0000'1'1111111111111111:
            arm7tdmi_instr_and(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // eor
        case 0b00'0'0001'0'0000000000000000 ... 0b00'0'0001'0'1111111111111111:
        case 0b00'0'0001'1'0000000000000000 ... 0b00'0'0001'1'1111111111111111:
            arm7tdmi_instr_eor(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // eor imm 
        case 0b00'1'0001'0'0000000000000000 ... 0b00'1'0001'0'1111111111111111:
        case 0b00'1'0001'1'0000000000000000 ... 0b00'1'0001'1'1111111111111111:
            arm7tdmi_instr_eor(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // sub
        case 0b00'0'0010'0'0000000000000000 ... 0b00'0'0010'0'1111111111111111:
        case 0b00'0'0010'1'0000000000000000 ... 0b00'0'0010'1'1111111111111111:
            arm7tdmi_instr_sub(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // sub imm 
        case 0b00'1'0010'0'0000000000000000 ... 0b00'1'0010'0'1111111111111111:
        case 0b00'1'0010'1'0000000000000000 ... 0b00'1'0010'1'1111111111111111:
            arm7tdmi_instr_sub(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // rsb
        case 0b00'0'0011'0'0000000000000000 ... 0b00'0'0011'0'1111111111111111:
        case 0b00'0'0011'1'0000000000000000 ... 0b00'0'0011'1'1111111111111111:
            arm7tdmi_instr_rsb(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // rsb imm 
        case 0b00'1'0011'0'0000000000000000 ... 0b00'1'0011'0'1111111111111111:
        case 0b00'1'0011'1'0000000000000000 ... 0b00'1'0011'1'1111111111111111:
            arm7tdmi_instr_rsb(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // add
        case 0b00'0'0100'0'0000000000000000 ... 0b00'0'0100'0'1111111111111111:
        case 0b00'0'0100'1'0000000000000000 ... 0b00'0'0100'1'1111111111111111:
            arm7tdmi_instr_add(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // add imm 
        case 0b00'1'0100'0'0000000000000000 ... 0b00'1'0100'0'1111111111111111:
        case 0b00'1'0100'1'0000000000000000 ... 0b00'1'0100'1'1111111111111111:
            arm7tdmi_instr_add(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // adc
        case 0b00'0'0101'0'0000000000000000 ... 0b00'0'0101'0'1111111111111111:
        case 0b00'0'0101'1'0000000000000000 ... 0b00'0'0101'1'1111111111111111:
            arm7tdmi_instr_adc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // adc imm 
        case 0b00'1'0101'0'0000000000000000 ... 0b00'1'0101'0'1111111111111111:
        case 0b00'1'0101'1'0000000000000000 ... 0b00'1'0101'1'1111111111111111:
            arm7tdmi_instr_adc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // sbc
        case 0b00'0'0110'0'0000000000000000 ... 0b00'0'0110'0'1111111111111111:
        case 0b00'0'0110'1'0000000000000000 ... 0b00'0'0110'1'1111111111111111:
            arm7tdmi_instr_sbc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // sbc imm 
        case 0b00'1'0110'0'0000000000000000 ... 0b00'1'0110'0'1111111111111111:
        case 0b00'1'0110'1'0000000000000000 ... 0b00'1'0110'1'1111111111111111:
            arm7tdmi_instr_sbc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // rsc
        case 0b00'0'0111'0'0000000000000000 ... 0b00'0'0111'0'1111111111111111:
        case 0b00'0'0111'1'0000000000000000 ... 0b00'0'0111'1'1111111111111111:
            arm7tdmi_instr_rsc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // rsc imm 
        case 0b00'1'0111'0'0000000000000000 ... 0b00'1'0111'0'1111111111111111:
        case 0b00'1'0111'1'0000000000000000 ... 0b00'1'0111'1'1111111111111111:
            arm7tdmi_instr_rsc(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // tst
        case 0b00'0'1000'0'0000000000000000 ... 0b00'0'1000'0'1111111111111111:
        case 0b00'0'1000'1'0000000000000000 ... 0b00'0'1000'1'1111111111111111:
            arm7tdmi_instr_tst(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // tst imm 
        case 0b00'1'1000'0'0000000000000000 ... 0b00'1'1000'0'1111111111111111:
        case 0b00'1'1000'1'0000000000000000 ... 0b00'1'1000'1'1111111111111111:
            arm7tdmi_instr_tst(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // teq
        case 0b00'0'1001'0'0000000000000000 ... 0b00'0'1001'0'1111111111111111:
        case 0b00'0'1001'1'0000000000000000 ... 0b00'0'1001'1'1111111111111111:
            arm7tdmi_instr_teq(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // teq imm 
        case 0b00'1'1001'0'0000000000000000 ... 0b00'1'1001'0'1111111111111111:
        case 0b00'1'1001'1'0000000000000000 ... 0b00'1'1001'1'1111111111111111:
            arm7tdmi_instr_teq(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // cmp
        case 0b00'0'1010'0'0000000000000000 ... 0b00'0'1010'0'1111111111111111:
        case 0b00'0'1010'1'0000000000000000 ... 0b00'0'1010'1'1111111111111111:
            arm7tdmi_instr_cmp(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // cmp imm 
        case 0b00'1'1010'0'0000000000000000 ... 0b00'1'1010'0'1111111111111111:
        case 0b00'1'1010'1'0000000000000000 ... 0b00'1'1010'1'1111111111111111:
            arm7tdmi_instr_cmp(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // cmn
        case 0b00'0'1011'0'0000000000000000 ... 0b00'0'1011'0'1111111111111111:
        case 0b00'0'1011'1'0000000000000000 ... 0b00'0'1011'1'1111111111111111:
            arm7tdmi_instr_cmn(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // cmn imm 
        case 0b00'1'1011'0'0000000000000000 ... 0b00'1'1011'0'1111111111111111:
        case 0b00'1'1011'1'0000000000000000 ... 0b00'1'1011'1'1111111111111111:
            arm7tdmi_instr_cmn(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // orr
        case 0b00'0'1100'0'0000000000000000 ... 0b00'0'1100'0'1111111111111111:
        case 0b00'0'1100'1'0000000000000000 ... 0b00'0'1100'1'1111111111111111:
            arm7tdmi_instr_orr(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // orr imm 
        case 0b00'1'1100'0'0000000000000000 ... 0b00'1'1100'0'1111111111111111:
        case 0b00'1'1100'1'0000000000000000 ... 0b00'1'1100'1'1111111111111111:
            arm7tdmi_instr_orr(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // mov
        case 0b00'0'1101'0'0000000000000000 ... 0b00'0'1101'0'1111111111111111:
        case 0b00'0'1101'1'0000000000000000 ... 0b00'0'1101'1'1111111111111111:
            arm7tdmi_instr_mov(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // mov imm 
        case 0b00'1'1101'0'0000000000000000 ... 0b00'1'1101'0'1111111111111111:
        case 0b00'1'1101'1'0000000000000000 ... 0b00'1'1101'1'1111111111111111:
            arm7tdmi_instr_mov(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // bic
        case 0b00'0'1110'0'0000000000000000 ... 0b00'0'1110'0'1111111111111111:
        case 0b00'0'1110'1'0000000000000000 ... 0b00'0'1110'1'1111111111111111:
            arm7tdmi_instr_bic(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // bic imm 
        case 0b00'1'1110'0'0000000000000000 ... 0b00'1'1110'0'1111111111111111:
        case 0b00'1'1110'1'0000000000000000 ... 0b00'1'1110'1'1111111111111111:
            arm7tdmi_instr_bic(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;

        // mvn
        case 0b00'0'1111'0'0000000000000000 ... 0b00'0'1111'0'1111111111111111:
        case 0b00'0'1111'1'0000000000000000 ... 0b00'0'1111'1'1111111111111111:
            arm7tdmi_instr_mvn(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
        // mvn imm 
        case 0b00'1'1111'0'0000000000000000 ... 0b00'1'1111'0'1111111111111111:
        case 0b00'1'1111'1'0000000000000000 ... 0b00'1'1111'1'1111111111111111:
            arm7tdmi_instr_mvn(arm, DATA_PROCESS_OP1(op), DATA_PROCESS_OP2_IMM(op), DATA_PROCESS_DST(op), DATA_PROCESS_S(op));
            break;
    }


    // was going to turn the table from 4.1.1 into a switch statement
    // however it would mean multiple switches because i'd have to still
    // decode which actual instruction, which is silly.
    // leaving this here just in case.

    // 0b000000000000000000000000
    // switch ((op >> 4) & 0xFFFFFF) {
    //     // multiply
    //     case 0b000000000000000000001001 ... 0b000000111111111111111001:
    //         break;

    //     // multiply long
    //     case 0b000010000000000000001001 ... 0b000011111111111111111001:
    //         break;

    //     // single data swap
    //                                      // 0b000100101111111111110001
    //     case 0b000100000000000000001001 ... 0b000101001111111100001001:
    //         break;

    //     // branch and exchange
    //     case 0b000100101111111111110001:
    //         break;

    //     // coprocessor data transfer
    //     case 0b111000000000000000000000 ... 0b111011111111111111111110:
    //         break;

    //     // coprocessor register transfer
    //     case 0b111000000000000000000001 ... 0b111011111111111111111111:
    //         break;

    //     // swi (software interrupt)
    //     case 0b111100000000000000000000 ... 0b111111111111111111111111:
    //         break;
    // }

}

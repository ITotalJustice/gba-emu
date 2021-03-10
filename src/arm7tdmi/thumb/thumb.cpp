#include "../arm7tdmi.hpp"
#include "../../bit.hpp"

#include <stdio.h>
#include <assert.h>
#include <bit>

namespace arm7tdmi {

// this masks the lower 28-bits and then sets the cond flags
// which are the upper 4-bits (31-28).
// flag values are evaluated once so they can be part of an expression
// rather than a static value.
// however, they should not be part of a function or expression that
// causes side effects, such as ++ or --.
#define SET_COND_FLAGS_NZCV(n, z, c, v) \
    arm.registers[16] = (arm.registers[16] & 0b1111'1111'1111'1111'1111'1111'1111'1111) | \
        ((n) << 31) | ((z) << 30) | ((c) << 29) | ((v) << 28)

#define SET_COND_FLAGS_NZC(n, z, c) \
    arm.registers[16] = (arm.registers[16] & 0b0001'1111'1111'1111'1111'1111'1111'1111) | \
        ((n) << 31) | ((z) << 30) | ((c) << 29)

#define SET_COND_FLAGS_NZ(n, z) \
    arm.registers[16] = (arm.registers[16] & 0b0011'1111'1111'1111'1111'1111'1111'1111) | \
        ((n) << 31) | ((z) << 30)

#define SET_COND_FLAGS_N(n) \
    arm.registers[16] = (arm.registers[16] & 0b1111'1111'1111'1111'1111'1111'1111'1111) | \
        ((n) << 31)

#define GET_BIT(v, bit) ((v & (1 << (bit))) > 0)
#define SET_BIT(v, bit) v |= (1 << (bit))

// basically checks if the sign bit of a and b match but differ from r
// if bit 31 of a, b = 1 and r = 0, then overflow happened
// if bit 31 of a, b = 1 and r = 1, then overflow did not happened
#define FLAG_V_HELPER(a, b, r) ((a & 0x80000000) == (b & 0x80000000)) && ((a & 0x80000000) != (r & 0x80000000))

#define GET_CARRY() GET_BIT(arm.registers[16], 29)

#define scast(type) static_cast<type>
#define rcast(type) reinterpret_cast<type>
#define dcast(type) dynamic_cast<type>

/*
__builtin_uadd_overflow
__builtin_umul_overflow
__builtin_smul_overflow
*/

// 0b1111'1111'1111'1111'1111'1111'1111'1111;
// 0b1111'1111'1111'1111'1111'1111'1111;

// TODO: the shifts and rotates are going to be slow with branching
// to calc carry.
// this can be optimised later on either by setting all flags in each
// branch or by trying to not branch at all.

// template<bool update_flag = 1>
// u32 arm7tdmi_ror(arm7tdmi& arm, const u32 val, const u32 shift) {
//     return bit::rotr(val, shift);
// }

void thumb_and(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] & arm.registers[rs];
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0
    );
    arm.registers[rd] = result;
}

void thumb_eor(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] ^ arm.registers[rs];
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0
    );
    arm.registers[rd] = result;
}

void thumb_lsl(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] << (arm.registers[rs] & 0xFF);

    const bool carry = [&]() -> bool {
        switch (arm.registers[rs] & 0xFF) {
            case 0:
                return GET_CARRY();
            case 1 ... 31:
                return (arm.registers[rd] & (32 - arm.registers[rs])) > 0;
            case 32:
                return GET_BIT(arm.registers[rd], 0);
            default:
                return 0;
        }
    }();

    SET_COND_FLAGS_NZC(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ carry
    );
    arm.registers[rd] = result;
}

void thumb_lsr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] >> rs;

    const bool carry = [&]() -> bool {
        switch (arm.registers[rs] & 0xFF) {
            case 0:
                return GET_CARRY();
            case 1 ... 31:
                return (arm.registers[rd] & arm.registers[rs]) > 0;
            case 32:
                return GET_BIT(arm.registers[rd], 31);
            default:
                return 0;
        }
    }();

    SET_COND_FLAGS_NZC(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ carry
    );
    arm.registers[rd] = result;
}

void thumb_asr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    switch (arm.registers[rs]) {
        case 0:
        case 1 ... 31:
        default:
            break;
    };

    // i think if i cast reg[rd] to signed, this will be an asr
    const u32 result = static_cast<u32>(static_cast<int32_t>(arm.registers[rd]) >> arm.registers[rs]);
    // const u32 result = (arm.registers[rd] >> arm.registers[rs]) | (arm.registers[rd] & 0x80000000);
    
    const bool carry = [&]() -> bool {
        switch (arm.registers[rs] & 0xFF) {
            case 0:
                return GET_CARRY();
            case 1 ... 31:
                return (arm.registers[rd] & arm.registers[rs]) > 0;
            default:
                return GET_BIT(arm.registers[rd], 31);
        }
    }();

    SET_COND_FLAGS_NZC(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ carry
    );
    arm.registers[rd] = result;    
}

void thumb_adc(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] + arm.registers[rs] + GET_CARRY();
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ (arm.registers[rd] + arm.registers[rs] + GET_CARRY()) > 0xFF'FF'FF'FFU,
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
    arm.registers[rd] = result;
}

void thumb_sbc(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] - (arm.registers[rs] + GET_CARRY());
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ arm.registers[rd] < (arm.registers[rs] + GET_CARRY()),
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
    arm.registers[rd] = result;
}

void thumb_ror(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = bit::rotr(arm.registers[rd], arm.registers[rs]);
    
    const bool carry = [&]() -> bool {
        switch (arm.registers[rs] & 0xFF) {
            case 0:
                return GET_CARRY();
            case 1 ... 31:
                return (arm.registers[rd] & arm.registers[rs]) > 0;
            case 32:
                return GET_BIT(arm.registers[rd], 31);
            default:
                return 0;
        }
    }();

    SET_COND_FLAGS_NZC(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ carry
    );
    arm.registers[rd] = result;
}

void thumb_tst(arm7tdmi& arm, const u8 rs, const u8 rd) {    
    arm.registers[rd] = arm.registers[rd] & arm.registers[rs];
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(arm.registers[rd], 31),
        /* Z */ arm.registers[rd] == 0
    );
}

void thumb_neg(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = 0 - arm.registers[rs];
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ result > 0,
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
    arm.registers[rd] = result;
}

void thumb_cmp(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] - arm.registers[rs];
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ arm.registers[rd] < arm.registers[rs],
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
}

void thumb_cmn(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] + arm.registers[rs];
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ (arm.registers[rd] + arm.registers[rs]) > 0xFFFFFFFFU,
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
}

void thumb_orr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    arm.registers[rd] = arm.registers[rd] | arm.registers[rs];
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(arm.registers[rd], 31),
        /* Z */ arm.registers[rd] == 0
    );
}

void thumb_mul(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const u32 result = arm.registers[rd] * arm.registers[rs];
    SET_COND_FLAGS_NZCV(
        /* N */ GET_BIT(result, 31),
        /* Z */ result == 0,
        /* C */ (arm.registers[rd] * arm.registers[rs]) > 0xFFFFFFFFU,
        /* V */ FLAG_V_HELPER(arm.registers[rd], arm.registers[rs], result)
    );
    arm.registers[rd] = result;
}

// data sheet says rd AND NOT Rs
// not sure if it means making Rs either 1-0,
// or complement it with ~
void thumb_bic(arm7tdmi& arm, const u8 rs, const u8 rd) {
    arm.registers[rd] = arm.registers[rd] & (~arm.registers[rs]);
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(arm.registers[rd], 31),
        /* Z */ arm.registers[rd] == 0
    );
}

// same as above comments but no AND, just NOT
void thumb_mvn(arm7tdmi& arm, const u8 rs, const u8 rd) {
    arm.registers[rd] = ~arm.registers[rs];
    SET_COND_FLAGS_NZ(
        /* N */ GET_BIT(arm.registers[rd], 31),
        /* Z */ arm.registers[rd] == 0
    );
}

void thumb_decode_test(u16 op) {

    // 16-bit binary literal template
    // 0b0000'0000'0000'0000

    // only upper 10-bits matter (i think)
    // shifting and masking the value should be free
    // and i'd imagine it's more likely to make a jump table with less cases.

    // TODO: check if gcc / clang generate big jump table or
    // if else chain, hopefully jump table...

    switch ((op >> 6) & 0x3FF) {
    // [5.1]
        // LSL Rd, Rs, #Offset5
        case 0b000'00'00000 ... 0b000'00'11111:
            fputs("LSL Rd, Rs, #Offset5\n", stdout);
            break;

        // LSR Rd, Rs, #Offset5
        case 0b000'01'00000 ... 0b000'01'11111:
            fputs("LSR Rd, Rs, #Offset5\n", stdout);
            break;

        // ASR Rd, Rs, #Offset5
        case 0b000'10'00000 ... 0b000'10'11111:
            fputs("ASR Rd, Rs, #Offset5\n", stdout);
            break;
    
    // [5.2]
        // ADD Rd, Rs, Rn
        case 0b00011'0'0'000 ... 0b00011'0'0'111:
            fputs("ADD Rd, Rs, Rn\n", stdout);
            break;
        
        // ADD Rd, Rs, #Offset3
        case 0b00011'1'0'000 ... 0b00011'1'0'111:
            fputs("ADD Rd, Rs, #Offset3\n", stdout);
            break;

        // SUB Rd, Rs, Rn
        case 0b00011'0'1'000 ... 0b00011'0'1'111:
            fputs("SUB Rd, Rs, Rn\n", stdout);
            break;
        
        // SUB Rd, Rs, #Offset3
        case 0b00011'1'1'000 ... 0b00011'1'1'111:
            fputs("SUB Rd, Rs, #Offset3\n", stdout);
            break;

    // [5.3]
        // MOV Rd, #Offset8
        case 0b001'00'00000 ... 0b001'00'11111:
            fputs("MOV Rd, #Offset8\n", stdout);
            break;

        // CMP Rd, #Offset8
        case 0b001'01'00000 ... 0b001'01'11111:
            fputs("CMP Rd, #Offset8\n", stdout);
            break;

        // ADD Rd, #Offset8
        case 0b001'10'00000 ... 0b001'10'11111:
            fputs("ADD Rd, #Offset8\n", stdout);
            break;

        // SUB Rd, #Offset8
        case 0b001'11'00000 ... 0b001'11'11111:
            fputs("SUB Rd, #Offset8\n", stdout);
            break;
    
    // [5.4]
        // AND Rd, Rs
        case 0b010000'0000 ... 0b010000'0000:
            fputs("AND Rd, Rs\n", stdout);
            break;

        // EOR Rd, Rs
        case 0b010000'0001 ... 0b010000'0001:
            fputs("EOR Rd, Rs\n", stdout);
            break;

        // LSL Rd, Rs
        case 0b010000'0010 ... 0b010000'0010:
            fputs("LSL Rd, Rs\n", stdout);
            break;

        // LSR Rd, Rs
        case 0b010000'0011 ... 0b010000'0011:
            fputs("LSR Rd, Rs\n", stdout);
            break;
    
        // ASR Rd, Rs
        case 0b010000'0100 ... 0b010000'0100:
            fputs("ASR Rd, Rs\n", stdout);
            break;

        // ADC Rd, Rs
        case 0b010000'0101 ... 0b010000'0101:
            fputs("ADC Rd, Rs\n", stdout);
            break;

        // SBC Rd, Rs
        case 0b010000'0110 ... 0b010000'0110:
            fputs("SBC Rd, Rs\n", stdout);
            break;

        // ROR Rd, Rs
        case 0b010000'0111 ... 0b010000'0111:
            fputs("ROR Rd, Rs\n", stdout);
            break;

        // TST Rd, Rs
        case 0b010000'1000 ... 0b010000'1000:
            fputs("TST Rd, Rs\n", stdout);
            break;
        
        // NEG Rd, Rs
        case 0b010000'1001 ... 0b010000'1001:
            fputs("NEG Rd, Rs\n", stdout);
            break;

        // CMP Rd, Rs
        case 0b010000'1010 ... 0b010000'1010:
            fputs("CMP Rd, Rs\n", stdout);
            break;

        // CMN Rd, Rs
        case 0b010000'1011 ... 0b010000'1011:
            fputs("CMN Rd, Rs\n", stdout);
            break;

        // ORR Rd, Rs
        case 0b010000'1100 ... 0b010000'1100:
            fputs("ORR Rd, Rs\n", stdout);
            break;

        // MUL Rd, Rs
        case 0b010000'1101 ... 0b010000'1101:
            fputs("MUL Rd, Rs\n", stdout);
            break;

        // BIC Rd, Rs
        case 0b010000'1110 ... 0b010000'1110:
            fputs("BIC Rd, Rs\n", stdout);
            break;

        // MVN Rd, Rs
        case 0b010000'1111 ... 0b010000'1111:
            fputs("MVN Rd, Rs\n", stdout);
            break;
    
    // [5.5]
        // ADD Rd, Hs
        case 0b010001'00'0'1 ... 0b010001'00'0'1:
            fputs("ADD Rd, Hs\n", stdout);
            break;

        // ADD Hd, Rs
        case 0b010001'00'1'0 ... 0b010001'00'1'0:
            fputs("ADD Hd, Rs\n", stdout);
            break;

        // ADD Hd, Hs
        case 0b010001'00'1'1 ... 0b010001'00'1'1:
            fputs("ADD Hd, Hs\n", stdout);
            break;

        // CMP Rd, Hs
        case 0b010001'01'0'1 ... 0b010001'01'0'1:
            fputs("CMP Rd, Hs\n", stdout);
            break;

        // CMP Hd, Rs
        case 0b010001'01'1'0 ... 0b010001'01'1'0:
            fputs("CMP Hd, Rs\n", stdout);
            break;

        // CMP Hd, Hs
        case 0b010001'01'1'1 ... 0b010001'01'1'1:
            fputs("CMP Hd, Hs\n", stdout);
            break;

        // MOV Rd, Rs
        case 0b010001'10'0'1 ... 0b010001'10'0'1:
            fputs("MOV Rd, Rs\n", stdout);
            break;

        // MOV Hd, Rs
        case 0b010001'10'1'0 ... 0b010001'10'1'0:
            fputs("MOV Hd, Rs\n", stdout);
            break;

        // MOV Hd, Hs
        case 0b010001'10'1'1 ... 0b010001'10'1'1:
            fputs("MOV Hd, Hs\n", stdout);
            break;

        // BX Rs
        case 0b010001'11'0'0 ... 0b010001'11'0'0:
            fputs("BX Rs\n", stdout);
            break;

        // BX Hs
        case 0b010001'11'0'1 ... 0b010001'11'0'1:
            fputs("BX Hs\n", stdout);
            break;

    // [5.6]
        // LDR Rd, [PC, #Imm]
        case 0b0100100000 ... 0b0100111111:
            fputs("LDR Rd, [PC, #Imm]\n", stdout);
            break;

    // [5.7]
        // STR Rd, [Rb, Ro]
        case 0b0101'0'0'0000 ... 0b0101'0'0'0111:
            fputs("STR Rd, [Rb, Ro]\n", stdout);
            break;

        // STRB Rd, [Rb, Ro]
        case 0b0101'0'1'0000 ... 0b0101'0'1'0111:
            fputs("STRB Rd, [Rb, Ro]\n", stdout);
            break;
    
        // LDR Rd [Rb, Ro]
        case 0b0101'1'0'0000 ... 0b0101'1'0'0111:
            fputs("LDR Rd [Rb, Ro]\n", stdout);
            break;
    
        // LDRB Rd [Rb, Ro]
        case 0b0101'1'1'0000 ... 0b0101'1'1'0111:
            fputs("LDRB Rd [Rb, Ro]\n", stdout);
            break;
    
    // [5.8]
        // STRH Rd, [Rb, Ro] 
        case 0b0101'0'0'1000 ... 0b0101'0'0'1111:
            fputs("STRH Rd, [Rb, Ro] \n", stdout);
            break;

        // LDRH Rd, [Rb, Ro]
        case 0b0101'0'1'1000 ... 0b0101'0'1'1111:
            fputs("LDRH Rd, [Rb, Ro]\n", stdout);
            break;

        // LDSB Rd, [Rb, Ro]
        case 0b0101'1'0'1000 ... 0b0101'1'0'1111:
            fputs("LDSB Rd, [Rb, Ro]\n", stdout);
            break;
    
        // LDSH Rd, [Rb, Ro]
        case 0b0101'1'1'1000 ... 0b0101'1'1'1111:
            fputs("LDSH Rd, [Rb, Ro]\n", stdout);
            break;
    
    // [5.9]
        // STR Rd, [Rb, #Imm]
        case 0b011'0'0'00000 ... 0b011'0'0'11111:
            fputs("STR Rd, [Rb, #Imm]\n", stdout);
            break;

        // LDR Rd, [Rb, #Imm]
        case 0b011'1'0'00000 ... 0b011'1'0'11111:
            fputs("LDR Rd, [Rb, #Imm]\n", stdout);
            break;

        // STRB Rd, [Rb, #Imm]
        case 0b011'0'1'00000 ... 0b011'0'1'11111:
            fputs("STRB Rd, [Rb, #Imm]\n", stdout);
            break;

        // LDRB Rd, [Rb, #Imm]
        case 0b011'1'1'00000 ... 0b011'1'1'11111:
            fputs("LDRB Rd, [Rb, #Imm]\n", stdout);
            break;
    
    // [5.10]

        // STRH Rd, [Rb, #Imm]
        case 0b1000'0'00000 ... 0b1000'0'11111:
            fputs("STRH Rd, [Rb, #Imm]\n", stdout);
            break;

        // LDRH Rd, [Rb, #Imm]
        case 0b1000'1'00000 ... 0b1000'1'11111:
            fputs("LDRH Rd, [Rb, #Imm]\n", stdout);
            break;
    
    // [5.11]
        // STR Rd, [SP, #Imm]
        case 0b1001'0'00000 ... 0b1001'0'11111:
            fputs("STR Rd, [SP, #Imm]\n", stdout);
            break;   
        
        // LDR Rd, [SP, #Imm]
        case 0b1001'1'00000 ... 0b1001'1'11111:
            fputs("LDR Rd, [SP, #Imm]\n", stdout);
            break;

    // [5.12]
        // ADD Rd, PC, #Imm
        case 0b1010'0'00000 ... 0b1010'0'11111:
            fputs("ADD Rd, PC, #Imm\n", stdout);
            break;   
        
        // ADD Rd, SP, #Imm
        case 0b1010'1'00000 ... 0b1010'1'11111:
            fputs("ADD Rd, SP, #Imm\n", stdout);
            break;   
    
    // [5.13]
        // ADD SP, #Imm
        case 0b10110000'0'0 ... 0b10110000'0'1:
            fputs("ADD SP, #Imm\n", stdout);
            break;
        
        // ADD SP, #-Imm
        case 0b10110000'1'0 ... 0b10110000'1'1:
            fputs("ADD SP, #-Imm\n", stdout);
            break;

    // [5.14]
        // PUSH { Rlist }
        case 0b1011'0'10'0'00 ... 0b1011'0'10'0'11:
            fputs("PUSH { Rlist }\n", stdout);
            break;

        // PUSH { Rlist, LR }
        case 0b1011'0'10'1'00 ... 0b1011'0'10'1'11:
            fputs("PUSH { Rlist, LR }\n", stdout);
            break;

        // POP { Rlist }
        case 0b1011'1'10'0'00 ... 0b1011'1'10'0'11:
            fputs("POP { Rlist }\n", stdout);
            break;

        // POP { Rlist, PC }
        case 0b1011'1'10'1'00 ... 0b1011'1'10'1'11:
            fputs("POP { Rlist, PC }\n", stdout);
            break;
    
    // [5.15]
    // 0b0000000000000000
        // STMIA Rb!, { Rlist }
        case 0b1100'0'00000 ... 0b1100'0'11111:
            fputs("STMIA Rb!, { Rlist }\n", stdout);
            break;

        // LDMIA Rb!, { Rlist }
        case 0b1100'1'00000 ... 0b1100'1'11111:
            fputs("LDMIA Rb!, { Rlist }\n", stdout);
            break;
    
    // [5.16]
        // BEQ
        case 0b1101'0000'00 ... 0b1101'0000'11:
            fputs("BEQ\n", stdout);
            break;
        
        // BNE
        case 0b1101'0001'00 ... 0b1101'0001'11:
            fputs("BNE\n", stdout);
            break;
        
        // BCS
        case 0b1101'0010'00 ... 0b1101'0010'11:
            fputs("BCS\n", stdout);
            break;
        
        // BCC
        case 0b1101'0011'00 ... 0b1101'0011'11:
            fputs("BCC\n", stdout);
            break;
        
        // BMI
        case 0b1101'0100'00 ... 0b1101'0100'11:
            fputs("BMI\n", stdout);
            break;
        
        // BPL
        case 0b1101'0101'00 ... 0b1101'0101'11:
            fputs("BPL\n", stdout);
            break;
        
        // BVS
        case 0b1101'0110'00 ... 0b1101'0110'11:
            fputs("BVS\n", stdout);
            break;
        
        // BVC
        case 0b1101'0111'00 ... 0b1101'0111'11:
            fputs("BVC\n", stdout);
            break;
        
        // BHI
        case 0b1101'1000'00 ... 0b1101'1000'11:
            fputs("BHI\n", stdout);
            break;
        
        // BLS
        case 0b1101'1001'00 ... 0b1101'1001'11:
            fputs("BLS\n", stdout);
            break;
        
        // BGE
        case 0b1101'1010'00 ... 0b1101'1010'11:
            fputs("BGE\n", stdout);
            break;
        
        // BLT
        case 0b1101'1011'00 ... 0b1101'1011'11:
            fputs("BLT\n", stdout);
            break;
        
        // BGT
        case 0b1101'1100'00 ... 0b1101'1100'11:
            fputs("BGT\n", stdout);
            break;
        
        // BLE
        case 0b1101'1101'00 ... 0b1101'1101'11:
            fputs("BLE\n", stdout);
            break;

    // [5.17]
        // SWI Value8
        case 0b1101111100 ... 0b1101111111:
            fputs("SWI Value8\n", stdout);
            break;
    
    // [5.18]
        // B (unconditional branch)
        case 0b1110000000 ... 0b1110011111:
            fputs("B (unconditional branch)\n", stdout);
            break;

    // [5.19]
        // BL (offset high)
        case 0b1111'0'00000 ... 0b1111'0'11111:
            fputs("BL (offset high)\n", stdout);
            break;

        // BL (offset low)
        case 0b1111'1'00000 ... 0b1111'1'11111:
            fputs("BL (offset low)\n", stdout);
            break;
    }
}

} // namespace arm7tdmi

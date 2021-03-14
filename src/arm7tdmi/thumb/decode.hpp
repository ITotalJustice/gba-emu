#pragma once

#include "../arm7tdmi.hpp"
#include "alu_operations.hpp"
#include "branch.hpp"
#include "load_store.hpp"
#include "rlist.hpp"
#include "../../bit.hpp"

#include <cassert>

#define LOG(s) assert(0 && s);

namespace arm7tdmi::thumb {

constexpr auto thumb_decode_test(arm7tdmi& arm, const u16 op) {
    switch (bit::get_range<6, 15>(op)) {
    // [5.1]
        // LSL Rd, Rs, #Offset5
        case 0b000'00'00000 ... 0b000'00'11111:
            instruction_move_shifted_reg<shifted_reg_type::lsl>(arm,
                bit::get_range<6, 10>(op),
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)                
            );
            break;

        // LSR Rd, Rs, #Offset5
        case 0b000'01'00000 ... 0b000'01'11111:
            instruction_move_shifted_reg<shifted_reg_type::lsr>(arm,
                bit::get_range<6, 10>(op),
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)                
            );
            break;

        // ASR Rd, Rs, #Offset5
        case 0b000'10'00000 ... 0b000'10'11111:
            instruction_move_shifted_reg<shifted_reg_type::asr>(arm,
                bit::get_range<6, 10>(op),
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)                
            );
            break;
    
    // [5.2]
        // ADD Rd, Rs, Rn
        case 0b00011'0'0'000 ... 0b00011'0'0'111:
            instruction_add_generic<flags_cond::modify>(arm,
                bit::get_range<0, 2>(op),
                bit::get_range<3, 5>(op),
                arm.registers[bit::get_range<6, 8>(op)]                
            );
            break;
        
        // ADD Rd, Rs, #Offset3
        case 0b00011'1'0'000 ... 0b00011'1'0'111:
            instruction_add_generic<flags_cond::modify>(arm,
                bit::get_range<0, 2>(op),
                bit::get_range<3, 5>(op),
                bit::get_range<6, 8>(op)             
            );
            break;

        // SUB Rd, Rs, Rn
        case 0b00011'0'1'000 ... 0b00011'0'1'111:
            instruction_sub_generic<flags_cond::modify>(arm,
                bit::get_range<0, 2>(op),
                bit::get_range<3, 5>(op),
                arm.registers[bit::get_range<6, 8>(op)]                
            );
            break;
        
        // SUB Rd, Rs, #Offset3
        case 0b00011'1'1'000 ... 0b00011'1'1'111:
            instruction_sub_generic<flags_cond::modify>(arm,
                bit::get_range<0, 2>(op),
                bit::get_range<3, 5>(op),
                bit::get_range<6, 8>(op)             
            );
            break;

    // [5.3]
        // MOV Rd, #Offset8
        case 0b001'00'00000 ... 0b001'00'11111:
            instruction_mov_generic<flags_cond::modify>(arm,
                bit::get_range<8, 10>(op),
                bit::get_range<0, 7>(op)           
            );
            break;

        // CMP Rd, #Offset8
        case 0b001'01'00000 ... 0b001'01'11111:
            instruction_cmp_generic<flags_cond::modify>(arm,
                arm.registers[bit::get_range<8, 10>(op)],
                bit::get_range<0, 7>(op)           
            );
            break;

        // ADD Rd, #Offset8
        case 0b001'10'00000 ... 0b001'10'11111:
            instruction_add_generic<flags_cond::modify>(arm,
                bit::get_range<8, 10>(op),
                arm.registers[bit::get_range<8, 10>(op)],
                bit::get_range<0, 7>(op)           
            );
            break;

        // SUB Rd, #Offset8
        case 0b001'11'00000 ... 0b001'11'11111:
            instruction_sub_generic<flags_cond::modify>(arm,
                bit::get_range<8, 10>(op),
                arm.registers[bit::get_range<8, 10>(op)],
                bit::get_range<0, 7>(op)           
            );
            break;
    
    // [5.4]
        // AND Rd, Rs
        case 0b010000'0000 ... 0b010000'0000:
            instruction_and(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // EOR Rd, Rs
        case 0b010000'0001 ... 0b010000'0001:
            instruction_eor(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // LSL Rd, Rs
        case 0b010000'0010 ... 0b010000'0010:
            instruction_lsl(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // LSR Rd, Rs
        case 0b010000'0011 ... 0b010000'0011:
            instruction_lsr(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;
    
        // ASR Rd, Rs
        case 0b010000'0100 ... 0b010000'0100:
            instruction_asr(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // ADC Rd, Rs
        case 0b010000'0101 ... 0b010000'0101:
            instruction_adc(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // SBC Rd, Rs
        case 0b010000'0110 ... 0b010000'0110:
            instruction_sbc(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // ROR Rd, Rs
        case 0b010000'0111 ... 0b010000'0111:
            instruction_ror(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // TST Rd, Rs
        case 0b010000'1000 ... 0b010000'1000:
            instruction_tst(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;
        
        // NEG Rd, Rs
        case 0b010000'1001 ... 0b010000'1001:
            instruction_neg(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // CMP Rd, Rs
        case 0b010000'1010 ... 0b010000'1010:
            instruction_cmp(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // CMN Rd, Rs
        case 0b010000'1011 ... 0b010000'1011:
            instruction_cmn(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // ORR Rd, Rs
        case 0b010000'1100 ... 0b010000'1100:
            instruction_orr(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // MUL Rd, Rs
        case 0b010000'1101 ... 0b010000'1101:
            instruction_mul(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // BIC Rd, Rs
        case 0b010000'1110 ... 0b010000'1110:
            instruction_bic(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;

        // MVN Rd, Rs
        case 0b010000'1111 ... 0b010000'1111:
            instruction_mvn(arm,
                bit::get_range<3, 5>(op),
                bit::get_range<0, 2>(op)           
            );
            break;
    
    // [5.5]
        // ADD Rd, Hs
        case 0b010001'00'0'1 ... 0b010001'00'0'1:
            instruction_add_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op),
                arm.registers[bit::get_range<0, 2>(op)],
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // ADD Hd, Rs
        case 0b010001'00'1'0 ... 0b010001'00'1'0:
            instruction_add_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op) + 8,
                arm.registers[bit::get_range<0, 2>(op) + 8],
                arm.registers[bit::get_range<3, 5>(op)]
            );
            break;

        // ADD Hd, Hs
        case 0b010001'00'1'1 ... 0b010001'00'1'1:
            instruction_add_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op) + 8,
                arm.registers[bit::get_range<0, 2>(op) + 8],
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // CMP Rd, Hs
        case 0b010001'01'0'1 ... 0b010001'01'0'1:
            instruction_cmp_generic<flags_cond::modify>(arm,
                arm.registers[bit::get_range<0, 2>(op)],
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // CMP Hd, Rs
        case 0b010001'01'1'0 ... 0b010001'01'1'0:
            instruction_cmp_generic<flags_cond::modify>(arm,
                arm.registers[bit::get_range<0, 2>(op) + 8],
                arm.registers[bit::get_range<3, 5>(op)]
            );
            break;

        // CMP Hd, Hs
        case 0b010001'01'1'1 ... 0b010001'01'1'1:
            instruction_cmp_generic<flags_cond::modify>(arm,
                arm.registers[bit::get_range<0, 2>(op) + 8],
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // MOV Rd, Hs
        case 0b010001'10'0'1 ... 0b010001'10'0'1:
            instruction_mov_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op),
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // MOV Hd, Rs
        case 0b010001'10'1'0 ... 0b010001'10'1'0:
            instruction_mov_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op) + 8,
                arm.registers[bit::get_range<3, 5>(op)]
            );
            break;

        // MOV Hd, Hs
        case 0b010001'10'1'1 ... 0b010001'10'1'1:
            instruction_mov_generic<flags_cond::no_modify>(arm,
                bit::get_range<0, 2>(op) + 8,
                arm.registers[bit::get_range<3, 5>(op) + 8]
            );
            break;

        // BX Rs
        case 0b010001'11'0'0 ... 0b010001'11'0'0:
            LOG("BX Rs");
            break;

        // BX Hs
        case 0b010001'11'0'1 ... 0b010001'11'0'1:
            LOG("BX Hs");
            break;

    // [5.6]
        // LDR Rd, [PC, #Imm]
        case 0b0100100000 ... 0b0100111111:
            instruction_ldr<u32>(arm,
                arm.get_pc(),
                bit::get_range<0, 7>(op) << 2,
                bit::get_range<8, 10>(op)
            );
            break;

    // [5.7]
        // STR Rd, [Rb, Ro]
        case 0b0101'0'0'0000 ... 0b0101'0'0'0111:
            instruction_str<u32>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;

        // STRB Rd, [Rb, Ro]
        case 0b0101'0'1'0000 ... 0b0101'0'1'0111:
            instruction_str<u8>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;
    
        // LDR Rd [Rb, Ro]
        case 0b0101'1'0'0000 ... 0b0101'1'0'0111:
            instruction_ldr<u32>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;
    
        // LDRB Rd [Rb, Ro]
        case 0b0101'1'1'0000 ... 0b0101'1'1'0111:
            instruction_ldr<u8>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;
    
    // [5.8]
        // STRH Rd, [Rb, Ro] 
        case 0b0101'0'0'1000 ... 0b0101'0'0'1111:
            instruction_str<u16>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;

        // LDRH Rd, [Rb, Ro]
        case 0b0101'0'1'1000 ... 0b0101'0'1'1111:
            instruction_ldr<u16>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;

        // LDSB Rd, [Rb, Ro]
        case 0b0101'1'0'1000 ... 0b0101'1'0'1111:
            instruction_ldr<u8, sign_extend::yes>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;
    
        // LDSH Rd, [Rb, Ro]
        case 0b0101'1'1'1000 ... 0b0101'1'1'1111:
            instruction_ldr<u16, sign_extend::yes>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                arm.registers[bit::get_range<6, 8>(op)],
                bit::get_range<0, 2>(op)
            );
            break;
    
    // [5.9]
        // STR Rd, [Rb, #Imm]
        case 0b011'0'0'00000 ... 0b011'0'0'11111:
            instruction_str<u32>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op) << 2,
                bit::get_range<0, 2>(op)
            );
            break;
        
        // STRB Rd, [Rb, #Imm]
        case 0b011'1'0'00000 ... 0b011'1'0'11111:
            instruction_str<u8>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op),
                bit::get_range<0, 2>(op)
            );
            break;

        // LDR Rd, [Rb, #Imm]
        case 0b011'0'1'00000 ... 0b011'0'1'11111:
            instruction_ldr<u32>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op) << 2,
                bit::get_range<0, 2>(op)
            );
            break;

        // LDRB Rd, [Rb, #Imm]
        case 0b011'1'1'00000 ... 0b011'1'1'11111:
            instruction_ldr<u8>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op),
                bit::get_range<0, 2>(op)
            );
            break;
    
    // [5.10]
        // STRH Rd, [Rb, #Imm]
        case 0b1000'0'00000 ... 0b1000'0'11111:
            instruction_str<u16>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op) << 1,
                bit::get_range<0, 2>(op)
            );
            break;

        // LDRH Rd, [Rb, #Imm]
        case 0b1000'1'00000 ... 0b1000'1'11111:
            instruction_ldr<u16>(arm,
                arm.registers[bit::get_range<3, 5>(op)],
                bit::get_range<6, 10>(op) << 1,
                bit::get_range<0, 2>(op)
            );
            break;
    
    // [5.11]
        // STR Rd, [SP, #Imm]
        case 0b1001'0'00000 ... 0b1001'0'11111:
            instruction_str<u32>(arm,
                arm.get_sp(),
                bit::get_range<0, 7>(op) << 2,
                bit::get_range<8, 10>(op)
            );
            break;   
        
        // LDR Rd, [SP, #Imm]
        case 0b1001'1'00000 ... 0b1001'1'11111:
            instruction_str<u32>(arm,
                arm.get_sp(),
                bit::get_range<0, 7>(op) << 2,
                bit::get_range<8, 10>(op)
            );
            break;

    // [5.12]
        // ADD Rd, PC, #Imm
        case 0b1010'0'00000 ... 0b1010'0'11111:
            instruction_add_generic<flags_cond::no_modify>(arm,
                bit::get_range<8, 10>(op),
                arm.get_pc(),
                bit::get_range<0, 7>(op) << 2
            );
            break;   
        
        // ADD Rd, SP, #Imm
        case 0b1010'1'00000 ... 0b1010'1'11111:
            instruction_add_generic<flags_cond::no_modify>(arm,
                bit::get_range<8, 10>(op),
                arm.get_sp(),
                bit::get_range<0, 7>(op) << 2
            );
            break;   
    
    // [5.13]
        // ADD SP, #Imm
        case 0b10110000'0'0 ... 0b10110000'0'1:
            instruction_add_sp(arm,
                bit::get_range<0, 7>(op)
            );
            break;
        
        // ADD SP, #-Imm
        case 0b10110000'1'0 ... 0b10110000'1'1:
            instruction_add_sp(arm,
                bit::get_range<0, 7>(op)
            );
            break;

    // [5.14]
        // PUSH { Rlist }
        case 0b1011'0'10'0'00 ... 0b1011'0'10'0'11:
            instruction_push<push_pop_type::lr_0>(arm,
                bit::get_range<0, 7>(op)
            );
            break;

        // PUSH { Rlist, LR }
        case 0b1011'0'10'1'00 ... 0b1011'0'10'1'11:
            instruction_push<push_pop_type::lr_1>(arm,
                bit::get_range<0, 7>(op)
            );
            break;

        // POP { Rlist }
        case 0b1011'1'10'0'00 ... 0b1011'1'10'0'11:
            instruction_pop<push_pop_type::lr_0>(arm,
                bit::get_range<0, 7>(op)
            );
            break;

        // POP { Rlist, PC }
        case 0b1011'1'10'1'00 ... 0b1011'1'10'1'11:
            instruction_pop<push_pop_type::lr_1>(arm,
                bit::get_range<0, 7>(op)
            );
            break;
    
    // [5.15]
        // STMIA Rb!, { Rlist }
        case 0b1100'0'00000 ... 0b1100'0'11111:
            instruction_stmia(arm,
                bit::get_range<8, 10>(op),
                bit::get_range<0, 7>(op)
            );
            break;

        // LDMIA Rb!, { Rlist }
        case 0b1100'1'00000 ... 0b1100'1'11111:
            instruction_ldmia(arm,
                bit::get_range<8, 10>(op),
                bit::get_range<0, 7>(op)
            );
            break;
    
    // [5.16]
        // [COND] branch
        case 0b1101'0000'00 ... 0b1101'1101'11:
            instruction_cond_b(arm,
                bit::get_range<8, 11>(op),
                bit::get_range<0, 7>(op)
            );
            break;

    // [5.17]
        // SWI Value8
        case 0b1101111100 ... 0b1101111111:
            LOG("SWI Value8");
            break;
    
    // [5.18]
        // B (unconditional branch)
        case 0b1110000000 ... 0b1110011111:
            instruction_b(arm,
                bit::get_range<0, 10>(op)
            );
            break;

    // [5.19]
        // BL (offset high)
        case 0b1111'0'00000 ... 0b1111'0'11111:
            instruction_bl<bl_type::hi>(arm,
                bit::get_range<0, 10>(op)
            );
            break;

        // BL (offset low)
        case 0b1111'1'00000 ... 0b1111'1'11111:
            instruction_bl<bl_type::lo>(arm,
                bit::get_range<0, 10>(op)
            );
            break;
    }
}

} // namespace arm7tdmi::thumb

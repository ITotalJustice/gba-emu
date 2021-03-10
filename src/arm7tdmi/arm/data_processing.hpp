#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

namespace arm7tdmi::arm::data_processing {

enum class flags_cond {
    no_modify,
    modify,
};

// returns [new_pc, new_cpsr]
struct [[nodiscard]] arm_bx_result { u32 new_pc, new_cpsr; };
constexpr arm_bx_result arm_bx(const u32 cpsr, const u32 rn) {
    return { 
        .new_pc = rn,
        .new_cpsr = switch_state( // set new state to thumb if bit-0 = 1, else set to arm.
            cpsr,
            static_cast<state>(bit::is_set<0>(rn))
        )
    };
}

// returns new PC
void arm_bx(arm7tdmi_t* arm, const u8 rn) {
    assert(rn != 15 && "R15 is UB!");

    const auto [new_pc, new_cpsr] = arm_bx(arm->registers[16], arm->registers[rn]);
    arm->registers[15] = new_pc;
    arm->registers[16] = new_cpsr;
}

constexpr s32 branch_offset(const u32 offset) {
    return static_cast<s32>(bit::sign_extend<24>(offset) << 2);
}

[[nodiscard]]
constexpr u32 arm_b(const u32 pc, const s32 offset) {
    return pc + offset;
}

struct [[nodiscard]] arm_bl_result { u32 new_pc, new_link; };
constexpr auto arm_bl(const u32 pc, const s32 offset) -> arm_bl_result {
    // TODO: i think the mode bits are cleared...
    return {
        .new_pc = pc + offset,
        .new_link = pc
    };
}

void arm_b(arm7tdmi_t* arm, const s32 offset) {
    arm->registers[15] = arm_b(arm->registers[15], offset);
}

void arm_bl(arm7tdmi_t* arm, const s32 offset) {
    const auto [new_pc, new_link] = arm_bl(arm->registers[15], offset);
    arm->registers[14] = new_link;
    arm->registers[15] = new_pc;
}

struct Test { u32 result, cpsr; };

// V = no change
// C = carry from barrel shifter
// Z = r == 0
// N = (r & 31) > 0
template <flags_cond flag>
constexpr Test alu_flags_logical(const u32 result, const u32 cpsr, const bool barrel_carry) {
    if constexpr(flag == flags_cond::modify) {
        return {
            .result = result,
            .cpsr = set_flags<ftest::nzc>(
                cpsr,
                bit::is_set<31>(result),
                result == 0,
                barrel_carry
            )
        };
    }

    return { .result = result, .cpsr = cpsr };
}

enum class alu_logical {
    AND,
    EOR,
    ORR,
    BIC,
    TST,
    TEQ,
    MOV,
    MVN,
};

/*
.5.4 Writing to R15
    When Rd is a register other than R15, the condition code flags in the CPSR may be 
    updated from the ALU flags as described above. 
    When Rd is R15 and the S flag in the instruction is not set the result of the operation is 
    placed in R15 and the CPSR is unaffected.

    When Rd is R15 and the S flag is set the result of the operation is placed in R15 and 
    the SPSR corresponding to the current mode is moved to the CPSR. This allows state 
    changes which atomically restore both PC and CPSR. This form of instruction should 
    not be used in User mode.

4.5.5 Using R15 as an operand
    If R15 (the PC) is used as an operand in a data processing instruction the register is 
    used directly. 
    The PC value will be the address of the instruction, plus 8 or 12 bytes due to instruction 
    prefetching. If the shift amount is specified in the instruction, the PC will be 8 bytes 
    ahead. If a register is used to specify the shift amount the PC will be 12 bytes ahead.
*/
// template <alu_logical alu, flags_cond flag>
// constexpr Test alu_test(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
//     if constexpr(alu == alu_logical::AND) {

//     }
// }

// bitwise and
template <flags_cond flag>
constexpr Test arm_and2(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    const u32 result = op1 & op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise eor
template <flags_cond flag>
constexpr Test arm_eor(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    const u32 result = op1 ^ op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise or
template <flags_cond flag>
constexpr Test arm_orr(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    const u32 result = op1 | op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise op1 and not op2
template <flags_cond flag>
constexpr Test arm_bic(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    const u32 result = op1 & (~op2);
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise and, result is returned however it should be discard, only update flags!
template <flags_cond flag>
constexpr Test arm_tst(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_assert(flag == flags_cond::modify);
    const u32 result = op1 & op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise eor, result is returned however it should be discard, only update flags!
template <flags_cond flag>
constexpr Test arm_teq(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_assert(flag == flags_cond::modify);
    const u32 result = op1 ^ op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise mov op2
template <flags_cond flag>
constexpr Test arm_mov(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_cast<void>(op1);
    const u32 result = op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise mmob not op2
template <flags_cond flag>
constexpr Test arm_mvn(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_cast<void>(op1);
    const u32 result = ~op2;
    return alu_flags_logical<flag>(result, cpsr, barrel_carry);
}

// bitwise and
template <flags_cond flag>
void arm_and(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_and2<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

// bitwise exclusive or
template <flags_cond flag>
void arm_eor(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_eor<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

// bitwise or
template <flags_cond flag>
void arm_orr(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_orr<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

// bitwise and
template <flags_cond flag>
void arm_tst(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [_, cpsr] = arm_tst<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[16] = cpsr;
}

// bitwise eor
template <flags_cond flag>
void arm_teq(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [_, cpsr] = arm_teq<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[16] = cpsr;
}

// bitwise or
template <flags_cond flag>
void arm_bic(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_bic<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

// bitwise or
template <flags_cond flag>
void arm_mov(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_mov<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

// bitwise or
template <flags_cond flag>
void arm_mvn(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_mvn<flag>(
        arm->registers[16], op1, op2, arm->barrel_carry
    );
    arm->registers[rd] = result;
    arm->registers[16] = cpsr;
}

} // arm7tdmi::arm::data_processing

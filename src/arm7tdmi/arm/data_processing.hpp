#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

namespace arm7tdmi::arm {

enum class flags_cond {
    no_modify,
    modify,
};

// returns new PC
// TODO: check that this is correct
constexpr void instruction_bx(arm7tdmi& arm, const u8 rn) {
    assert(rn != 15 && "R15 is UB!");

    const auto new_pc = arm.registers[rn];
    const auto new_cpsr = switch_state(
        arm.registers[16],
        static_cast<state>(bit::is_set<0>(new_pc))
    );

    arm.registers[15] = new_pc;
    arm.registers[16] = new_cpsr;
}

constexpr s32 branch_offset(const u32 offset) {
    return static_cast<s32>(bit::sign_extend<24>(offset) << 2);
}

constexpr void instruction_b(arm7tdmi& arm, const s32 offset) {
    arm.set_pc(arm.get_pc() + offset);
}

constexpr void instruction_bl(arm7tdmi& arm, const s32 offset) {
    const auto pc = arm.get_pc();
    // TODO: i think the mode bits are cleared...
    arm.set_lr(pc);
    arm.set_pc(pc + offset);
}

enum class save_result {
    yes, no
};

// V = no change
// C = carry from barrel shifter
// Z = r == 0
// N = (r & 31) > 0
template <flags_cond flag, save_result save = save_result::yes>
constexpr void alu_flags_logical(arm7tdmi& arm, const u32 result, const u8 rd, const bool barrel_carry) {
    if constexpr(save == save_result::no) {
        static_assert(
            flag == flags_cond::modify,
            "Not saving result or modifying flags!"
        );
    }

    if constexpr(flag == flags_cond::modify) {
        arm.cpsr.set_flags<ftest::nzc>(
            bit::is_set<31>(result),
            result == 0,
            barrel_carry
        );
    }

    if constexpr(save == save_result::yes) {
        arm.registers[rd] = result;
    }
}

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

// bitwise and
template <flags_cond flag>
constexpr void instruction_and(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 & op2;
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

// bitwise exclusive or
template <flags_cond flag>
constexpr void instruction_eor(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 ^ op2;
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

// bitwise or
template <flags_cond flag>
constexpr void instruction_orr(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 | op2;
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

// bitwise and, discard result
template <flags_cond flag>
constexpr void instruction_tst(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 & op2;
    alu_flags_logical<flag, save_result::no>(arm, result, rd, barrel_carry);
}

// bitwise eor, discard result
template <flags_cond flag>
constexpr void instruction_teq(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 ^ op2;
    alu_flags_logical<flag, save_result::no>(arm, result, rd, barrel_carry);
}

// bitwise or
template <flags_cond flag>
constexpr void instruction_bic(arm7tdmi& arm, const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op1 & (~op2);
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

// bitwise or
template <flags_cond flag>
constexpr void instruction_mov(arm7tdmi& arm, [[maybe_unused]] const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = op2;
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

// bitwise or
template <flags_cond flag>
constexpr void instruction_mvn(arm7tdmi& arm, [[maybe_unused]] const u32 op1, const u32 op2, const u8 rd, const bool barrel_carry) {
    const auto result = ~op2;
    alu_flags_logical<flag>(arm, result, rd, barrel_carry);
}

} // arm7tdmi::arm

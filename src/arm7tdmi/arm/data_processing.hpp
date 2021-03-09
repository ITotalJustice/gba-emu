#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

#include <array>

namespace arm7tdmi::arm::data_processing {

enum class flags_cond {
    no_modify,
    modify,
};

// returns [new_pc, new_cpsr]
struct [[nodiscard]] arm_bx_result { u32 new_pc, new_cpsr; };
constexpr arm_bx_result arm_bx(const u32 cpsr, const u32 rn) {
    return { 
        rn,
        switch_state( // set new state to thumb if bit-0 = 1, else set to arm.
            cpsr,
            static_cast<state>(bit::is_set<0>(rn))
        )
    };
}

// returns new PC
constexpr void arm_bx(arm7tdmi_t* arm, const u8 rn) {
    assert(
        rn != 15 &&
        "R15 is UB!"
    );

    const auto [new_pc, new_cpsr] = arm_bx(arm->registers[16], arm->registers[rn]);
    arm->registers[15] = new_pc;
    arm->registers[16] = new_cpsr;
}

constexpr s32 branch_offset(const u32 offset) {
    // not sure if sign_extend works on all platforms, so just in case...
    if constexpr(bit::sign_extend<24>(0b1100'1111'1111'1111'1111'1111) == 0b1111'1111'0011'1111'1111'1111'1111'1100) {
        return static_cast<s32>(bit::sign_extend<24>(offset) << 2);
    }

    if (bit::is_set<23>(offset)) {
        return static_cast<s32>(0b1111'1111'0000'0000'0000'0000'0000'0000 | (offset << 2));
    }
    return static_cast<s32>(offset << 2);
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

constexpr void arm_b(arm7tdmi_t* arm, const s32 offset) {
    arm->registers[15] = arm_b(arm->registers[15], offset);
}

constexpr void arm_bl(arm7tdmi_t* arm, const s32 offset) {
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
        return {result, set_flags<ftest::nzc>(cpsr,
            bit::is_set<31>(result),
            result == 0,
            barrel_carry
        )};
    }

    return {result, cpsr};
}

// bitwise and
template <flags_cond flag>
constexpr Test arm_and(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
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
    return idk<flag>(result, cpsr, barrel_carry);
}

// bitwise mov op2
template <flags_cond flag>
constexpr Test arm_mov(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_cast<void>(op1);
    const u32 result = op2;
    return idk<flag>(result, cpsr, barrel_carry);
}

// bitwise mmob not op2
template <flags_cond flag>
constexpr Test arm_mvn(const u32 cpsr, const u32 op1, const u32 op2, const bool barrel_carry) {
    static_cast<void>(op1);
    const u32 result = ~op2;
    return idk<flag>(result, cpsr, barrel_carry);
}

constexpr void arm_and_test() {
    struct Args {
        struct Value {
            u32 cpsr, op1, op2;
            bool carry;
        } value;
        struct Result {
            u32 result, cpsr;
        } result;
    };

    constexpr std::array<Args, 4> args{{
        {
            .value{0x00, 0x00, 0x00, true},
            .result{0x00, 0b0110'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0x80, 0x80, true},
            .result{0x80, 0b0010'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0xFFFFFFFF, 0xFFFFFFFF, true},
            .result{0xFF'FF'FF'FF, 0b1010'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0x00, 0x00, false},
            .result{0x00, 0b0100'0000'0000'0000'0000'0000'0000'0000}
        }
    }};

    constexpr auto func_test = [](const auto span) {
        for (const auto [value, result] : span) {
            const auto r = arm_and<flags_cond::modify>(value.cpsr, value.op1, value.op2, value.carry);
            if (r.result != result.result || r.cpsr != result.cpsr) {
                return false;
            }
        }
        return true;
    };

    static_assert(
        func_test(args) == true,
        "failed to pass arm AND test!"
    );
}

// bitwise and
template <flags_cond flag>
void arm_and(arm7tdmi_t* arm, const u32 op1, const u32 op2, const u8 rd) {
    const auto [result, cpsr] = arm_and<flag>(
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

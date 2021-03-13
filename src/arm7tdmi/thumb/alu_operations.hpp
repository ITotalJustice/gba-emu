#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

namespace arm7tdmi::thumb {

// todo: use builtins (if possible!)
/*
__builtin_uadd_overflow
__builtin_umul_overflow
__builtin_smul_overflow
*/

// basically checks if the sign bit of a and b match, but differ from r
// if bit 31 of a, b = 1 and r = 0, then overflow happened
// if bit 31 of a, b = 0 and r = 1, then overflow happened
// if bit 31 of a, b = 1 and r = 1, then overflow did not happen
// if bit 31 of a = 1, b = 0 and r = X, then overflow did not happen
constexpr auto calc_v_flag(const u32 a, const u32 b, const u32 r) {
    return ((a & 0x80000000) == (b & 0x80000000)) && ((a & 0x80000000) != (r & 0x80000000));
}

enum class flags_cond {
    no_modify,
    modify,
};

// these instructions are used by taking in different params
// a is alwys the left-side param.
// so for sub(a, b) would be { result = (a - b) };
template <flags_cond cond>
constexpr void instruction_cmp_generic(arm7tdmi& arm, const u32 a, const u32 b) {
    static_assert(cond == flags_cond::modify);
    
    const s64 result = a - b;

    arm.cpsr.set_flags<ftest::nzcv>(
        bit::is_set<31>(result),
        result == 0,
        result < 0,
        calc_v_flag(a, b, result)
    );
}

template <flags_cond cond>
constexpr auto instruction_add_generic(arm7tdmi& arm, const u8 rd, const u32 a, const u32 b) {
    const u64 result = a + b;

    if constexpr(cond == flags_cond::modify) {
        arm.cpsr.set_flags<ftest::nzcv>(
            bit::is_set<31>(result),
            (result & 0xFF'FF'FF'FFU) == 0,
            result > 0xFF'FF'FF'FFU,
            calc_v_flag(a, b, result)
        );
    }
    
    arm.registers[rd] = static_cast<u32>(result);
}

template <flags_cond cond>
constexpr auto instruction_sub_generic(arm7tdmi& arm, const u8 rd, const u32 a, const u32 b) {
    const s64 result = a - b;

    if constexpr(cond == flags_cond::modify) {
        arm.cpsr.set_flags<ftest::nzcv>(
            bit::is_set<31>(result),
            result == 0,
            result < 0,
            calc_v_flag(a, b, result)
        );
    }
    
    arm.registers[rd] = static_cast<u32>(result);
}

template <flags_cond cond>
constexpr void instruction_mov_generic(arm7tdmi& arm, const u8 rd, const u32 a) {
    const auto result = a;

    if constexpr(cond == flags_cond::modify) {
        arm.cpsr.set_flags<ftest::nz>(
            bit::is_set<31>(result),
            result == 0
        );
    }

    arm.registers[rd] = result;
}

// the rest of the instructions are normal alu ops

constexpr void instruction_and(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");
    
    const auto result = arm.registers[rd] & arm.registers[rs];
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
    arm.registers[rd] = result;
}

constexpr void instruction_eor(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto result = arm.registers[rd] ^ arm.registers[rs];
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
    arm.registers[rd] = result;
}

constexpr void instruction_lsl(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto [result, new_carry] = barrel::shift<barrel::type::lsl>(
        arm.registers[rd], arm.registers[rs] & 0xFF,
        arm.cpsr.get_flag<flags::C>()
    );

    arm.cpsr.set_flags<ftest::nzc>(
        bit::is_set<31>(result),
        result == 0,
        new_carry
    );

    arm.registers[rd] = result;
}

constexpr void instruction_lsr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto [result, new_carry] = barrel::shift<barrel::type::lsr>(
        arm.registers[rd], arm.registers[rs] & 0xFF,
        arm.cpsr.get_flag<flags::C>()
    );

    arm.cpsr.set_flags<ftest::nzc>(
        bit::is_set<31>(result),
        result == 0,
        new_carry
    );

    arm.registers[rd] = result;
}

constexpr void instruction_asr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto [result, new_carry] = barrel::shift<barrel::type::asr>(
        arm.registers[rd], arm.registers[rs] & 0xFF,
        arm.cpsr.get_flag<flags::C>()
    );

    arm.cpsr.set_flags<ftest::nzc>(
        bit::is_set<31>(result),
        result == 0,
        new_carry
    );

    arm.registers[rd] = result;
}

constexpr void instruction_adc(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto carry = arm.cpsr.get_flag<flags::C>();
    instruction_add_generic<flags_cond::modify>(arm, rd,
        arm.registers[rd], arm.registers[rs] + carry
    );
}

constexpr void instruction_sbc(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto carry = arm.cpsr.get_flag<flags::C>();
    instruction_sub_generic<flags_cond::modify>(arm, rd,
        arm.registers[rd], arm.registers[rs] + carry
    );
}

constexpr void instruction_ror(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto [result, new_carry] = barrel::shift<barrel::type::ror>(
        arm.registers[rd], arm.registers[rs] & 0xFF,
        arm.cpsr.get_flag<flags::C>()
    );

    arm.cpsr.set_flags<ftest::nzc>(
        bit::is_set<31>(result),
        result == 0,
        new_carry
    );

    arm.registers[rd] = result;
}

// same as instruction_and but the result is discarded
constexpr void instruction_tst(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto result = arm.registers[rd] & arm.registers[rs];
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
}

constexpr void instruction_neg(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");
    
    const u32 result = (~arm.registers[rs]) + 1; // same as -rs

    arm.cpsr.set_flags<ftest::nzcv>(
        bit::is_set<31>(result),
        result == 0,
        1, // i think this is always set?
        calc_v_flag(arm.registers[rd], arm.registers[rs], result)
    );
    
    arm.registers[rd] = result & 0xFF'FF'FF'FFU;
}

constexpr void instruction_cmp(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    instruction_cmp_generic<flags_cond::modify>(
        arm, arm.registers[rd], arm.registers[rs]
    );
}

constexpr void instruction_cmn(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const u64 result = arm.registers[rd] + arm.registers[rs];

    arm.cpsr.set_flags<ftest::nzcv>(
        bit::is_set<31>(result),
        (result & 0xFF'FF'FF'FFU) == 0,
        result > 0xFF'FF'FF'FFU,
        calc_v_flag(arm.registers[rd], arm.registers[rs], result)
    );
}

constexpr void instruction_orr(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto result = arm.registers[rd] | arm.registers[rs];
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
    arm.registers[rd] = result;
}

constexpr void instruction_mul(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const u64 result = arm.registers[rd] * arm.registers[rs];

    arm.cpsr.set_flags<ftest::nzcv>(
        bit::is_set<31>(result),
        (result & 0xFF'FF'FF'FFU) == 0,
        0, // gbatek says carry is destroyed in armv4...
        calc_v_flag(arm.registers[rd], arm.registers[rs], result)
    );

    arm.registers[rd] = result & 0xFF'FF'FF'FFU;
}

constexpr void instruction_bic(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");

    const auto result = arm.registers[rd] & (~arm.registers[rs]);
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
    arm.registers[rd] = result;
}

constexpr void instruction_mvn(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert(rs <= 7 && rd <= 7 && "not a lo register oprand!");
    
    const auto result = ~arm.registers[rs];
    arm.cpsr.set_flags<ftest::nz>(
        bit::is_set<31>(result),
        result == 0
    );
    arm.registers[rd] = result;
}

// pass in the offset as u8 rather than s8 because we'd need to cast it
// to an unsigned value anyway as left shift signed value is UB.
constexpr auto instruction_add_sp(arm7tdmi& arm, const u8 offset) {
    const auto shift_v = static_cast<s16>(bit::sign_extend<8>(offset << 1));
    arm.set_sp(arm.get_sp() + shift_v);
}

constexpr void instruction_add_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {  
    assert((rs >= 8 || rd >= 8) && "no hi registers were passed!");

    instruction_add_generic<flags_cond::no_modify>(
        arm, rd, arm.registers[rd], arm.registers[rs]
    );
}

constexpr void instruction_cmp_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {
    assert((rs >= 8 || rd >= 8) && "no hi registers were passed!");
    
    instruction_cmp_generic<flags_cond::modify>(
        arm, arm.registers[rd], arm.registers[rs]
    );
}

constexpr void instruction_mov_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {  
    assert((rs >= 8 || rd >= 8) && "no hi registers were passed!");

    instruction_mov_generic<flags_cond::no_modify>(
        arm, rd, arm.registers[rs]
    );
}


constexpr auto test_and() {
    constexpr auto func = []() {
        constexpr auto rs = 0;
        constexpr auto rd = 5;

        arm7tdmi arm{};
        arm.registers[rs] = 0x31;
        arm.registers[rd] = 0xFF;

        instruction_and(arm, rs, rd);

        return arm.registers[rd] == 0x31;
    };

    static_assert(func());
}

constexpr auto test_asr() {
    constexpr auto func = []() {
        constexpr auto rs = 0;
        constexpr auto rd = 5;

        arm7tdmi arm{};
        arm.registers[rs] = 258; // same as 2 (258 & 0xFF == 2)
        arm.registers[rd] = 0b1000'1111'1111'1111'1111'1111'1111'1100;

        instruction_asr(arm, rs, rd);

        return arm.registers[rd] == 0b1110'0011'1111'1111'1111'1111'1111'1111;
    };

    static_assert(func());
}

} // namespace arm7tdmi::thumb

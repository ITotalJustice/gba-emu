#pragma once

#include "../arm7tdmi.hpp"

namespace arm7tdmi::arm::multiply {

enum class flags_cond {
    no_modify,
    modify,
};

struct [[nodiscard]] mul_result { u32 result, cpsr; };

template <flags_cond flags>
constexpr mul_result mul_flags(const u32 result, const u32 cpsr) {
    // flag V is set to a random value...
    if constexpr(flags == flags_cond::modify) {
        return {
            .result = result,
            .cpsr = set_flags<ftest::nz>(
                cpsr,
                bit::is_set<31>(result),
                result == 0
            )
        };
    }
    return { .result = result, .cpsr = cpsr };
}

template <flags_cond flags>
constexpr mul_result mul(const u32 cpsr, const u32 rm, const u32 rs) {
    return mul_flags<flags>(rm * rs, cpsr);
}

template <flags_cond flags>
constexpr mul_result mla(const u32 cpsr, const u32 rm, const u32 rs, const u32 rn) {
    return mul_flags<flags>(rm * rs + rn, cpsr);
}

struct [[nodiscard]] mull_result { u32 lo, hi, cpsr; };

template <flags_cond flags>
constexpr mull_result mull_flags(const u64 result, const u32 cpsr) {
    // flag V and C are set to random values...
    if constexpr(flags == flags_cond::modify) {
        return {
            .lo = result & 0xFF'FF'FF'FF,
            .hi = result >> 32,
            .cpsr = set_flags<ftest::nz>(
                cpsr,
                bit::is_set<63>(result),
                result ==  0
            )
        };
    }
    return { .lo = result & 0xFF'FF'FF'FF, .hi = result >> 32, .cpsr = cpsr };
}

template <flags_cond flags>
constexpr mull_result umull(const u32 cpsr, const u32 rm, const u32 rs) {
    return mull_flags<flags>(rm * rs, cpsr);
}

template <flags_cond flags>
constexpr mull_result umlal(const u32 cpsr, const u32 rdlo, const u32 rdhi, const u32 rm, const u32 rs) {
    const u64 add_value = (static_cast<u64>(rdhi) << 32) | rdlo;
    return mull_flags<flags>(rm * rs + add_value, cpsr);
}

template <flags_cond flags>
constexpr mull_result smull(const u32 cpsr, const s32 rm, const s32 rs) {
    return mull_flags<flags>(rm * rs, cpsr);
}

template <flags_cond flags>
constexpr mull_result smlal(const u32 cpsr, const s32 rdlo, const s32 rdhi, const s32 rm, const s32 rs) {
    const s64 add_value = (static_cast<u64>(rdhi) << 32) | rdlo;
    return mull_flags<flags>(static_cast<u64>(rm * rs + add_value), cpsr);
}

template <flags_cond flags>
constexpr void mul(arm7tdmi& arm, const u8 rd, const u8 rm, const u8 rs) {
    const auto [result, cpsr] = mul<flags>(arm.registers[16],
        arm.registers[rm], arm.registers[rs]
    );

    arm.registers[16] = cpsr;
    arm.registers[rd] = result;
}

template <flags_cond flags>
constexpr void mla(arm7tdmi& arm, const u8 rd, const u8 rm, const u8 rs, const u8 rn) {
    const auto [result, cpsr] = mla<flags>(arm.registers[16],
        arm.registers[rm], arm.registers[rs], arm.registers[rn]
    );

    arm.registers[16] = cpsr;
    arm.registers[rd] = result;
}

template <flags_cond flags>
constexpr void umull(arm7tdmi& arm, const u8 rdlo, const u8 rdhi, const u8 rm, const u8 rs) {
    const auto [lo, hi, cpsr] = umull<flags>(arm.registers[16],
        arm.registers[rm], arm.registers[rs]
    );

    arm.registers[16] = cpsr;
    arm.registers[rdlo] = lo;
    arm.registers[rdhi] = hi;
}

template <flags_cond flags>
constexpr void umlal(arm7tdmi& arm, const u8 rdlo, const u8 rdhi, const u8 rm, const u8 rs, const u8 rn) {
    const auto [lo, hi, cpsr] = umlal<flags>(arm.registers[16],
        arm.registers[rdlo], arm.registers[rdhi], arm.registers[rm], arm.registers[rs]
    );

    arm.registers[16] = cpsr;
    arm.registers[rdlo] = lo;
    arm.registers[rdhi] = hi;
}

template <flags_cond flags>
constexpr void smull(arm7tdmi& arm, const u8 rdlo, const u8 rdhi, const u8 rm, const u8 rs) {
    const auto [lo, hi, cpsr] = smull<flags>(arm.registers[16],
        arm.registers[rm], arm.registers[rs]
    );

    arm.registers[16] = cpsr;
    arm.registers[rdlo] = lo;
    arm.registers[rdhi] = hi;
}

template <flags_cond flags>
constexpr void smlal(arm7tdmi& arm, const u8 rdlo, const u8 rdhi, const u8 rm, const u8 rs, const u8 rn) {
    const auto [lo, hi, cpsr] = smlal<flags>(arm.registers[16],
        arm.registers[rdlo], arm.registers[rdhi], arm.registers[rm], arm.registers[rs]
    );

    arm.registers[16] = cpsr;
    arm.registers[rdlo] = lo;
    arm.registers[rdhi] = hi;
}

} // namespace arm7tdmi::arm::multiply


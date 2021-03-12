#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

#include <cassert>

namespace arm7tdmi::thumb {

// basically checks if the sign bit of a and b match, but differ from r
// if bit 31 of a, b = 1 and r = 0, then overflow happened
// if bit 31 of a, b = 0 and r = 1, then overflow happened
// if bit 31 of a, b = 1 and r = 1, then overflow did not happen
// if bit 31 of a = 1, b = 0 and r = X, then overflow did not happen
constexpr auto calc_v_flag(const u32 a, const u32 b, const u32 r) {
    return ((a & 0x80000000) == (b & 0x80000000)) && ((a & 0x80000000) != (r & 0x80000000));
}

constexpr void instruction_add_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {  
    arm.registers[rd] += arm.registers[rs];
}

constexpr void instruction_cmp_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {
    const s64 result = arm.registers[rd] - arm.registers[rs];

    arm.cpsr.set_flags<ftest::nzcv>(
        bit::is_set<31>(result),
        result == 0,
        result < 0,
        calc_v_flag(arm.registers[rd], arm.registers[rs], result)
    );
}

constexpr void instruction_mov_hi(arm7tdmi& arm, const u8 rs, const u8 rd) {  
    arm.registers[rd] = arm.registers[rs];
}

// TODO: BX

} // namespace arm7tdmi::thumb

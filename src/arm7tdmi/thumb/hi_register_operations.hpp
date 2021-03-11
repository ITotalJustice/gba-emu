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

constexpr void instruction_ldr_pc_relative(arm7tdmi& arm, const u8 rd, const u8 word8) {
    const u32 offset = word8 << 2;
    // mask lower 2-bits to ensure the addr is word-aligned
    const u32 addr = (arm.get_pc() & 0xFF'FF'FF'FC) + offset;
    // load value from mem [addr] into reg[rd];
    (void)rd;
}

} // namespace arm7tdmi::thumb

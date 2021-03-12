#pragma once

#include "../arm7tdmi.hpp"

#include <cassert>

namespace arm7tdmi::thumb {

constexpr auto instruction_cond_b(arm7tdmi& arm, const u8 cond, const s16 offset) {
    assert(offset <= 0b1111'1111 && "offset is bigger than 8-bits!");
    assert(cond <= 0b1101 && "invalid cond arg!");

    // check if we jump!
    if (test_cond(arm.get_cpsr(), cond) == true) {
        const auto shifted_offset = offset << 1;
        arm.set_pc(arm.get_pc() + shifted_offset);
    }
}

constexpr auto instruction_b(arm7tdmi& arm, const u16 offset) {
    assert(offset <= 0b111'1111'1111 && "offset is bigger than 11-bits!");
    
    const auto shifted_offset = offset << 1;
    arm.set_pc(arm.get_pc() + shifted_offset);
}

enum class bl_type {
    hi, lo
};

template <bl_type type>
constexpr auto instruction_bl(arm7tdmi& arm, const u16 offset) {
    assert(offset <= 0b111'1111'1111 && "offset is bigger than 11-bits!");

    if constexpr(type == bl_type::hi) {
        const auto shifted_offset = static_cast<u32>(offset) << 12;
        arm.set_lr(shifted_offset);
    }
    if constexpr(type == bl_type::lo) {
        // this might not be correct...
        const auto next_addr = arm.get_pc();
        const auto shifted_offset = static_cast<u32>(offset) << 1;
        
        arm.set_pc(arm.get_lr() + shifted_offset);
        arm.set_lr(next_addr | 1); // bit-0 is set
    }
}

} // namespace arm7tdmi::thumb

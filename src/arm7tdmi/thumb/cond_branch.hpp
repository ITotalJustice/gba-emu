#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

#include <cassert>

namespace arm7tdmi::thumb {

// the offset should be shifted left 1
// instruction_branch(arm, cond, offset << 1)
constexpr auto instruction_branch(arm7tdmi& arm, const u8 cond, const s16 offset) {
    assert(offset <= 0b1'1111'1111 && "offset is bigger than 9-bits!");
    assert(cond <= 0b1101 && "invalid cond arg!");

    // check if we jump!
    if (test_cond(arm.get_cpsr(), cond) == true) {
        arm.set_pc(arm.get_pc() + offset);
    }
}

} // namespace arm7tdmi::thumb

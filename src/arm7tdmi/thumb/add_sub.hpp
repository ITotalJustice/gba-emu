#pragma once

#include "../arm7tdmi.hpp"

#include <cassert>

namespace arm7tdmi::thumb {

// pass in the offset as u8 rather than s8 because we'd need to cast it
// to an unsigned value anyway as left shift signed value is UB.
constexpr auto instruction_add_sp(arm7tdmi& arm, const u8 offset) {
    const auto shift_v = static_cast<s16>(bit::sign_extend<8>(offset << 1));
    arm.set_sp(arm.get_sp() + shift_v);
}

} // namespace arm7tdmi::thumb

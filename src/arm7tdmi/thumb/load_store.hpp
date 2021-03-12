#pragma once

#include "../arm7tdmi.hpp"
#include "../barrel_shifter.hpp"

#include <cassert>

namespace arm7tdmi::thumb {

enum class sign_extend {
    no, yes
};

template <typename T>
constexpr auto instruction_str(arm7tdmi& arm, const u32 a, const u32 b, const u8 rd) {
    const u32 addr = a + b;
    arm.mmio.write<T>(addr, arm.registers[rd] & bit::get_mask<T>());
}

template <typename T, sign_extend sign = sign_extend::no>
constexpr auto instruction_ldr(arm7tdmi& arm, const u32 a, const u32 b, const u8 rd) {
    const u32 addr = a + b;
    arm.registers[rd] = arm.mmio.read<T>(addr);

    if constexpr(sign == sign_extend::yes) {
        static_assert(
            sizeof(T) == sizeof(u16),
            "only halfword (LDRSB, LDRSH) is supported for sign extended loads!"
        );

        arm.registers[rd] = bit::sign_extend<16>(arm.registers[rd]);
    }
}

} // namespace arm7tdmi::thumb

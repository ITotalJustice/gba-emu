#pragma once

#include "arm7tdmi.hpp"
#include "bit.hpp"

#include <cstdint>
// #include <bit>

namespace arm7tdmi {

// unused yet. some shifts update flags (i think) whilst others don't
// idea is to pass this enum as template arg then have if constexpr()
// to switch functions at compile time, so the caller function is consistent
// and there's no duplicate function / overloading.
enum class shift {
    no_flags,
    flags_nzcv,
};

// NOTE: might make these templated args as i imagine that
// there are signed shifted values.
// however signed shifted values in c/c++ are by default arithmetic shifts
// this means the sign bit is presvered, so logicals will all be arithmetic.
// this likely isn't the desired behaviour...

// All shifts are mask 31 (&31) as to not cause UB when shifting by more
// bits than possible.
constexpr u32 shift_logical_left(const u32 v, const u32 shift) {
    return v << (shift & 31U);
}

constexpr u32 shift_logical_right(const u32 v, const u32 shift) {
    return v >> (shift & 31U);
}

constexpr u32 shift_arithmetic_right(const u32 v, const u32 shift) {
    // signed bit should always be preserved, so casting works...
    static_assert(
        -16 >> 1U == -8,
        "arithemtic shift is not supported for this platform!"
    );

    return static_cast<u32>(static_cast<s32>(v) >> shift);
}

constexpr u32 shift_rotate_right(const u32 v, const u32 shift) {
    return bit::rotr(v, shift);
}

namespace barrel {

enum class shift_type {
    lsl, lsr, asr, ror,
};

struct [[nodiscard]] shift_result { u32 result; bool carry; };
template <shift_type type>
constexpr auto shift(const u32 v, const u32 shift, const bool old_carry) -> shift_result {
    if constexpr(type == shift_type::lsl) {
        switch (shift) {
            case 0: return { v, old_carry };
            case 1 ... 31: return { v << shift, bit::is_set(v, 32 - shift) };
            case 32: return { 0, bit::is_set<0>(v) };
            default: return { 0, false };
        }
    }
    if constexpr(type == shift_type::lsr) {
        assert(shift != 0 && "lsr #0 should be encoded as lsl #0!");

        if (shift < 32) [[likely]] {
            return {v >> shift, bit::is_set(v, shift - 1)};
        } else if (v == 32) {
            return {0, bit::is_set<31>(v)};
        } else {
            return {0, false};
        }
    }
    if constexpr(type == shift_type::asr) {
        assert(shift != 0 && "asr #0 should be encoded as lsl #0!");

        if (shift < 32) [[likely]] {
            return {shift_arithmetic_right(v, shift), bit::is_set(v, shift - 1)};
        } else {
            return {shift_arithmetic_right(v, 31), bit::is_set<31>(v)};
        }
    }
    if constexpr(type == shift_type::ror) {
        assert(shift != 0 && "ror #0 should be encoded as lsl #0!");

        if (shift < 32) [[likely]] {
            return {shift_arithmetic_right(v, shift), bit::is_set(v, shift - 1)};
        } else {
            return {shift_arithmetic_right(v, 31), bit::is_set<31>(v)};
        }
    }
}

} // namespace barrel
} // namespace arm7tdmi

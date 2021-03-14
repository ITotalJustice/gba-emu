#pragma once

#include "arm7tdmi.hpp"
#include "../bit.hpp"

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
[[nodiscard]]
constexpr u32 shift_logical_left(const u32 v, const u32 shift) {
    return v << (shift & 31U);
}

[[nodiscard]]
constexpr u32 shift_logical_right(const u32 v, const u32 shift) {
    return v >> (shift & 31U);
}

[[nodiscard]]
constexpr u32 shift_arithmetic_right(const u32 v, const u32 shift) {
    // signed bit should always be preserved, so casting works...
    static_assert(
        -16 >> 1U == -8,
        "arithemtic shift is not supported for this platform!"
    );

    return static_cast<u32>(static_cast<s32>(v) >> shift);
}

[[nodiscard]]
constexpr u32 shift_rotate_right(const u32 v, const u32 shift) {
    return bit::rotr(v, shift);
}

namespace barrel {

enum class type {
    lsl, lsr, asr, ror, rrx
};

struct [[nodiscard]] shift_result { u32 result; bool carry; };
template <type type>
constexpr auto shift(const u32 v, const u32 shift_v, const bool old_carry) -> shift_result {
    if constexpr(type == type::lsl) {
        switch (shift_v) {
            case 0: return { v, old_carry };
            case 1 ... 31: return { v << shift_v, bit::is_set(v, 32 - shift_v) };
            case 32: return { 0, bit::is_set<0>(v) };
            default: return { 0, false };
        }
    }
    if constexpr(type == type::lsr) {
        switch (shift_v) {
            case 0: return { v, old_carry };
            case 1 ... 31: return { v << shift_v, bit::is_set(v, 32 - shift_v) };
            case 32: return { 0, bit::is_set<31>(v) };
            default: return { 0, false };
        }
    }
    if constexpr(type == type::asr) {
        switch (shift_v) {
            case 0: return { v, old_carry };
            case 1 ... 31: return { shift_arithmetic_right(v, shift_v), bit::is_set(v, shift_v - 1) };
            default: return { shift_arithmetic_right(v, 31), bit::is_set<31>(v) };
        }
    }
    if constexpr(type == type::ror) {
        switch (shift_v) {
            case 0: return { v, old_carry };
            case 1 ... 31: return { bit::rotr(v, shift_v), bit::is_set(v, shift_v - 1) };
            case 32: return { v, bit::is_set(v, 31) };
            // todo: remove uneeded recursion
            default: return shift<type>(v, shift_v - 32, old_carry);// { bit::rotr(v, shift_v & 31), bit::is_set(v, (shift_v & 31) - 1) };
        }
    }
    // ror imm #0 is a special RRX shift, which is >> 1 with the carry being shifted in
    if constexpr(type == type::rrx) {
        assert(shift_v == 0 && "invalid shift param for rrx!");
        return { (v >> 1) | (old_carry << 31), bit::is_set(v, 0) };
    }
}

} // namespace barrel
} // namespace arm7tdmi

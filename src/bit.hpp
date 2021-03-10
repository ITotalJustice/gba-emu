#pragma once

#include <cstdint>
#include <cassert>

#ifdef __ARM_ACLE
#include <arm_acle.h>
#endif

namespace bit {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

constexpr u32 rotr(const u32 value, const u32 shift) {
// on arm, ror is an actual instruction, however i am not sure how to force
// gcc to generate a ror without using instrinsics.
// i don't imagine the arm_acle is avaliable on all platforms either.
// if compiling for arm AND the header isn't avaliable, then i should write inline asm
// for the instruction.
#ifdef __ARM_ACLE
    return __ror(value, shift & 31U);
#else
    return (value >> (shift & 31U)) | static_cast<u32>(((static_cast<u64>(value) << 32U) >> (shift & 31U)));
#endif
}

static_assert(
    rotr(0b1111'1111'1111'1111'1111'1111'1111'1111, 2) == 0b1111'1111'1111'1111'1111'1111'1111'1111 &&
    rotr(0b0011'1111'1111'1111'1111'1111'1111'1111, 2) == 0b1100'1111'1111'1111'1111'1111'1111'1111 &&
    // all 1's
    rotr(0b1111'1111'1111'1111'1111'1111'1111'1111, 69) == 0b1111'1111'1111'1111'1111'1111'1111'1111 &&
    // same as rotr(x, 2)
    rotr(0b0011'1111'1111'1111'1111'1111'1111'1111, 34) == 0b1100'1111'1111'1111'1111'1111'1111'1111,
    "bit::rotr is broken!"
);

// bit value should probably be either masked or, in debug mode,
// check if bit is ever >= 32, if so, throw.
template <typename T> [[nodiscard]]
constexpr bool is_set(const T value, const u32 bit) {
    assert(bit < (sizeof(T) * 8) && "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

// template <typename T> [[nodiscard]]
// constexpr T set(const T value, const u32 bit, const bool on) {
//     assert(bit < (sizeof(T) * 8) && "bit value out of bounds!");
//     return value | (on << bit);
// }

// compile-time bit-size checked checked alternitives
template <u8 bit, typename T> [[nodiscard]]
constexpr bool is_set(const T value) {
    static_assert(bit < (sizeof(T) * 8), "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

template <u8 bit, typename T>
constexpr T set(const T value, const bool on) {
    constexpr auto bit_width = sizeof(T) * 8;

    static_assert(bit < bit_width, "bit value out of bounds!");

    constexpr auto get_mask = [bit_width]() -> u64 {
        switch (bit_width) {
            case 8:  return 0xFF;
            case 16: return 0xFF'FF;
            case 32: return 0xFF'FF'FF'FF;
            case 64: return 0xFF'FF'FF'FF'FF'FF'FF'FF;
        }
    };

    // create a mask with all bits set apart from THE `bit`
    // this allows toggling the bit.
    // if on = true, the bit is set, else nothing is set
    constexpr auto mask = get_mask() & (~(1 << bit));

    return (value & mask) | (on << bit);
}

static_assert(
    bit::set<0, u8>(0b1100, true) == 0b1101 &&
    bit::set<3, u8>(0b1101, false) == 0b0101,
    "bit::set is broken!"
);

// CREDIT: thanks to peach teaching me asr carries down the sign bit!
// as of c++20, a >> b, where a is negative-signed, this will perform asr shift.
// prior to c++20 this was implementation defiened, so it could be lsr
// however gcc and clang both did asr anyway.
template <u8 start_size>
constexpr u32 sign_extend(const u32 value) {
    static_assert(start_size <= 31, "bit start size is out of bounds!");

    const u8 bits = 32 - start_size;
    return static_cast<u32>(static_cast<s32>(value << bits) >> bits);
}

static_assert(
    // simple 24-bit asr
    bit::sign_extend<24>(0b1100'1111'1111'1111'1111'1111) == 0b1111'1111'1100'1111'1111'1111'1111'1111 &&
    // set the sign-bit to bit 1, then asr 31-bits
    bit::sign_extend<1>(0b0001) == 0b1111'1111'1111'1111'1111'1111'1111'1111,
    "sign_extend is broken!"
);

} // namespace bit


#pragma once

#include <cstdint>
#include <cassert>

#ifdef __ARM_ACLE
#include <arm_acle.h>
#endif

namespace bit {

constexpr std::uint32_t rotr(const std::uint32_t value, const std::uint32_t shift) {
// on arm, ror is an actual instruction, however i am not sure how to force
// gcc to generate a ror without using instrinsics.
// i don't imagine the arm_acle is avaliable on all platforms either.
// if compiling for arm AND the header isn't avaliable, then i should write inline asm
// for the instruction.
#ifdef __ARM_ACLE
    return __ror(value, shift & 31U);
#else
    // both versions below are identicle asm on gcc, haven't yet tried other compilers
    // so leaving this here for now. 
    // return (value >> (shift & 31U)) | (((static_cast<uint64_t>(value) << 32U) >> (shift & 31U)) & 0xFFFFFFFF);
    return (value >> (shift & 31U)) | static_cast<std::uint32_t>(((static_cast<std::uint64_t>(value) << 32U) >> (shift & 31U)));
#endif
}

// bit value should probably be either masked or, in debug mode,
// check if bit is ever >= 32, if so, throw.
constexpr bool is_set(const std::uint32_t value, const std::uint32_t bit) {
    assert(bit <= 31 && "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

constexpr std::uint32_t set(const std::uint32_t value, const std::uint32_t bit) {
    assert(bit <= 31 && "bit value out of bounds!");
    return value | (1 <<bit);
}

// compile-time bit-size checked checked alternitives
template <std::uint8_t bit> [[nodiscard]]
constexpr bool is_set(const std::uint32_t value) {
    static_assert(bit <= 31, "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

template <std::uint8_t bit>
constexpr std::uint32_t set(const std::uint32_t value) {
    static_assert(bit <= 31, "bit value out of bounds!");
    return value | (1 << bit);
}

// CREDIT: peach (link GH here...)
// sign extend an arbitrary-size value to 32 bits
template <std::uint8_t start_size>
constexpr std::uint32_t sign_extend(const std::uint32_t value) {
    static_assert(start_size <= 31, "bit start size is out of bounds!");

    const std::int32_t temp = static_cast<std::int32_t>(value);
    const std::uint8_t bitsToShift = 32 - start_size;
    return static_cast<std::uint32_t>((temp << bitsToShift) >> bitsToShift);
}

static_assert(
    sign_extend<24>(0b1100'1111'1111'1111'1111'1111) == 0b1111'1111'1100'1111'1111'1111'1111'1111
);

} // namespace bit


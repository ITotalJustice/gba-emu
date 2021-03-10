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
    return (value >> (shift & 31U)) | static_cast<std::uint32_t>(((static_cast<std::uint64_t>(value) << 32U) >> (shift & 31U)));
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
constexpr bool is_set(const T value, const std::uint32_t bit) {
    assert(bit < (sizeof(T) * 8) && "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

template <typename T> [[nodiscard]]
constexpr T set(const T value, const std::uint32_t bit) {
    assert(bit < (sizeof(T) * 8) && "bit value out of bounds!");
    return value | (1 <<bit);
}

// compile-time bit-size checked checked alternitives
template <std::uint8_t bit, typename T> [[nodiscard]]
constexpr bool is_set(const T value) {
    static_assert(bit < (sizeof(T) * 8), "bit value out of bounds!");
    return (value & (1 << bit)) > 0;
}

template <std::uint8_t bit, typename T>
constexpr T set(const T value) {
    static_assert(bit < (sizeof(T) * 8), "bit value out of bounds!");
    return value | (1 << bit);
}

// CREDIT: thanks to peach teaching me asr carries down the sign bit!
// as of c++20, a >> b, where a is negative-signed, this will perform asr shift.
// prior to c++20 this was implementation defiened, so it could be lsr
// however gcc and clang both did asr anyway.
template <std::uint8_t start_size>
constexpr std::uint32_t sign_extend(const std::uint32_t value) {
    static_assert(start_size <= 31, "bit start size is out of bounds!");

    const std::uint8_t bits = 32 - start_size;
    return static_cast<std::uint32_t>(static_cast<std::int32_t>(value << bits) >> bits);
}

static_assert(
    // simple 24-bit asr
    bit::sign_extend<24>(0b1100'1111'1111'1111'1111'1111) == 0b1111'1111'1100'1111'1111'1111'1111'1111 &&
    // set the sign-bit to bit 1, then asr 31-bits
    bit::sign_extend<1>(0b0001) == 0b1111'1111'1111'1111'1111'1111'1111'1111,
    "sign_extend is broken!"
);

} // namespace bit


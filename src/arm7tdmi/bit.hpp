#pragma once

#include <stdint.h>

#ifdef __ARM_ACLE
#include <arm_acle.h>
#endif

namespace bit {

constexpr uint32_t rotr(const uint32_t value, const uint32_t shift) {
#ifdef __ARM_ACLE
    return __ror(value, shift);
#else
    // return (value >> (shift & 31U)) | (((static_cast<uint64_t>(value) << 32U) >> (shift & 31U)) & 0xFFFFFFFF);
    return (value >> (shift & 31U)) | static_cast<uint32_t>(((static_cast<uint64_t>(value) << 32U) >> (shift & 31U)));
#endif
}

} // namespace bit


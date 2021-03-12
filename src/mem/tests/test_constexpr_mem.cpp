// testing to see if it's possible to create a constexpr mem system
// for the gba.
// this likely requires a re-design of the arm struct,
// so no longer treating the arm7tdmi as a seperate chip that gets
// memory connected at runtime
// instead, pass a memory interface struct as a template param!
// this removes runtime errors / overhead and allows for constexpr stuff!

#include "../mmio.hpp"

#include <cstdint>
#include <cassert>
#include <array>
// #include <vector> // not constexpr yet in gcc or clang...
#include <span>
#include <algorithm>

namespace arm7tdmi {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

template <typename Mem>
struct Gba {
    
    Mem mem{};
};

constexpr auto compiletime_test() {
    constexpr auto func = []() {
        Gba<gba::mem::MMIO> Gba{};
        Gba.mem.write<u8>(Gba.mem.IWRAM_REGION_START, 69);
        const auto result = Gba.mem.read<u8>(Gba.mem.IWRAM_REGION_START);
        return result == 69;
    };

    static_assert(func());
}

} // namespace arm7tdmi

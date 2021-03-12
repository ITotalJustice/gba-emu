#include "../mmio.hpp"

namespace gba::mem {

constexpr auto compiletime_test() {
    constexpr auto func = []<typename T>(T value, T wanted) {
        gba::mem::MMIO mem{};
        
        mem.write<T>(mem.IWRAM_REGION_START, value);
        const auto result = mem.read<T>(mem.IWRAM_REGION_START);

        return result == wanted;
    };

    static_assert(
        func.template operator()<u8>(0x12, 0x12),
        "failed basic mem u8 read/write test!"
    );

    static_assert(
        func.template operator()<u16>(0x1234, 0x1234),
        "failed basic mem u16 read/write test!"
    );
    
    static_assert(
        func.template operator()<u32>(0x12'34'56'78, 0x12'34'56'78),
        "failed basic mem u32 read/write test!"
    );

    // tests if the correct number of bytes are masked off

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
    static_assert(
        func.template operator()<u8>(0x120, 0x120 & 0xFF),
        "failed basic mem u8 read/write test!"
    );

    static_assert(
        func.template operator()<u16>(0x123400, 0x123400 & 0xFF'FF),
        "failed basic mem u16 read/write test!"
    );
    
    static_assert(
        func.template operator()<u32>(0x12'34'56'7800, 0x12'34'56'7800 & 0xFF'FF'FF'FF),
        "failed basic mem u32 read/write test!"
    );
#pragma GCC diagnostic pop
}

} // namespace gba::mem

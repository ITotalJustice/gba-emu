#include "../mmio.hpp"

namespace gba::mem {

constexpr auto compiletime_test() {
    constexpr auto func = []<typename T>(T value) {
        gba::mem::MMIO mem{};
        
        mem.write<T>(mem.IWRAM_REGION_START, value);
        const auto result = mem.read<T>(mem.IWRAM_REGION_START);

        return result == value;
    };

    static_assert(
        func.template operator()<u8>(0x12),
        "failed basic mem u8 read/write test!"
    );

    static_assert(
        func.template operator()<u16>(0x1234),
        "failed basic mem u16 read/write test!"
    );
    
    static_assert(
        func.template operator()<u32>(0x12'34'56'78),
        "failed basic mem u32 read/write test!"
    );
}

} // namespace gba::mem

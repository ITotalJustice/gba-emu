#include "../thumb/load_store.hpp"

namespace arm7tdmi::thumb {

constexpr auto test_str_ldr() {
    constexpr auto func = []<typename T, sign_extend sign = sign_extend::no>(const u32 start_v, const u32 end_v, const u32 a, const u32 b, const u8 rd) {
        arm7tdmi arm{};
        arm.registers[rd] = start_v;

        instruction_str<T>(arm, a, b, rd);
        instruction_ldr<T, sign>(arm, a, b, rd);

        return end_v == arm.registers[rd];
    };

    constexpr auto offset = gba::mem::MMIO::IWRAM_REGION_START;

    static_assert(
        func.template operator()<u8, sign_extend::no>(0x10, 0x10, offset, 0x69, 5),
        "failed ldr-str u8 test!"
    );

    static_assert(
        func.template operator()<u16, sign_extend::no>(0xFFFF, 0xFFFF, offset, 0x69, 5),
        "failed ldr-str u16 test!"
    );

    static_assert(
        func.template operator()<u16, sign_extend::yes>(0xFFFF, 0xFF'FF'FF'FF, offset, 0x69, 5),
        "failed ldr-str u16 sign-extend test!"
    );

    static_assert(
        func.template operator()<u32, sign_extend::no>(0xFFFFFFFF, 0xFFFFFFFF, offset, 0x69, 5),
        "failed ldr-str u32 test!"
    );
}

} // namespace arm7tdmi::thumb

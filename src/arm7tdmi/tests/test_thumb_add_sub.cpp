#include "../thumb/add_sub.hpp"

namespace arm7tdmi::thumb {

constexpr auto test_add_sp() {
    constexpr auto func = [](const u32 start, const u32 want, const u8 offset) {
        arm7tdmi arm{};
        arm.set_sp(start);
        instruction_add_sp(arm, offset);
        return want == arm.get_sp();
    };

    static_assert(
        func(0x1000, 0x1000 + 2, 1),
        "failed add sp with non-signed number!"
    );

    static_assert(
        func(0x1000, 0x1000 - 2, -1),
        "failed add sp with signed number!"
    );
}

} // namespace arm7tdmi::thumb

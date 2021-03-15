#include "../thumb/alu_operations.hpp"

namespace arm7tdmi::thumb {

constexpr auto test_and() {
    constexpr auto func = []() {
        constexpr auto rs = 0;
        constexpr auto rd = 5;

        arm7tdmi arm{};
        arm.registers[rs] = 0x31;
        arm.registers[rd] = 0xFF;

        instruction_and(arm, rs, rd);

        return arm.registers[rd] == 0x31;
    };

    static_assert(func());
}

constexpr auto test_asr() {
    constexpr auto func = []() {
        constexpr auto rs = 0;
        constexpr auto rd = 5;

        arm7tdmi arm{};
        arm.registers[rs] = 258; // same as 2 (258 & 0xFF == 2)
        arm.registers[rd] = 0b1000'1111'1111'1111'1111'1111'1111'1100;

        instruction_asr(arm, rs, rd);

        return arm.registers[rd] == 0b1110'0011'1111'1111'1111'1111'1111'1111;
    };

    static_assert(func());
}

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

#include "../thumb/decode.hpp"

namespace arm7tdmi::thumb {

constexpr auto test_format3() {
    constexpr auto func = []() {
        constexpr std::array op_array{
            0b001'00'000'11111111, // mov r0, #255
            0b001'00'001'11111111, // mov r1, #255
            0b001'11'000'00001000, // sub r0, #8
            0b001'10'001'00001001, // add r1, #9
            0b001'01'000'11111111, // cmp r0, #255
        };

        arm7tdmi arm{};

        for (auto op : op_array) {
            thumb_decode_test(arm, op);
        }
        
        return (arm.registers[0] == (255 - 8)) &&
            (arm.registers[1] == (255 + 9)) &&
            arm.cpsr.get_flag<flags::N>() == true &&
            arm.cpsr.get_flag<flags::Z>() == false &&
            arm.cpsr.get_flag<flags::C>() == true &&
            arm.cpsr.get_flag<flags::V>() == true;
    };

    static_assert(
        func(),
        "failed thumb_test format3!"
    );
}

} // namespace arm7tdmi::thumb

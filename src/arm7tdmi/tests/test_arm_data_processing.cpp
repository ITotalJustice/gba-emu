#include "../arm/data_processing.hpp"

namespace arm7tdmi::arm {

static_assert(
    branch_offset(0b1100'1111'1111'1111'1111'1111) == (s32)0b1111'1111'0011'1111'1111'1111'1111'1100 &&
    branch_offset(0b0100'1111'1111'1111'1111'1111) == 0b0000'0001'0011'1111'1111'1111'1111'1100
);

constexpr void alu_logical_test() {
    struct Args {
        struct Value {
            u32 cpsr, op1, op2;
            bool carry;
        } value;
        struct Result {
            u32 result, cpsr;
        } result;
    };

    constexpr auto func_test = [](const auto func, const auto arr) -> bool {
        arm7tdmi arm{};
        for (const auto [value, result] : arr) {
            func(arm, value.op1, value.op2, 0, value.carry);
            if (arm.registers[0] != result.result || arm.get_cpsr() != result.cpsr) {
                return false;
            }
        }
        return true;
    };

    constexpr std::array<Args, 4> args{{
        {
            .value{0x00, 0x00, 0x00, true},
            .result{0x00, 0b0110'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0x80, 0x80, true},
            .result{0x80, 0b0010'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0xFFFFFFFF, 0xFFFFFFFF, true},
            .result{0xFF'FF'FF'FF, 0b1010'0000'0000'0000'0000'0000'0000'0000}
        },
        {
            .value{0x00, 0x00, 0x00, false},
            .result{0x00, 0b0100'0000'0000'0000'0000'0000'0000'0000}
        }
    }};

    static_assert(
        func_test(instruction_and<flags_cond::modify>, args),
        "failed to pass arm AND test!"
    );
}

} // namespace arm7tdmi::arm

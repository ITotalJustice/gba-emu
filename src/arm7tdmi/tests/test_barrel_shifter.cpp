#include "../barrel_shifter.hpp"

#include <array>

namespace arm7tdmi::barrel {

constexpr void shift_test() {
    struct Args {
        struct Value {
            u32 value, shift;
            bool old_carry;
        } value;
        shift_result result;
    };

    // pass in function to test and array of Args
    // return false if test fails at any point.
    constexpr auto test_func = [](const auto func, const auto array) {
        for (const auto [value, result] : array) {
            const auto r = func(value.value, value.shift, value.old_carry);
            if (r.result != result.result || r.carry != result.carry) {
                return false;
            }
        }
        return true;
    };

    {
        constexpr std::array<Args, 4> args{{
            {
                .value{ 0b0000'1000'0000'0000'0000'0000'0001'0000, 5, false },
                .result{ 0b0000'0000'0000'0000'0000'0010'0000'0000, true }
            },
            {
                .value{ 0, 0, true },
                .result{ 0, true }
            },
            {
                .value{ 0b0001, 32, false },
                .result{ 0, true }
            },
            {
                .value{ 0b1111'1111'1111'1111'1111'1111'1111'1111, 69, true },
                .result{ 0, false }
            },
        }};

        static_assert(test_func(shift<shift_type::lsl>, args), "failed lsl tests!");
    }

    {
        constexpr std::array<Args, 4> args{{
            {
                .value{ 0b1000'0000'0000'0000'0000'0000'0000'0000, 64, false },
                .result{ 0b1111'1111'1111'1111'1111'1111'1111'1111, true }
            },
            {
                .value{ 0b0000'0000'0000'0000'0000'0000'0000'0000, 64, false },
                .result{ 0b0000'0000'0000'0000'0000'0000'0000'0000, false }
            },
            {
                .value{ 0b1000'0000'0000'0000'0000'0000'0000'0000, 4, false },
                .result{ 0b1111'1000'0000'0000'0000'0000'0000'0000, false }
            },
            {
                .value{ 0b1000'0000'0000'0000'0000'0000'0001'0000, 5, false },
                .result{ 0b1111'1100'0000'0000'0000'0000'0000'0000, true }
            },
        }};

        static_assert(test_func(shift<shift_type::asr>, args), "failed asr tests!");
    }
}

} // arm7tdmi::barrel

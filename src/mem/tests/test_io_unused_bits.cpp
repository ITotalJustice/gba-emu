#include "../io_types.hpp"

namespace gba::mem {

constexpr auto test_unused_bits() {
    constexpr auto func = []<typename T>(auto value, auto want) {
        T io_reg{};
        io_reg.write(value);
        return io_reg.read() == want;
    };

    static_assert(
        func.template operator()<DISPCNT>(
            0b1111'1111'1111'1111,
            0b1111'1111'1111'1000
        ),
        "failed to pass DISPCNT"
    );

    static_assert(
        func.template operator()<DISPSTAT>(
            0b1111'1111'1111'1111,
            0b1111'1111'0011'1000
        ),
        "failed to pass DISPSTAT"
    );

    static_assert(
        func.template operator()<VCOUNT>(
            0b1111'1111'1111'1111,
            0b0000'0000'0000'0000
        ),
        "failed to pass VCOUNT"
    );

    static_assert(
        func.template operator()<BG0CNT>(
            0b1111'1111'1111'1111,
            0b1101'1111'1111'1111
        ),
        "failed to pass BG0CNT"
    );

    static_assert(
        func.template operator()<BG1CNT>(
            0b1111'1111'1111'1111,
            0b1101'1111'1111'1111
        ),
        "failed to pass BG1CNT"
    );

    static_assert(
        func.template operator()<BG2CNT>(
            0b1111'1111'1111'1111,
            0b1111'1111'1111'1111
        ),
        "failed to pass BG2CNT"
    );

    static_assert(
        func.template operator()<BG3CNT>(
            0b1111'1111'1111'1111,
            0b1111'1111'1111'1111
        ),
        "failed to pass BG3CNT"
    );

    // TODO: lot's more left to do...
}

} // namespace gba::mem

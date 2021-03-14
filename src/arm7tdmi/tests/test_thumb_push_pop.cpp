#include "../thumb/rlist.hpp"

namespace arm7tdmi::thumb {

constexpr auto test_push_pop() {
    constexpr auto func = []() {
        constexpr std::array<u32, 5> register_values{
            77, 33, 99, 80, 22
        };

        arm7tdmi arm{};
        arm.set_sp(gba::mem::MMIO::IWRAM_REGION_END - 0x10);

        // set reg values
        for (std::size_t i = 0; i < register_values.size(); ++i) {
            arm.registers[i] = register_values[i];
        }

        // push them
        instruction_push<push_pop_type::lr_0>(
            arm, 0b11111
        );

        // reset registers
        for (std::size_t i = 0; i < register_values.size(); ++i) {
            arm.registers[i] = 0;
        }

        // pop them
        instruction_pop<push_pop_type::lr_0>(
            arm, 0b11111
        );

        // check that they were popped in the same order (and match)
        for (std::size_t i = 0; i < register_values.size(); ++i) {
            if (arm.registers[i] != register_values[i]) {
                return false;
            }
        }

        return true;
    };

    static_assert(func());
}

} // namespace arm7tdmi::thumb

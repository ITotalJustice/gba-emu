#include "../arm7tdmi.hpp"

namespace arm7tdmi {

constexpr auto testing() {
    constexpr auto func = []() {
        std::array<u32, 7> array{ 1, 69, 45, 99, 3, 2, 5 };

        const auto fill_func = [&array](auto& dst_array) {
            for (size_t i = 0; i < dst_array.size(); ++i) {
                dst_array[i] = array[i];
            }
        };

        arm7tdmi arm{};

        fill_func(arm.banked_user);
        fill_func(arm.banked_fiq);
        fill_func(arm.banked_irq);
        fill_func(arm.banked_supervisor);
        fill_func(arm.banked_abort);
        fill_func(arm.banked_undefined);

        arm.cpsr.set_mode(mode::USER);
        arm.switch_mode(mode::FIQ);


        for (size_t i = 0; i < array.size(); ++i) {
            if (arm.registers[8 + i] != array[i]) {
                return false;
            }
        }

        arm.switch_mode(mode::USER);
        
        for (size_t i = 0; i < array.size(); ++i) {
            if (arm.registers[8 + i] != 0) {
                return false;
            }
        }

        return true;
    };

    static_assert(func(), "failed cpsr test!");
}

} // namespace arm7tdmi

#pragma once

#include "../arm7tdmi.hpp"

#include <bit>
#include <cassert>

namespace arm7tdmi::thumb {

enum class push_pop_type {
    lr_0, // todo: name these better
    lr_1,
};

constexpr auto rlist_callback(arm7tdmi& arm, const auto Callback, u8 rlist) {
    assert(rlist <= 0b1111'1111 && "invalid rlist!");
    
    while (rlist != 0) {
        const auto reg_index = std::countr_zero(rlist);
        Callback(arm, reg_index);
        rlist = bit::unset(rlist, reg_index);
    }
}

// NOTE: this code is likely wrong!

template <push_pop_type type>
constexpr auto instruction_push(arm7tdmi& arm, const u8 rlist) {
    constexpr auto push_callback = [](arm7tdmi& arm_param, u8 reg_index) {
        arm_param.mmio.write<u32>(
            arm_param.registers[arm_param.REGISTER_SP_INDEX],
            arm_param.registers[reg_index]
        );
        arm_param.registers[arm_param.REGISTER_SP_INDEX] += sizeof(u32);
    };
    
    if constexpr(type == push_pop_type::lr_0) {
        // edge case!
        if (rlist == 0) {
            // store the PC, then offset the SP by -64
            arm.registers[arm.REGISTER_SP_INDEX] -= 64;
            push_callback(arm, arm.REGISTER_PC_INDEX);
        } else {
            const auto addr = std::popcount(rlist) << 2;
            arm.registers[arm.REGISTER_SP_INDEX] -= addr;
            rlist_callback(arm, push_callback, rlist);
            arm.registers[arm.REGISTER_SP_INDEX] -= addr;
        }
    } else {
        const auto addr = (std::popcount(rlist) << 2) + sizeof(u32);
        arm.registers[arm.REGISTER_SP_INDEX] -= addr;
        
        rlist_callback(arm, push_callback, rlist);
        push_callback(arm, arm.REGISTER_LR_INDEX);

        arm.registers[arm.REGISTER_SP_INDEX] -= addr;
    }
}

template <push_pop_type type>
constexpr auto instruction_pop(arm7tdmi& arm, const u8 rlist) {
    constexpr auto pop_callback = [](arm7tdmi& arm_param, u8 reg_index) {
        arm_param.registers[reg_index] = arm_param.mmio.read<u32>(
            arm_param.registers[arm_param.REGISTER_SP_INDEX]
        );
        arm_param.registers[arm_param.REGISTER_SP_INDEX] += sizeof(u32);
    };
    
    if constexpr(type == push_pop_type::lr_0) {
        // edge case!
        if (rlist == 0) {
            // load the PC, then offset the SP by +64
            pop_callback(arm, arm.REGISTER_PC_INDEX);
            arm.registers[arm.REGISTER_SP_INDEX] += 64;
        } else {
            rlist_callback(arm, pop_callback, rlist);
        }
    } else {
        rlist_callback(arm, pop_callback, rlist);
        pop_callback(arm, arm.REGISTER_PC_INDEX);
    }
}

} // namespace arm7tdmi::thumb

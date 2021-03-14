#pragma once

#include "../arm7tdmi.hpp"

#include <bit>
#include <cassert>

namespace arm7tdmi::thumb {

enum class push_pop_type {
    lr_0, // todo: name these better
    lr_1,
};

constexpr auto rlist_callback(arm7tdmi& arm, const auto Callback, u32 addr, u32 rlist) {  
    while (rlist != 0) {
        const auto reg_index = std::countr_zero(rlist);
        Callback(arm, addr, reg_index);
        rlist = bit::unset(rlist, reg_index);
    }

    return addr;
}

constexpr auto push_callback(arm7tdmi& arm, u32& addr, const u32 reg_index) {
    arm.mmio.write<u32>(
        addr,
        arm.registers[reg_index]
    );
    addr += sizeof(u32);
};

constexpr auto pop_callback(arm7tdmi& arm, u32& addr, u8 reg_index) {
    arm.registers[reg_index] = arm.mmio.read<u32>(
        addr
    );
    addr += sizeof(u32);
};

template <push_pop_type type>
constexpr auto instruction_push(arm7tdmi& arm, const u32 rlist) {
    assert(rlist <= 0b1111'1111 && "invalid rlist!");
    
    if constexpr(type == push_pop_type::lr_0) {
        // edge case!
        if (rlist == 0) {
            assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
        } else {
            const auto addr = arm.get_sp() - (std::popcount(rlist) << 2);
            rlist_callback(arm, push_callback, addr, rlist);
            arm.set_sp(addr);
        }
    } else {
        const auto new_rlist = rlist | 0b0111'1111'1111'1111;
        const auto addr = arm.get_sp() - (std::popcount(new_rlist) << 2);
        
        rlist_callback(arm, push_callback, addr, new_rlist);
        arm.set_sp(addr);
    }
}

template <push_pop_type type>
constexpr auto instruction_pop(arm7tdmi& arm, const u8 rlist) {  
    if constexpr(type == push_pop_type::lr_0) {
        // edge case!
        if (rlist == 0) {
            assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
        } else {
            const auto addr = arm.get_sp();
            const auto new_addr = rlist_callback(arm, pop_callback, addr, rlist);
            arm.set_sp(new_addr);
        }
    } else {
        const auto new_rlist = rlist | 0b1111'1111'1111'1111;
        const auto addr = arm.get_sp();

        const auto new_addr = rlist_callback(arm, pop_callback, addr, rlist);
        arm.set_sp(new_addr);
    }
}

constexpr auto instruction_stmia(arm7tdmi& arm, const u8 rb, const u8 rlist) {
    // edge case!
    if (rlist == 0) {
        assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
    } else {
        const auto addr = arm.registers[rb] - (std::popcount(rlist) << 2);
        rlist_callback(arm, push_callback, addr, rlist);
        arm.registers[rb] = addr;
    }
}

constexpr auto instruction_ldmia(arm7tdmi& arm, const u8 rb, const u8 rlist) {
    // edge case!
    if (rlist == 0) {
        assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
    } else {
        const auto addr = arm.registers[rb];
        const auto new_addr = rlist_callback(arm, pop_callback, addr, rlist);
        arm.registers[rb] =  new_addr;
    }
}

} // namespace arm7tdmi::thumb

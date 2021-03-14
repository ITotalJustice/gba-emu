#pragma once

#include "../arm7tdmi.hpp"

#include <bit>
#include <cassert>

namespace arm7tdmi::thumb {

// todo: name these better
enum class push_pop_type {
    lr_0, lr_1
};

enum class rlist_type {
    store, load
};

// todo: clean this up, it's a little too over complicated atm
template <rlist_type type>
constexpr auto rlist_callback(arm7tdmi& arm, u32 addr, u32 rlist) {  
    if constexpr(type == rlist_type::store) {
        // the lowest reg is pushed to the lowest addr
        // however it is still pushed first.
        // because of this, calc the final offset and then write to it
        // whilst incrementing the addr each time.
        const auto final_addr = addr - (std::popcount(rlist) << 2);
        addr = final_addr;

        while (rlist != 0) {
            const auto reg_index = std::countr_zero(rlist);
            arm.mmio.write<u32>(
                addr, arm.registers[reg_index]
            );
            addr += sizeof(u32);
            rlist = bit::unset(rlist, reg_index);
        }

        return final_addr;
    
    } else {
        while (rlist != 0) {
            const auto reg_index = std::countr_zero(rlist);
            arm.registers[reg_index] = arm.mmio.read<u32>(
                addr
            );
            addr += sizeof(u32);
            rlist = bit::unset(rlist, reg_index);
        }

        return addr;
    }
}

template <push_pop_type type>
constexpr auto instruction_push(arm7tdmi& arm, const u32 rlist) {
    assert(rlist <= 0b1111'1111 && "invalid rlist!");
    
    if constexpr(type == push_pop_type::lr_0) {
        // edge case!
        if (rlist == 0) {
            assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
        } else {
            const auto addr = rlist_callback<rlist_type::store>(
                arm,
                arm.get_sp(),
                rlist
            );
            arm.set_sp(addr);
        }
    } else {
        const auto addr = rlist_callback<rlist_type::store>(
            arm, 
            arm.get_sp(),
            rlist | 0b0100'0000'0000'0000
        );
        
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
            const auto addr = rlist_callback<rlist_type::load>(
                arm,
                arm.get_sp(),
                rlist
            );
            arm.set_sp(addr);
        }
    } else {
        const auto addr = rlist_callback<rlist_type::load>(
            arm,
            arm.get_sp(),
            rlist | 0b1000'0000'0000'0000
        );
        arm.set_sp(addr);
    }
}

constexpr auto instruction_stmia(arm7tdmi& arm, const u8 rb, const u8 rlist) {
    // edge case!
    if (rlist == 0) {
        assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
    } else {
        const auto addr = rlist_callback<rlist_type::store>(
            arm,
            arm.registers[rb],
            rlist
        );
        arm.registers[rb] = addr;
    }
}

constexpr auto instruction_ldmia(arm7tdmi& arm, const u8 rb, const u8 rlist) {
    // edge case!
    if (rlist == 0) {
        assert(rlist != 0 && "not handled rlist == 0 edge case yet!");
    } else {
        const auto addr = rlist_callback<rlist_type::load>(
            arm,
            arm.registers[rb],
            rlist
        );
        arm.registers[rb] = addr;
    }
}

} // namespace arm7tdmi::thumb

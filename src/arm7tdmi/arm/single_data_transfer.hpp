#pragma once

#include "../arm7tdmi.hpp"

#include <cassert>

namespace arm7tdmi::arm {

// [4.9]
/*
The offset from the base may be either a 12 bit unsigned binary immediate value in the 
instruction, or a second register (possibly shifted in some way).

The offset may be added to (U=1) or subtracted from (U=0) the base register Rn. The offset modification 
may be performed either before (pre-indexed, P=1) or after (post-indexed, P=0) the 
base is used as the transfer address.

The W bit gives optional auto increment and decrement addressing modes. The 
modified base value may be written back into the base (W=1), or the old base value 
may be kept (W=0).

In the case of post-indexed addressing, the write back bit is 
redundant and must be set to zero, since the old base value can be retained by setting 
the offset to zero. Therefore post-indexed data transfers always write back the modified 
base.

The only use of the W bit in a post-indexed data transfer is in privileged mode 
code, where setting the W bit forces non-privileged mode for the transfer, allowing the 
operating system to generate a user address in a system where the memory 
management hardware makes suitable use of this hardware.
*/

// P | U | B | W
template <u8 P, u8 U, typename B, u8 W, u8 str, typename T>
constexpr void instruction_str_ldr(arm7tdmi& arm, const T offset, const u8 rn, const u8 rd) {
    auto addr = arm.registers[rn];

    // pre addr modify
    if constexpr(P == 1) {
        if constexpr(U == 0) { // sub
            addr -= offset;
        } else { // add
            addr += offset;
        }
    }

    // store
    if constexpr(str == 1) {
        assert(0 && 
            "ldr does some strange offset things that i havent added yet" &&
            "the addr needs to be aligned but then shifts the result bits around");
        arm.mmio.write<B>(addr, arm.registers[rd]);
    } else { // load
        arm.registers[rd] = arm.mmio.read<B>(addr);
    }

    // post addr modify
    if constexpr(P == 0) {
        if constexpr(U == 0) { // sub
            addr -= offset;
        } else { // add
            addr += offset;
        }
    }

    // write back the value
    // p is post increment, thus W is overriden
    if constexpr(P == 0 || W == 1) {
        arm.registers[rn] = addr;
    }
}

} // arm7tdmi::arm

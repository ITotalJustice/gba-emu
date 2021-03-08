#pragma once

#include <stdint.h>

// NOTES:

// 32-bit cpu
// 37 32-bit registers, 32 general purpose, 6 status
// 2 instructions sets
//  - arm (32-bit)
//  - thumb (16-bit, uses 32-bit registers still)
// there's something about banked registers which i don't understand...

// 3.7.3
// thumb register set:
// r0-r7
// ...
// r13: SP
// r14: LR
// r15: PC
// r16: CPSR
// r17+?: SPSR (saved program status reg)

// arm register set:
// r0-r7: lo registers
// r8-r15: hi registers (same sp, lr, pc as thumb above)
// r16: CPSR
// r17+?: SPSR (saved program status reg)

// 3.7.4
// hi regs can still be used in thumb with special mov instructions
// and can be compared or added.

// 3.8
typedef enum arm7tdmi_mode {
    MODE_USER = 0b10000,
    MODE_FIQ = 0b10001,
    MODE_IRQ = 0b10010,
    MODE_SUPERVISOR = 0b10011,
    MODE_ABORT = 0b10111,
    MODE_UNDEFINED = 0b11011,
    MODE_SYSTEM = 0b11111,
} arm7tdmi_mode_t;

// 3.9.9
typedef enum arm7tdmi_exception_vectors {
    EXCEPTION_RESET = 0x00000000,
    EXCEPTION_UNDEFINED_INSTRUCTION = 0x00000004,
    EXCEPTION_SOFTWARE_INTERRUPT = 0x00000008,
    EXCEPTION_ABORT_PREFETCH = 0x0000000C,
    EXCEPTION_ABORT_DATA = 0x00000010,
    EXCEPTION_RESERVED = 0x00000014,
    EXCEPTION_IRQ = 0x00000018,
    EXCEPTION_FIQ = 0x0000001C,
} arm7tdmi_exception_vectors_t;

typedef enum arm7tdmi_condition {
    CONDITION_EQ = 0b0000, // Z set
    CONDITION_NE = 0b0001, // Z clear
    CONDITION_CS = 0b0010, // C set
    CONDITION_CC = 0b0011, // C clear
    CONDITION_MI = 0b0100, // N set
    CONDITION_PL = 0b0101, // N clear
    CONDITION_VS = 0b0110, // V set
    CONDITION_VC = 0b0111, // V clear
    CONDITION_HI = 0b1000, // C set and Z clear
    CONDITION_LS = 0b1001, // C clear or Z set
    CONDITION_GE = 0b1010, // N equals V
    CONDITION_LT = 0b1011, // N not equal to V
    CONDITION_GT = 0b1100, // Z clear and N equals V
    CONDITION_LE = 0b1101, // Z set or N not equal to V
    CONDITION_AL = 0b1110, // ignored (AL = always)
    CONDITION_RESERVED = 0b1111, // not used
} arm7tdmi_condition_t;

// exception priority:
// - EXCEPTION_RESET
// - EXCEPTION_ABORT_DATA
// - EXCEPTION_FIQ
// - EXCEPTION_IRQ
// - EXCEPTION_ABORT_PREFETCH
// - EXCEPTION_UNDEFINED_INSTRUCTION | EXCEPTION_SOFTWARE_INTERRUPT

// theres 1 cpsr and 5 spsr
typedef union arm7tdmi_psr {
    struct {
        // condition flags
        uint32_t N : 1; // negative, less than
        uint32_t Z : 1; // zero
        uint32_t C : 1; // carry, borrow, extend
        uint32_t V : 1; // overflow
        
        // reserved
        uint32_t reserved : 20;
        
        // control
        uint32_t I : 1; // IRQ disable (1=off,0=on)
        uint32_t F : 1; // FIQ disable (1=off,0=on)
        uint32_t T : 1; // state bit (1=thumb,0=thumb)
        uint32_t M : 5; // mode
    };
    uint32_t word;
} arm7tdmi_psr_t;

// in arm state, 16 gReg are visible and 1-2 sReg
typedef struct arm7tdmi {
    // r14 - sub link reg, gets set to r15 during branch and links
    // r15 - pc, bits 0-1 are zero in arm state, bit-0 is zero in thumb
    // r16 - cpsr (current program status reg).
    // hoping these can indexed as an arrary...
    uint32_t registers[31];
    
    // these should be union / struct bitfields for each flag bit
    uint32_t status_registers[6];
} arm7tdmi_t;

void arm_decode_test(uint32_t op);
void thumb_decode_test(uint16_t op);

#pragma once

#include "../bit.hpp"

#include <cstdint>
#include <array>
#include <cassert>

namespace arm7tdmi {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

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

enum class flags {
    N, // negative
    Z, // zero
    C, // carry
    V, // overflow
};

enum class state : bool {
    ARM = 0,
    THUMB = 1
};

// 3.8
enum class mode {
    USER = 0b10000,
    FIQ = 0b10001,
    IRQ = 0b10010,
    SUPERVISOR = 0b10011,
    ABORT = 0b10111,
    UNDEFINED = 0b11011,
    SYSTEM = 0b11111,
};

// 3.9.9
enum class exception_vectors : u32 {
    RESET = 0x00000000,
    UNDEFINED_INSTRUCTION = 0x00000004,
    SOFTWARE_INTERRUPT = 0x00000008,
    ABORT_PREFETCH = 0x0000000C,
    ABORT_DATA = 0x00000010,
    RESERVED = 0x00000014,
    IRQ = 0x00000018,
    FIQ = 0x0000001C,
};

enum class condition {
    EQ = 0b0000, // Z set
    NE = 0b0001, // Z clear
    CS = 0b0010, // C set
    CC = 0b0011, // C clear
    MI = 0b0100, // N set
    PL = 0b0101, // N clear
    VS = 0b0110, // V set
    VC = 0b0111, // V clear
    HI = 0b1000, // C set and Z clear
    LS = 0b1001, // C clear or Z set
    GE = 0b1010, // N equals V
    LT = 0b1011, // N not equal to V
    GT = 0b1100, // Z clear and N equals V
    LE = 0b1101, // Z set or N not equal to V
    AL = 0b1110, // ignored (AL = always)
};

// exception priority:
// - EXCEPTION_RESET
// - EXCEPTION_ABORT_DATA
// - EXCEPTION_FIQ
// - EXCEPTION_IRQ
// - EXCEPTION_ABORT_PREFETCH
// - EXCEPTION_UNDEFINED_INSTRUCTION | EXCEPTION_SOFTWARE_INTERRUPT

constexpr u32 switch_state(const uint32_t cpsr, const state new_state) {
    return (cpsr & 0b1111'1111'1111'1111'1111'1111'1101'1111) | (static_cast<u32>(new_state) << 5);
}

constexpr auto get_state(const u32 cpsr) {
    return static_cast<state>(bit::is_set<5>(cpsr));
}

constexpr bool is_state(const u32 cpsr, const state wanted) {
    return get_state(cpsr) == wanted;
}

constexpr auto get_mode(const u32 cpsr) {
    return static_cast<mode>(cpsr & 0x1F);
}

constexpr bool is_mode(const u32 cpsr, const mode wanted) {
    return get_mode(cpsr) == wanted;
}

template <flags flag>
constexpr bool is_flag_set(const u32 cpsr) {
    if constexpr (flag == flags::N) {
        return bit::is_set<31>(cpsr);
    }
    if constexpr (flag == flags::Z) {
        return bit::is_set<30>(cpsr);
    }
    if constexpr (flag == flags::C) {
        return bit::is_set<29>(cpsr);
    }
    if constexpr (flag == flags::V) {
        return bit::is_set<28>(cpsr);
    }
}

constexpr auto FLAG_N_MASK = 0b0111'1111'1111'1111'1111'1111'1111'1111;
constexpr auto FLAG_Z_MASK = 0b1011'1111'1111'1111'1111'1111'1111'1111;
constexpr auto FLAG_C_MASK = 0b1101'1111'1111'1111'1111'1111'1111'1111;
constexpr auto FLAG_V_MASK = 0b1110'1111'1111'1111'1111'1111'1111'1111;

constexpr auto FLAG_N_BIT = 31;
constexpr auto FLAG_Z_BIT = 30;
constexpr auto FLAG_C_BIT = 29;
constexpr auto FLAG_V_BIT = 28;

template <flags flag>
constexpr u32 set_flag(const u32 cpsr, const bool value) {
    if constexpr (flag == flags::N) {
        return bit::set<FLAG_N_BIT>(cpsr, value);
    }
    if constexpr (flag == flags::Z) {
        return bit::set<FLAG_Z_BIT>(cpsr, value);
    }
    if constexpr (flag == flags::C) {
        return bit::set<FLAG_C_BIT>(cpsr, value);
    }
    if constexpr (flag == flags::V) {
        return bit::set<FLAG_V_BIT>(cpsr, value);
    }
}

// cond should already be a shifted down opcode, ie, opcode >> 28
constexpr bool test_cond(const u32 cpsr, const u8 cond) {
    assert(cond < 0x10 && "invalid cond value!");

    // TODO: use `using enum` once i have a gcc version that supports it!

    switch (static_cast<condition>(cond)) {
        case condition::EQ: return is_flag_set<flags::Z>(cpsr);
        case condition::NE: return !is_flag_set<flags::Z>(cpsr);
        case condition::CS: return is_flag_set<flags::C>(cpsr);
        case condition::CC: return !is_flag_set<flags::C>(cpsr);
        case condition::MI: return is_flag_set<flags::N>(cpsr);
        case condition::PL: return !is_flag_set<flags::N>(cpsr);
        case condition::VS: return is_flag_set<flags::V>(cpsr);
        case condition::VC: return !is_flag_set<flags::V>(cpsr);
        case condition::HI: return is_flag_set<flags::C>(cpsr) && !is_flag_set<flags::Z>(cpsr);
        case condition::LS: return !is_flag_set<flags::C>(cpsr) || is_flag_set<flags::Z>(cpsr);
        case condition::GE: return is_flag_set<flags::N>(cpsr) == is_flag_set<flags::V>(cpsr);
        case condition::LT: return is_flag_set<flags::N>(cpsr) != is_flag_set<flags::V>(cpsr);
        case condition::GT: return !is_flag_set<flags::Z>(cpsr) && (is_flag_set<flags::N>(cpsr) == is_flag_set<flags::V>(cpsr));
        case condition::LE: return is_flag_set<flags::Z>(cpsr) || (is_flag_set<flags::N>(cpsr) != is_flag_set<flags::V>(cpsr));
        case condition::AL: return true;
    }

    __builtin_unreachable();
}

// i'm unsure if i should:
// - return the result and flags in struct (pure function)
// - pass result by ref but update the flags inside the function
// - pass result and flags all by ref (easier to test)
// - pass rd value which is index into reg array, update flags in function
// all approaches are valid, and i do like the idea of making as many
// functions pure as possible due to ease of testing and no side effects.
// but then it makes the call-site more messy, as it's up to the caller
// to then set the returned flags, which i think is a harder bug to catch...

// TODO: rename this!
enum class ftest {
    v,
    c,
    z,
    n,
    nz,
    nzc,
    nzcv,
};

// TODO: check asm output from this...
// i'm hoping that it does a single mask of cpsr then shifts values and or's them
// rather than multiple masks / shifts.
// std::array should also be optimised away
// TODO: try and remove std::array

template <ftest flag>
#if 1
constexpr u32 set_flags(const u32 cpsr, const bool p1 = false, const bool p2 = false, const bool p3 = false, const bool p4 = false) {
    if constexpr(flag == ftest::v) {
        return (cpsr & FLAG_V_MASK) | (p1 << FLAG_V_BIT);
    }
    if constexpr(flag == ftest::c) {
        return (cpsr & FLAG_C_MASK) | (p1 << FLAG_C_BIT);
    }
    if constexpr(flag == ftest::z) {
        return (cpsr & FLAG_Z_MASK) | (p1 << FLAG_Z_BIT);
    }
    if constexpr(flag == ftest::n) {
        return (cpsr & FLAG_N_MASK) | (p1 << FLAG_N_BIT);
    }
    if constexpr(flag == ftest::nz) {
        return (cpsr & 0b0011'1111'1111'1111'1111'1111'1111'1111) | (p1 << FLAG_N_BIT) | (p2 << FLAG_Z_BIT);
    }
    if constexpr(flag == ftest::nzc) {
        return (cpsr & 0b0001'1111'1111'1111'1111'1111'1111'1111) | (p1 << FLAG_N_BIT) | (p2 << FLAG_Z_BIT) | (p3 << FLAG_C_BIT);
    }
    if constexpr(flag == ftest::nzcv) {
        return (cpsr & 0b0000'1111'1111'1111'1111'1111'1111'1111) | (p1 << FLAG_N_BIT) | (p2 << FLAG_Z_BIT) | (p3 << FLAG_C_BIT) | (p4 << FLAG_V_BIT);
    }
#else
constexpr u32 set_flags(const u32 cpsr, const auto ...args) {
    const std::array list{args...};

    if constexpr(flag == ftest::v) {
        static_assert(sizeof...(args) == 1);
        return set_flag<flags::V>(cpsr, list[0]);
    }
    if constexpr(flag == ftest::c) {
        static_assert(sizeof...(args) == 1);
        return set_flag<flags::C>(cpsr, list[0]);
    }
    if constexpr(flag == ftest::z) {
        static_assert(sizeof...(args) == 1);
        return set_flag<flags::Z>(cpsr, list[0]);
    }
    if constexpr(flag == ftest::n) {
        static_assert(sizeof...(args) == 1);
        return set_flag<flags::N>(cpsr, list[0]);
    }
    if constexpr(flag == ftest::nz) {
        static_assert(sizeof...(args) == 2);
        return set_flag<flags::N>(set_flag<flags::Z>(cpsr, list[1]), list[0]);
    }
    if constexpr(flag == ftest::nzc) {
        static_assert(sizeof...(args) == 3);
        return set_flag<flags::N>(set_flag<flags::Z>(set_flag<flags::C>(cpsr, list[2]), list[1]), list[0]);
    }
    if constexpr(flag == ftest::nzcv) {
        static_assert(sizeof...(args) == 4);
        return set_flag<flags::N>(set_flag<flags::Z>(set_flag<flags::C>(set_flag<flags::V>(cpsr, list[3]), list[2]), list[1]), list[0]);
    }
#endif
}

// theres 1 cpsr and 5 spsr
union arm7tdmi_psr {
    struct {
        // condition flags
        u32 N : 1; // negative, less than
        u32 Z : 1; // zero
        u32 C : 1; // carry, borrow, extend
        u32 V : 1; // overflow
        
        // reserved
        u32 reserved : 20;
        
        // control
        u32 I : 1; // IRQ disable (1=off,0=on)
        u32 F : 1; // FIQ disable (1=off,0=on)
        u32 T : 1; // state bit (1=thumb,0=thumb)
        u32 M : 5; // mode
    };
    u32 word;
};

struct psr {
    u32 value;

    // [[nodiscard]]
    // constexpr operator auto() const noexcept {
    //     return this->value;
    // }

    // constexpr auto operator=(const u32 v) noexcept {
    //     return this->value = v;
    // }

    constexpr auto set_state(const state new_state) noexcept {
        this->value = (this->value & 0b1111'1111'1111'1111'1111'1111'1101'1111) | (static_cast<u32>(new_state) << 5);
    }

    [[nodiscard]]
    constexpr auto get_state() const noexcept {
        return static_cast<state>(bit::is_set<5>(this->value));
    }

    constexpr auto is_state(const state wanted) const noexcept {
        return this->get_state() == wanted;
    }

    [[nodiscard]]
    constexpr auto get_mode() const noexcept {
        return static_cast<mode>(this->value & 0x1F);
    }

    constexpr auto set_mode(const mode new_mode) noexcept {
        this->value = (this->value & 0b1111'1111'1111'1111'1111'1111'1110'0000) | static_cast<u32>(new_mode);
    }

    [[nodiscard]]
    constexpr auto is_mode(const mode wanted) const noexcept {
        return this->get_mode() == wanted;
    }

    template <flags flag> [[nodiscard]]
    constexpr auto get_flag() const noexcept {
        return is_flag_set<flag>(this->value);
    }

    template <flags flag>
    constexpr auto set_flag(const bool on) noexcept {
        set_flag<flag>(this->value, on);
    }
};

// in arm state, 16 gReg are visible and 1-2 sReg
struct arm7tdmi {
    static constexpr auto REGISTER_SP_INDEX = 13;
    static constexpr auto REGISTER_LR_INDEX = 14;
    static constexpr auto REGISTER_PC_INDEX = 15;

    // r14 - sub link reg, gets set to r15 during branch and links
    // r15 - pc, bits 0-1 are zero in arm state, bit-0 is zero in thumb
    // r16 - cpsr (current program status reg).
    std::array<u32, 16> registers;
    
    psr cpsr;
    psr spsr;

    std::array<u32, 7> banked_user;
    // psr spsr_user;
    std::array<u32, 7> banked_fiq;
    psr spsr_fiq;
    std::array<u32, 2> banked_supervisor;
    psr spsr_supervisor;
    std::array<u32, 2> banked_abort;
    psr spsr_abort;
    std::array<u32, 2> banked_irq;
    psr spsr_irq;
    std::array<u32, 2> banked_undefined;
    psr spsr_undefined;

    bool barrel_carry : 1;

    [[nodiscard]]
    constexpr auto get_sp() const noexcept {
        return this->registers[REGISTER_SP_INDEX];
    }

    constexpr auto set_sp(const u16 v) noexcept {
        this->registers[REGISTER_SP_INDEX] = v;
    }

    [[nodiscard]]
    constexpr auto get_lr() const noexcept {
        return this->registers[REGISTER_LR_INDEX];
    }

    constexpr auto set_lr(const u16 v) noexcept {
        this->registers[REGISTER_LR_INDEX] = v;
    }

    [[nodiscard]]
    constexpr auto get_pc() const noexcept {
        return this->registers[REGISTER_PC_INDEX];
    }

    constexpr auto set_pc(const u16 v) noexcept {
        this->registers[REGISTER_PC_INDEX] = v;
    }

    [[nodiscard]]
    constexpr auto get_cpsr() const noexcept {
        return this->cpsr.value;
    }

    constexpr auto set_cpsr(const u16 v) noexcept {
        this->cpsr.value = v;
    }

    constexpr auto switch_mode(const mode new_mode) noexcept {
        const auto current_mode = this->cpsr.get_mode();
        // if the modes are the same, don't switch...
        if (new_mode == current_mode) {
            return;
        }

        const auto store_func = [new_mode, this](auto& dst) {
            if (new_mode == mode::FIQ) {
                for (size_t i = 0; i < 5; ++i) {
                    this->banked_user[i] = this->registers[8 + i];
                }
            }

            const auto offset = 15 - dst.size();
            for (size_t i = 0; i < dst.size(); ++i) {
                dst[i] = this->registers[offset + i];
            }
        };

        const auto load_func = [this](const auto& src) {
            const auto offset = 15 - src.size();
            for (size_t i = 0; i < src.size(); ++i) {
                this->registers[offset + i] = src[i];
            }
        };

        // store the registers in banks
        switch (current_mode) {
            case mode::USER: case mode::SYSTEM:
                store_func(this->banked_user);
                // this->spsr_user = this->spsr;
                break;

            case mode::FIQ:
                store_func(this->banked_fiq);
                this->spsr_fiq = this->spsr;
                break;

            case mode::IRQ:
                store_func(this->banked_irq);
                this->spsr_irq = this->spsr;
                break;

            case mode::SUPERVISOR:
                store_func(this->banked_supervisor);
                this->spsr_supervisor = this->spsr;
                break;

            case mode::ABORT:
                store_func(this->banked_abort);
                this->spsr_abort = this->spsr;
                break;

            case mode::UNDEFINED:
                store_func(this->banked_undefined);
                this->spsr_undefined = this->spsr;
                break;
        }

        // load the registers from banks
        switch (new_mode) {
            case mode::USER: case mode::SYSTEM:
                load_func(this->banked_user);
                // this->spsr = this->spsr_user;
                break;

            case mode::FIQ:
                load_func(this->banked_fiq);
                this->spsr = this->spsr_fiq;
                break;

            case mode::IRQ:
                load_func(this->banked_irq);
                this->spsr = this->spsr_irq;
                break;

            case mode::SUPERVISOR:
                load_func(this->banked_supervisor);
                this->spsr = this->spsr_supervisor;
                break;

            case mode::ABORT:
                load_func(this->banked_abort);
                this->spsr = this->spsr_abort;
                break;

            case mode::UNDEFINED:
                load_func(this->banked_undefined);
                this->spsr = this->spsr_undefined;
                break;
        }

        // finally update the new mode
        this->cpsr.set_mode(new_mode);
    }
};

constexpr auto my_size = sizeof(arm7tdmi);
static_assert(sizeof(arm7tdmi) == 184);

void arm_decode_test(u32 op);
void thumb_decode_test(u16 op);

} // namespace arm7tdmi

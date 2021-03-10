#pragma once

#include "../bit.hpp"

#include <cstdint>
#include <cassert>
#include <stdexcept>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

namespace gba::mem {

// NOTE:
// im not sure how to approach defining io registers.

// 0.) a struct bitfield with union would be easiest, but this is UB,
// when reading from a field that was NOT previously written to,
// and i'd rather avoid that (sadly!).

// 1.) use struct bitfield, but add 2 methods for read
// and write which join the bits together.
// this requires 2 implementations for both big and little endian!

// 2.) cast the struct to a byte / word then read / write
// when writing, mask the write value so read only bits or unused bits
// remain unchanged.
// likewise for reading, mask the value so only valid bits are read.
// i personally don't like this option, hard to track down bugs...

// 3.) the last way is create a struct with a single value width (byte / word)
// then add a bunch of methods (getters / setters) for each bit value
// this way, read and write full value will be fast as no shifting, and accessing
// each bitfield becomes an inlined function, which is just a mask / shift
// which is what the compiler should do anyway with actual bitfields.

// inheriting from this seems to make read and write non-constexpr
// as it fails with static_asserts...
template <typename T>
struct RegBaseRW {
    constexpr virtual ~RegBaseRW() = default;

    constexpr virtual auto read() const -> T {
        return this->value;
    }

    constexpr virtual auto write(const u16 v) -> void {
        this->value = v;
    }

    T value;
};

#define GENERATE_GET_SET_FUNC(bit_v, name, value) \
    [[nodiscard]] \
    constexpr auto get_##name() const noexcept -> bool { \
        return bit::is_set<bit_v>(value); \
    } \
    constexpr auto set_##name(const bool on) noexcept -> void { \
        value = bit::set<bit_v>(value, on); \
    }

// LCD I/O Display Control 
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiodisplaycontrol
struct DISPCNT {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr auto write(const u16 v) noexcept -> void {
        this->value = v & 0b1111'1111'1111'1000;
    }

    // NOTE: values should be 0-5
    [[nodiscard]]
    constexpr auto get_bg_mode() const noexcept -> u8 {
        assert((this->value & 0b111) <= 5);
        return this->value & 0b111;
    }

    // NOTE: values should be 0-5
    constexpr auto set_bg_mode(const u8 v) noexcept -> void {
        assert((v & 0b111) <= 5);
        this->value = (this->value & 0b1111'1111'1111'1000) | (v & 0b111);
    }

    GENERATE_GET_SET_FUNC(3, cgb_mode, this->value);
    GENERATE_GET_SET_FUNC(4, display_frame_select, this->value);
    GENERATE_GET_SET_FUNC(5, hblank_interval_free, this->value);
    GENERATE_GET_SET_FUNC(6, obj_character_vram_mapping, this->value);
    GENERATE_GET_SET_FUNC(7, forced_blank, this->value);
    GENERATE_GET_SET_FUNC(8, screen_display_bg0, this->value);
    GENERATE_GET_SET_FUNC(9, screen_display_bg1, this->value);
    GENERATE_GET_SET_FUNC(10, screen_display_bg2, this->value);
    GENERATE_GET_SET_FUNC(11, screen_display_bg3, this->value);
    GENERATE_GET_SET_FUNC(12, screen_display_obj, this->value);
    GENERATE_GET_SET_FUNC(13, window_0_display_flag, this->value);
    GENERATE_GET_SET_FUNC(14, window_1_display_flag, this->value);
    GENERATE_GET_SET_FUNC(15, obj_window_display_flag, this->value);
};

struct GreenSwap {
    u16 value : 1;

    [[nodiscard]]
    constexpr auto read() const noexcept -> bool {
        return this->value;
    }

    constexpr auto write(const u16 v) noexcept -> void {
        this->value = v & 1;
    }

    GENERATE_GET_SET_FUNC(0, green_swap, this->value);
};

// LCD I/O Interrupts and Status
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiointerruptsandstatus
struct DISPSTAT {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b1111'1111'0011'1111;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v & 0b1111'1111'0011'1111;
    }

    GENERATE_GET_SET_FUNC(0, vblank_flag, this->value);
    GENERATE_GET_SET_FUNC(1, hblank_flag, this->value);
    GENERATE_GET_SET_FUNC(2, vcounter_flag, this->value);
    GENERATE_GET_SET_FUNC(3, vblank_irq_enable, this->value);
    GENERATE_GET_SET_FUNC(4, hblank_irq_enable, this->value);
    GENERATE_GET_SET_FUNC(5, vcounter_irq_enable, this->value);

    [[nodiscard]]
    constexpr auto get_vcount_setting() const noexcept -> u8 {
        return this->value >> 8;
    }

    constexpr auto set_vcount_setting(const u8 v) noexcept -> void {
        this->value = (this->value & 0b0011'1111) | (v << 8);
    }
};

// read only!
struct VCOUNT {
    u16 value : 8;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    [[nodiscard]]
    constexpr auto get_current_scanline() const noexcept -> u8 {
        return this->value & 0xFF;
    }
};

// LCD I/O BG Control
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiobgcontrol
template <u8 num>
struct BGxCNT {
    u16 value;
    
    static_assert(num <= 3, "only 0-3 is valid for BGxCNT!");

    [[nodiscard]]
    constexpr auto read() const noexcept {
        if constexpr(num == 0 || num == 1) {
            return this->value & 0b1101'1111'1111'1111;
        } else {
            return this->value;
        }
    }

    constexpr void write(const u16 v) noexcept {
        if constexpr(num == 0 || num == 1) {
            this->value = v & 0b1101'1111'1111'1111;
        } else {
            this->value = v;
        }
    }

    [[nodiscard]]
    constexpr auto get_bg_priority() const noexcept -> u8 {
        return this->value & 0b11;
    }

    constexpr auto set_bg_priority(const u8 v) noexcept -> void {
        this->value = (this->value & 0b1111'1111'1111'1100) | (v & 0b11);
    }

    [[nodiscard]]
    constexpr auto get_char_base_block() const noexcept -> u8 {
        return (this->value >> 2) & 0b11;
    }

    constexpr auto set_char_base_block(const u8 v) noexcept -> void {
        this->value = (this->value & 0b1111'1111'1111'0011) | ((v & 0b11) << 2);
    }

    GENERATE_GET_SET_FUNC(6, mosaic, this->value);
    GENERATE_GET_SET_FUNC(7, colours_palettes, this->value);
    
    [[nodiscard]]
    constexpr auto get_screen_base_block() const noexcept -> u8 {
        return (this->value >> 8) & 0b1'1111;
    }

    constexpr auto set_screen_base_block(const u8 v) noexcept -> void {
        this->value = (this->value & 0b1110'0000'1111'1111) | ((v & 0b1'1111) << 8);
    }

    [[nodiscard]]
    constexpr auto get_display_area_overflow() const -> bool {
        if constexpr(num == 2 || num == 3) {
            return (this->value >> 13) & 0b1;
        } else {
            throw std::runtime_error("this method is not usable!");
        }
    }

    constexpr auto set_display_area_overflow(const bool v) -> void {
        if constexpr(num == 2 || num == 3) {
            this->value = (this->value & 0b1101'1111'1111'1111) | (v << 13);
        } else {
            throw std::runtime_error("this method is not usable!");
        }
    }

    [[nodiscard]]
    constexpr auto get_screen_size() const noexcept -> u8 {
        return (this->value >> 14) & 0b11;
    }

    constexpr auto set_screen_size(const u8 v) noexcept -> void {
        this->value = (this->value & 0b0011'1111'1111'1111) | ((v & 0b11) << 14);
    }
};

// LCD I/O BG Scrolling
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiobgscrolling
struct BG0HOFS {
    u16 value : 9;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    [[nodiscard]]
    constexpr auto get_offset() const noexcept {
        return this->value;
    }

    [[nodiscard]]
    constexpr auto set_offset(const u16 v) noexcept {
        this->value = v;
    }
};

// these are all alias's
using BG0VOFS = BG0HOFS;
using BG1HOFS = BG0HOFS;
using BG1VOFS = BG0HOFS;
using BG2HOFS = BG0HOFS;
using BG2VOFS = BG0HOFS;
using BG3HOFS = BG0HOFS;
using BG4VOFS = BG0HOFS;

constexpr void testing() {
    constexpr auto func = [](){
        DISPSTAT reg{};
        reg.write(33);
        return reg.read() == 33;
    };

    constexpr auto func2 = [](){
        const DISPCNT reg{33};
        return reg.read() == 33;
    };

    constexpr auto func3 = [](){
        const BGxCNT<2> bg0cnt{};
        return !bg0cnt.get_display_area_overflow();
    };

    constexpr auto func4 = [](){
        BG0VOFS reg{};
        reg.write(511);
        return reg.get_offset() == 511;
    };

    constexpr auto func5 = [](){
        BG0VOFS reg{};
        reg.write(512);
        return reg.get_offset() == 0;
    };

    static_assert(func());
    static_assert(func2());
    static_assert(func3());
    static_assert(func4());
    static_assert(func5());
}

#undef GENERATE_GET_SET_FUNC

} // namespace gba::mem

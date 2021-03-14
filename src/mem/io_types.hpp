#pragma once

#include "../bit.hpp"

#include <cstdint>
#include <cassert>
#include <stdexcept>

namespace gba::mem {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

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

#define GENERATE_GET_SET_FUNC(bit_v, name, value) \
    [[nodiscard]] \
    constexpr auto get_##name() const noexcept -> bool { \
        return bit::is_set<bit_v>(value); \
    } \
    constexpr auto set_##name(const bool on) noexcept -> void { \
        value = bit::set<bit_v>(value, on); \
    }

#define GENERATE_GET_SET_RANGE_FUNC(r0, r1, name, value) \
    [[nodiscard]] \
    constexpr auto get_##name() const noexcept -> auto { \
        return bit::get_range<r0, r1>(value); \
    } \
    constexpr auto set_##name(const auto v) noexcept -> void { \
        value = bit::set_range<r0, r1>(value, v); \
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

    GENERATE_GET_SET_RANGE_FUNC(0, 2, bg_mode, this->value);
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
        this->value = v & 0b1111'1111'0011'1000;
    }

    GENERATE_GET_SET_FUNC(0, vblank_flag, this->value);
    GENERATE_GET_SET_FUNC(1, hblank_flag, this->value);
    GENERATE_GET_SET_FUNC(2, vcounter_flag, this->value);
    GENERATE_GET_SET_FUNC(3, vblank_irq_enable, this->value);
    GENERATE_GET_SET_FUNC(4, hblank_irq_enable, this->value);
    GENERATE_GET_SET_FUNC(5, vcounter_irq_enable, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 15, vcount_setting, this->value);
};

// read only!
struct VCOUNT {
    u16 value : 8;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write([[maybe_unused]] const u16 v) noexcept {}

    GENERATE_GET_SET_RANGE_FUNC(0, 7, current_scanline, this->value);
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

    GENERATE_GET_SET_RANGE_FUNC(0, 1, bg_priority, this->value);
    GENERATE_GET_SET_RANGE_FUNC(2, 3, char_base_block, this->value);
    GENERATE_GET_SET_FUNC(6, mosaic, this->value);
    GENERATE_GET_SET_FUNC(7, colours_palettes, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 12, screen_base_block, this->value);

    [[nodiscard]]
    constexpr auto get_display_area_overflow() const -> bool {
        if constexpr(num == 2 || num == 3) {
            return bit::is_set<13>(this->value);
        } else {
            throw std::runtime_error("this method is not usable!");
        }
    }

    constexpr auto set_display_area_overflow(const bool v) -> void {
        if constexpr(num == 2 || num == 3) {
            this->value = bit::set<13>(this->value, v);
        } else {
            throw std::runtime_error("this method is not usable!");
        }
    }

    GENERATE_GET_SET_RANGE_FUNC(14, 15, screen_size, this->value);
};

using BG0CNT = BGxCNT<0>;
using BG1CNT = BGxCNT<1>;
using BG2CNT = BGxCNT<2>;
using BG3CNT = BGxCNT<3>;

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
using BG3VOFS = BG0HOFS;

// LCD I/O BG Rotation/Scaling
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiobgrotationscaling
struct BG2X_L {
    u32 value : 28;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 7, fractional, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 26, interger, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 27, interger_signed, this->value);
    GENERATE_GET_SET_FUNC(27, sign, this->value);
};

using BG2X_H = BG2X_L;
using BG2Y_L = BG2X_L;
using BG2Y_H = BG2X_L;

struct BG2PA {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 7, fractional, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 14, interger, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 15, interger_signed, this->value);
    GENERATE_GET_SET_FUNC(15, sign, this->value);
};

using BG2PB = BG2PA;
using BG2PC = BG2PA;
using BG2PD = BG2PA;

using BG3X_L = BG2X_L;
using BG3X_H = BG2X_L;
using BG3Y_L = BG2X_L;
using BG3Y_H = BG2X_L;

using BG3PA = BG2PA;
using BG3PB = BG2PA;
using BG3PC = BG2PA;
using BG3PD = BG2PA;

// LCD I/O Window Feature
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiowindowfeature
struct WIN0H {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    // TODO: manually write these (read GBATEK)
    GENERATE_GET_SET_RANGE_FUNC(0, 7, x2, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 15, x1, this->value);
};

using WIN1H = WIN0H;

struct WIN0V {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    // TODO: manually write these (read GBATEK) 
    GENERATE_GET_SET_RANGE_FUNC(0, 7, y2, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 15, y1, this->value);
};

using WIN1V = WIN0V;

struct WININ {
    u16 value : 14;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b0011'1111'0011'1111;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v & 0b0011'1111'0011'1111;
    }

    GENERATE_GET_SET_FUNC(0, window0_bg0_enable, this->value);
    GENERATE_GET_SET_FUNC(1, window0_bg1_enable, this->value);
    GENERATE_GET_SET_FUNC(2, window0_bg2_enable, this->value);
    GENERATE_GET_SET_FUNC(3, window0_bg3_enable, this->value);
    GENERATE_GET_SET_FUNC(4, window0_obj_enable, this->value);
    GENERATE_GET_SET_FUNC(5, window0_colour_special_effect, this->value);

    GENERATE_GET_SET_FUNC(8, window1_bg0_enable, this->value);
    GENERATE_GET_SET_FUNC(9, window1_bg1_enable, this->value);
    GENERATE_GET_SET_FUNC(10, window1_bg2_enable, this->value);
    GENERATE_GET_SET_FUNC(11, window1_bg3_enable, this->value);
    GENERATE_GET_SET_FUNC(12, window1_obj_enable, this->value);
    GENERATE_GET_SET_FUNC(13, window1_colour_special_effect, this->value);
};

struct WINOUT {
    u16 value : 14;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b0011'1111'0011'1111;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v & 0b0011'1111'0011'1111;
    }

    GENERATE_GET_SET_FUNC(0, outside_bg0_enable, this->value);
    GENERATE_GET_SET_FUNC(1, outside_bg1_enable, this->value);
    GENERATE_GET_SET_FUNC(2, outside_bg2_enable, this->value);
    GENERATE_GET_SET_FUNC(3, outside_bg3_enable, this->value);
    GENERATE_GET_SET_FUNC(4, outside_obj_enable, this->value);
    GENERATE_GET_SET_FUNC(5, outside_colour_special_effect, this->value);

    GENERATE_GET_SET_FUNC(8, obj_window_bg0_enable, this->value);
    GENERATE_GET_SET_FUNC(9, obj_window_bg1_enable, this->value);
    GENERATE_GET_SET_FUNC(10, obj_window_bg2_enable, this->value);
    GENERATE_GET_SET_FUNC(11, obj_window_bg3_enable, this->value);
    GENERATE_GET_SET_FUNC(12, obj_window_obj_enable, this->value);
    GENERATE_GET_SET_FUNC(13, obj_window_colour_special_effect, this->value);
};

// LCD I/O Mosaic Function
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiomosaicfunction
struct MOSAIC {
    u32 value : 16;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v;
    }

    // TODO: manually write these (read GBATEK) 
    GENERATE_GET_SET_RANGE_FUNC(0, 3, bg_h_size, this->value);
    GENERATE_GET_SET_RANGE_FUNC(4, 7, bg_v_size, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 11, obj_h_size, this->value);
    GENERATE_GET_SET_RANGE_FUNC(12, 15, obj_v_size, this->value);
};

// LCD I/O Color Special Effects
// SOURCE: https://problemkaputt.de/gbatek.htm#lcdiocolorspecialeffects
struct BLDCNT {
    u16 value : 14;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr auto write(const u16 v) noexcept -> void {
        this->value = v;
    }

    GENERATE_GET_SET_FUNC(0, bg0_1st_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(1, bg1_1st_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(2, bg2_1st_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(3, bg3_1st_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(4, obj_1st_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(5, bd_1st_target_pixel, this->value);
    GENERATE_GET_SET_RANGE_FUNC(6, 7, colour_special_effect, this->value);
    GENERATE_GET_SET_FUNC(8, bg0_2nd_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(9, bg1_2nd_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(10, bg2_2nd_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(11, bg3_2nd_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(12, obj_2nd_target_pixel, this->value);
    GENERATE_GET_SET_FUNC(13, bd_2nd_target_pixel, this->value);
};

struct BLDALPHA {
    u16 value : 13;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b0001'1111'0001'1111;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v & 0b0001'1111'0001'111;
    }

    // TODO: manually write these (read GBATEK) 
    GENERATE_GET_SET_RANGE_FUNC(0, 4, eva_coefficient_1st, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 12, eva_coefficient_2nd, this->value);
};

struct BLDY {
    u32 value : 5;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v;
    }

    // TODO: manually write these (read GBATEK) 
    GENERATE_GET_SET_RANGE_FUNC(0, 4, evy_coefficient, this->value);
};


// GBA Sound Channel 1 - Tone & Sweep
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundchannel1tonesweep
struct SOUND1CNT_L {
    u16 value : 7;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 2, num_sweep_shift, this->value);
    GENERATE_GET_SET_FUNC(3, sweep_freq_direction, this->value);
    GENERATE_GET_SET_RANGE_FUNC(4, 6, sweep_time, this->value);
};

struct SOUND1CNT_H {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b1111'1111'1100'0000;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 5, sound_len, this->value);
    GENERATE_GET_SET_RANGE_FUNC(6, 7, wave_pattern_duty, this->value);
    GENERATE_GET_SET_RANGE_FUNC(8, 10, envelope_step_time, this->value);
    GENERATE_GET_SET_FUNC(11, envelope_direction, this->value);
    GENERATE_GET_SET_RANGE_FUNC(12, 15, initial_vol_of_envelope, this->value);
};

struct SOUND1CNT_X {
    u32 value : 16;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b0100'0000'0000'0000;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v & 0b1100'1111'1111'1111;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 10, freq, this->value);
    GENERATE_GET_SET_FUNC(14, len_flag, this->value);
    GENERATE_GET_SET_FUNC(15, initial, this->value);
};

// GBA Sound Channel 2 - Tone
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundchannel2tone


// GBA Sound Channel 3 - Wave Output
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundchannel3waveoutput


// GBA Sound Channel 4 - Noise
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundchannel4noise


// GBA Sound Channel A and B - DMA Sound
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundchannelaandbdmasound


// GBA Sound Control Registers
// SOURCE: https://problemkaputt.de/gbatek.htm#gbasoundcontrolregisters

// 1 more still left...

struct SOUNDCNT_H {
    u16 value;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b1111'0000'0000'1111;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v & 0b1111'1111'0000'1111;
    }

    GENERATE_GET_SET_RANGE_FUNC(0, 1, sound_1_4_vol, this->value);
    GENERATE_GET_SET_FUNC(2, dma_sound_a_vol, this->value);
    GENERATE_GET_SET_FUNC(3, dma_sound_b_vol, this->value);
    GENERATE_GET_SET_FUNC(8, dma_sound_a_enable_right, this->value);
    GENERATE_GET_SET_FUNC(9, dma_sound_a_enable_left, this->value);
    GENERATE_GET_SET_FUNC(10, dma_sound_a_timer_select, this->value);
    GENERATE_GET_SET_FUNC(11, dma_sound_a_reset_fifo, this->value);
    GENERATE_GET_SET_FUNC(12, dma_sound_b_enable_right, this->value);
    GENERATE_GET_SET_FUNC(13, dma_sound_b_enable_left, this->value);
    GENERATE_GET_SET_FUNC(14, dma_sound_b_timer_select, this->value);
    GENERATE_GET_SET_FUNC(15, dma_sound_b_reset_fifo, this->value);
};

struct SOUNDCNT_X {
    u32 value : 8;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b1100'0000'1000'1111;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v & 0b0000'0000'1000'0000;
    }

    GENERATE_GET_SET_FUNC(0, sound_flag1, this->value);
    GENERATE_GET_SET_FUNC(1, sound_flag2, this->value);
    GENERATE_GET_SET_FUNC(2, sound_flag3, this->value);
    GENERATE_GET_SET_FUNC(3, sound_flag4, this->value);
    GENERATE_GET_SET_FUNC(7, psg_master_enable, this->value);
};

struct SOUNDBIAS {
    u32 value : 16;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value & 0b1100'0011'1111'1110;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v & 0b1100'0011'1111'1110;
    }

    GENERATE_GET_SET_RANGE_FUNC(1, 9, bias_level, this->value);
    GENERATE_GET_SET_RANGE_FUNC(14, 15, amplitude_sampling_cycle, this->value);
    GENERATE_GET_SET_FUNC(15, initial, this->value);
};

// 
// SOURCE: 
struct IME {
    u32 value : 1;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u32 v) noexcept {
        this->value = v;
    }
};

struct IE {
    u16 value : 14;

    [[nodiscard]]
    constexpr auto read() const noexcept {
        return this->value;
    }

    constexpr void write(const u16 v) noexcept {
        this->value = v;
    }

    GENERATE_GET_SET_FUNC(0, lcd_vblank, this->value);
    GENERATE_GET_SET_FUNC(1, lcd_hblank, this->value);
    GENERATE_GET_SET_FUNC(2, lcd_vcounter_match, this->value);
    GENERATE_GET_SET_FUNC(3, timer0_overflow, this->value);
    GENERATE_GET_SET_FUNC(4, timer1_overflow, this->value);
    GENERATE_GET_SET_FUNC(5, timer2_overflow, this->value);
    GENERATE_GET_SET_FUNC(6, timer3_overflow, this->value);
    GENERATE_GET_SET_FUNC(7, serial_communication, this->value);
    GENERATE_GET_SET_FUNC(8, dma0, this->value);
    GENERATE_GET_SET_FUNC(9, dma1, this->value);
    GENERATE_GET_SET_FUNC(10, dma2, this->value);
    GENERATE_GET_SET_FUNC(11, dma3, this->value);
    GENERATE_GET_SET_FUNC(12, keypad, this->value);
    GENERATE_GET_SET_FUNC(13, game_pak, this->value);
};

// they're the same
using IF = IE;

#undef GENERATE_GET_SET_FUNC
#undef GENERATE_GET_SET_RANGE_FUNC

} // namespace gba::mem

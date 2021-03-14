#pragma once

#include <cstdint>
#include <string_view>
#include <span>
#include <algorithm>
#include <numeric>

namespace gba::rom {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

// i wanted this to be able to be used constexpr
// for this, i could either:
// 1) - union struct with an array
// 2) - copy each entry manually...
// i decided to go for option 1 as it reads better, though it's
// UB because i write to the array then read from struct entries.
union header {
    static constexpr inline auto SIZE = 0xC0;
    static constexpr inline auto FIXED_VALUE = 0x96;
    static constexpr inline auto CHECKSUM_OFFSET = 0xA0;
    static constexpr inline auto CHECKSUM_SIZE = 0x1D;

    enum class language_code {
        Japan,
        Europe,
        French,
        Spanish,
        USA,
        German,
        Italian
    };

    struct {
        u32 rom_entry_point;
        u8 nintendo_logo[156];
        // uppercase ascii
        char game_title[12];
        char game_code[4];
        char maker_code[2];
        // this has to be 0x96
        u8 fixed_value;
        // should be 0x00
        u8 main_unit_code;
        u8 device_type;
        // should be all zero
        u8 _reserved_area[7];
        // should be 0
        u8 software_version;
        // header checksum
        u8 complement_check;
        // should be all zero
        u8 _reserved_area2[2];
    };

    constexpr header(const std::span<const u8> data) {
        if (data.size() < this->SIZE) {
            return;
        }

        std::copy(data.begin(), data.begin() + this->SIZE, this->private_data);
    }

    [[nodiscard]]
    constexpr auto get_game_title() const {
        return std::string_view{this->game_title, std::size(this->game_title)};   
    }

    [[nodiscard]]
    constexpr auto get_game_code() const {
        return std::string_view{this->game_code, std::size(this->game_code)};  
    }

    [[nodiscard]]
    constexpr auto get_maker_code() const {
        return std::string_view{this->maker_code, std::size(this->maker_code)};  
    }

    [[nodiscard]]
    constexpr auto get_checksum() const {
        const std::span check_data{
            this->private_data + this->CHECKSUM_OFFSET,
            this->CHECKSUM_SIZE
        };

        // perform checksum, sub all entries (with 0xFF wrapping)
        return std::accumulate(
            check_data.begin(), check_data.end(),
            -0x19, std::minus<u8>()
        );
    };

    [[nodiscard]]
    constexpr auto verify_checksum() const {
        return complement_check == this->get_checksum();
    }

    [[nodiscard]]
    constexpr auto verify_fixed_value() const {
        return this->fixed_value == FIXED_VALUE;
    }

private:
    u8 private_data[SIZE];
};

static_assert(
    sizeof(char) == 1,
    "char != 1-byte! this will break header size"
);

static_assert(
    sizeof(header) == header::SIZE,
    "Header size has changed!"
);

} // namespace gba::rom

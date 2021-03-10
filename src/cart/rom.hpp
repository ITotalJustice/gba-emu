#pragma once

#include <cstdint>

namespace gba::rom {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

constexpr auto HEADER_SIZE = 0xC0;
// i think this offset is to be used for the memory map...
constexpr auto HEADER_START_ADDR = 0x8000000;
constexpr auto HEADER_END_ADDR = 0x8000000 + HEADER_SIZE;
constexpr auto HEADER_FIXED_VALUE = 0x96;

struct header {
    u32 rom_entry_point;
    u8 nintendo_logo[156];
    // uppercase ascii
    u8 game_title[12];
    u8 game_code[4];
    u8 maker_code[2];
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

    constexpr bool verify_checksum() const {
        u8 check_data[0xBD - 0xA0];
        u8 idx = 0;

        // fill check_sum_data with values
        for (auto a : this->game_title) {
            check_data[idx++] = a;   
        }
        for (auto a : this->game_code) {
            check_data[idx++] = a;   
        }
        for (auto a : this->maker_code) {
            check_data[idx++] = a;   
        }
        check_data[idx++] = this->fixed_value;
        check_data[idx++] = this->main_unit_code;
        check_data[idx++] = this->device_type;
        for (auto a : this->_reserved_area) {
            check_data[idx++] = a;   
        }
        check_data[idx++] = this->software_version;

        // perform checksum
        u8 chk = 0;
    #if 0
        for (auto a : check_data) {
            chk = chk - a;
            chk = (chk - 0x19) & 0xFF;
        }
    #else
        for (auto a : check_data) {
            chk = chk - a;
        }
        chk = (chk - 0x19) & 0xFF;
    #endif

        return this->complement_check == chk;

        // return idx == sizeof(check_data);
    }

    constexpr bool verify_fixed_value() const {
        return this->fixed_value == HEADER_FIXED_VALUE;
    }
};

static_assert(sizeof(header) == HEADER_SIZE);

// constexpr void test() {
//     constexpr auto func = [](){
//         header header{};
//         return header.verify_checksum();
//     };

//     static_assert(func());
// }

} // namespace gba::rom

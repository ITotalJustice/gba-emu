#include "../rom.hpp"

namespace gba::rom {

constexpr auto test_rom_checksum() {
    // array of [array, checksum_value] pairs
    constexpr std::array<std::pair<std::array<u8, 0x1D>, u8>, 11> test_array {{
        {
            // GBAZELDA
            { 0x47, 0x42, 0x41, 0x5A, 0x45, 0x4C, 0x44, 0x41, 0x00, 0x00, 0x00, 0x00, 0x41, 0x5A, 0x4C, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x8A
        },
        {
            // GBAZELDA MC
            { 0x47, 0x42, 0x41, 0x5A, 0x45, 0x4C, 0x44, 0x41, 0x20, 0x4D, 0x43, 0x00, 0x42, 0x5A, 0x4D, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0xD8
        },
        {
            // MARIO KART
            { 0x4D, 0x41, 0x52, 0x49, 0x4F, 0x20, 0x4B, 0x41, 0x52, 0x54, 0x00, 0x00, 0x41, 0x4D, 0x4B, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x08   
        },
        {
            // METROID4EUR
            { 0x4D, 0x45, 0x54, 0x52, 0x4F, 0x49, 0x44, 0x34, 0x45, 0x55, 0x52, 0x00, 0x41, 0x4D, 0x54, 0x50, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x8A
        },
        {
            // NES ZELDA 2
            { 0x4E, 0x45, 0x53, 0x20, 0x5A, 0x45, 0x4C, 0x44, 0x41, 0x20, 0x32, 0x00, 0x46, 0x4C, 0x42, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x0F
        },
        {
            // POKE DUNGEON
            { 0x50, 0x4F, 0x4B, 0x45, 0x20, 0x44, 0x55, 0x4E, 0x47, 0x45, 0x4F, 0x4E, 0x42, 0x32, 0x34, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0xA4
        },
        {
            // POKEMON EMER
            { 0x50, 0x4F, 0x4B, 0x45, 0x4D, 0x4F, 0x4E, 0x20, 0x45, 0x4D, 0x45, 0x52, 0x42, 0x50, 0x45, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x72
        },
        {
            // POKEMON FIRE
            { 0x50, 0x4F, 0x4B, 0x45, 0x4D, 0x4F, 0x4E, 0x20, 0x46, 0x49, 0x52, 0x45, 0x42, 0x50, 0x52, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
            0x67
        },
        {
            // SUPER MARIO
            { 0x53, 0x55, 0x50, 0x45, 0x52, 0x20, 0x4D, 0x41, 0x52, 0x49, 0x4F, 0x00, 0x46, 0x53, 0x4D, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x9E
        },
        {
            // SUPER MARIOD
	        { 0x53, 0x55, 0x50, 0x45, 0x52, 0x20, 0x4D, 0x41, 0x52, 0x49, 0x4F, 0x44, 0x41, 0x58, 0x34, 0x45, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
            0x72
        },
        {
            // ZEROMISSIONP
            { 0x5A, 0x45, 0x52, 0x4F, 0x4D, 0x49, 0x53, 0x53, 0x49, 0x4F, 0x4E, 0x50, 0x42, 0x4D, 0x58, 0x50, 0x30, 0x31, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            0x07
        }
    }};

    constexpr auto func = [](const auto pair) {
        const auto [array, checksum] = pair;
        std::array<u8, header::SIZE> empty_header_data{};
        
        for (std::size_t i = 0; i < array.size(); ++i) {
            empty_header_data[header::CHECKSUM_OFFSET + i] = array[i];
        }

        const header head{empty_header_data};

        return head.get_checksum() == checksum;
    };

    constexpr auto loop_func = [func](const auto& array) {
        for (const auto& a : array) {
            if (!func(a)) {
                return false;
            }
        }

        return true;
    };

    static_assert(loop_func(test_array));
}

} // namespace gba::rom

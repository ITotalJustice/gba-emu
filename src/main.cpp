#include "arm7tdmi/arm7tdmi.hpp"
#include "arm7tdmi/thumb/alu_operations.hpp"

#include "mem/io_types.hpp"
#include "bit.hpp"
#include "cart/rom.hpp"

#include <cstdio>
#include <cassert>
#include <array>

#include <iostream>
#include <bitset>
#include <string_view>
#include <fstream>
#include <vector>
#include <optional>
#include <span>
#include <memory>

#ifdef TEST_DECODING
#include "extra/gen_arm_data_process_table/test.h"
#include "extra/gen_arm_data_process_table/test_lut.hpp"
#endif

#if 0
void format01_test() {
    
    // 0b000'00'00000'000'000
    const std::array<uint16_t, 9> array{
    // LSL
        0b000'00'00000'000'000,
        0b000'00'00000'000'000,
        0b000'00'00000'000'000,

    // LSR
        0b000'01'00000'000'000,
        0b000'01'00000'000'000,
        0b000'01'00000'000'000,

    // ASR
        0b000'10'00000'000'000,
        0b000'10'00000'000'000,
        0b000'10'00000'000'000,
    };

    for (auto op : array) {
        arm7tdmi::thumb_decode_test(op);
    }
}

void format02_test() {

    // 0b00011'0'0'000'000'000
    const std::array<uint16_t, 4> array{
    // ADD
        0b00011'0'0'000'000'000,
        0b00011'1'0'000'000'000,

    // SUB
        0b00011'0'1'000'000'000,
        0b00011'1'1'000'000'000,
    };

    for (auto op : array) {
        arm7tdmi::thumb_decode_test(op);
    }
}

void format03_test() {

    // 0b001'00'000'00000000
    const std::array<uint16_t, 8> array{
    // MOV
        0b001'00'000'00000000,
        0b001'00'000'00000000,

    // CMP
        0b001'01'000'00000000,
        0b001'01'000'00000000,

    // ADD
        0b001'10'000'00000000,
        0b001'10'000'00000000,

    // SUB
        0b001'11'000'00000000,
        0b001'11'000'00000000,
    };

    for (auto op : array) {
        arm7tdmi::thumb_decode_test(op);
    }
}

void format04_test() {

    // 0b010000'0000'000'000
    const std::array<uint16_t, 32> array{
    // AND
        0b010000'0000'000'000,
        0b010000'0000'000'000,

    // EOR
        0b010000'0001'000'000,
        0b010000'0001'000'000,

    // LSL
        0b010000'0010'000'000,
        0b010000'0010'000'000,

    // LSR
        0b010000'0011'000'000,
        0b010000'0011'000'000,

    // ASR
        0b010000'0100'000'000,
        0b010000'0100'000'000,

    // ADC
        0b010000'0101'000'000,
        0b010000'0101'000'000,

    // SBC
        0b010000'0110'000'000,
        0b010000'0110'000'000,

    // ROR
        0b010000'0111'000'000,
        0b010000'0111'000'000,

    // TST
        0b010000'1000'000'000,
        0b010000'1000'000'000,

    // NEG
        0b010000'1001'000'000,
        0b010000'1001'000'000,

    // CMP
        0b010000'1010'000'000,
        0b010000'1010'000'000,

    // CMN
        0b010000'1011'000'000,
        0b010000'1011'000'000,

    // ORR
        0b010000'1100'000'000,
        0b010000'1100'000'000,

    // MUL
        0b010000'1101'000'000,
        0b010000'1101'000'000,

    // BIC
        0b010000'1110'000'000,
        0b010000'1110'000'000,

    // MVN
        0b010000'1111'000'000,
        0b010000'1111'000'000,
    };

    for (auto op : array) {
        arm7tdmi::thumb_decode_test(op);
    }
}
#endif // #if 0

enum class save_type {
    EEPROM,
    SRAM,
    FLASH,
    FLASH512,
    FLASH1M
};

auto load_rom(const std::string_view path) -> std::optional<std::vector<std::uint8_t>> {
    std::ifstream fs{path.data(), std::ios_base::binary};
    
    if (fs.good()) {
        fs.seekg(0, std::ios_base::end);
        const auto size = fs.tellg();
        fs.seekg(0, std::ios_base::beg);

        std::vector<std::uint8_t> data;
        data.resize(size);
        
        fs.read(reinterpret_cast<char*>(data.data()), data.size());
        return data;
    }

    return std::nullopt;
}

// cannot be constexpr because of the cast to string_view sadly...
auto find_rom_save_type(const std::span<const std::uint8_t> data) -> std::optional<save_type> {
    // we need to parse the data as a string.
    const std::string_view sv{reinterpret_cast<const char*>(data.data()), data.size()};

    // these are the string IDS for the save type
    // all ID's are followed by 'nnn' which is the version number.
    // the version number is not needed, only the string to be found.
    constexpr std::array<std::pair<std::string_view, save_type>, 5> tokens{{
        { "EEPROM_V", save_type::EEPROM },
        { "SRAM_V", save_type::SRAM },
        { "FLASH_V", save_type::FLASH },
        { "FLASH512_V", save_type::FLASH512 },
        { "FLASH1M_V", save_type::FLASH1M }
    }};

    for (const auto [token, type] : tokens) {
        if (const auto it = sv.find(token); it != sv.npos) {
            // add 3 for the version num. this is optional!
            std::cout << "FOUND: " << sv.substr(it, token.size() + 3) << '\n';
            return type;
        }
    }

    // failed to find save type string...
    return std::nullopt;
}

auto print_rom_header(const std::span<const std::uint8_t> data) {
    const gba::rom::header header{data};
    const auto game_title = header.get_game_title();
    const auto game_code = header.get_game_code();
    const auto maker_code = header.get_maker_code();

    std::cout << "\nGame Title: " << game_title;
    std::cout << "\nGame Code: " << game_code;
    std::cout << "\nMaker Code: " << maker_code << '\n';

    std::cout << "Valid Checksum : " << std::boolalpha << header.verify_checksum() << '\n';
    std::cout << "Valid Fixed Value : " << std::boolalpha << header.verify_fixed_value() << '\n';
}

int main(int argc, char** argv) {

    // test rom loading and save type detection
    if (argc > 1) {
        if (auto rom_data = load_rom(argv[1])) {
            find_rom_save_type(*rom_data);
            print_rom_header(*rom_data);
        }
    }

    // auto arm = std::make_unique<arm7tdmi::arm7tdmi>();

    // for (uint32_t i = 0; i < UINT32_MAX; ++i) {
    //     if (test(i) != 0) {
    //         // printf("oh no\n");
    //         const auto val = (((i >> 16) & 0xFF0) | ((i >> 4) & 0xF)) & 0xFFF;
    //         const auto [num, str] = DILLION[val];
    //         if (strcmp("UNDEFINED", str) != 0/* && (i & 0x1000000) != 0x1000000*/) {
    //             printf("no match at 0x%03X: opcode: %X str: %s\n", val, i, str);
    //             std::cout << "0b" << std::bitset<32>(i) << '\n';
    //             assert(0);
    //         }
    //     }
    // }
#ifdef TEST_DECODING
    for (uint32_t i = 0; i < 0x1000; ++i) {
        if (test(i) != 0) {
            const auto [num, str] = DILLION[i];
            if (strcmp("UNDEFINED", str) != 0) {
                printf("no match at 0x%03X: opcode: %X str: %s\n", i, i, str);
                std::cout << "0b" << std::bitset<32>(i) << '\n';
                // assert(0);
            }
        }
    }
#endif // TEST_DECODING
    const unsigned v = 0b100'0000;
    const unsigned slice = 0b111;
    const auto set = bit::set_range<1, 3>(v, slice);
    std::cout << std::bitset<8>(set) << '\n';

    printf("done\n");
    // test(0xA0);
    // std::cout << std::bitset<8>(0x8f) << '\n';
    // format01_test();
    // format02_test();
    // format03_test();
    // format04_test();
    // arm7tdmi::thumb_decode_test(0x40A9);
    // arm_decode_test(0xE0214392);

    return 0;
}

#include "arm7tdmi/arm7tdmi.h"

#include <stdio.h>
#include <array>

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
        thumb_decode_test(op);
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
        thumb_decode_test(op);
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
        thumb_decode_test(op);
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
        thumb_decode_test(op);
    }
}

int main() {
    format01_test();
    format02_test();
    format03_test();
    format04_test();
    format05_test();
    format06_test();
    format07_test();
    format08_test();
    format09_test();
    format10_test();
    format11_test();
    format12_test();
    format13_test();
    format14_test();
    format15_test();
    format16_test();
    format17_test();
    format18_test();
    format19_test();

    return 0;
}

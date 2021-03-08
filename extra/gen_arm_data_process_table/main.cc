#include <cstdint>
#include <cstdio>
#include <array>
// #include <vector>
#include <bitset>
#include <iostream>

struct DumbWrapper {
    DumbWrapper(const char* str, const char* mode) {
        this->f = fopen(str, mode);
    }
    ~DumbWrapper() {
        if (this->f != nullptr) {
            fclose(this->f);
            this->f = nullptr;
        }
    }
    auto IsGood() const {
        return this->f != nullptr;
    }
    FILE* f{nullptr};
};

void fbreak(FILE* file) {
    fputs("\t\t\tbreak;\n", file);
}

void section_4_03(FILE* file) {
    fprintf(file, "\n// %s\n", "4.3");

    {
        uint32_t value = 0b0001'0010'0001;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X:\n", "BX", value);
        fbreak(file);
    }
}

void section_4_04(FILE* file) {
    fprintf(file, "\n// %s\n", "4.4");

    {
        uint32_t value = 0b101'0'00000000;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "B", value, value | 0b000'0'11111111);
        fbreak(file);
    }

    {
        uint32_t value = 0b101'1'00000000;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "BL", value, value | 0b000'1'11111111);
        fbreak(file);
    }
}

void section_4_05(FILE* file) {
    fprintf(file, "\n// %s\n", "4.5");

    const std::array opcode_strings{
        "AND",
        "EOR",
        "SUB",
        "RSB",
        "ADD",
        "ADC",
        "SBC",
        "RCS",
        "TST",
        "TEQ",
        "CMP",
        "CMN",
        "ORR",
        "MOV",
        "BIC",
        "MVN",
    };

    const std::array shift_type{
        "Logical Left",
        "Logical Right",
        "Arithmetic Right",
        "Rotate Right"
    };

    for (uint32_t i = 0; i < 16; ++i) {
        // set the opcode!
        const uint32_t value = (i << 17) >> 12;
        
        if (i == 0b1000 || i == 0b1001 || i == 0b1010 || i == 0b1011) {
fprintf(file,
"\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X ... 0x%03X:\n",
                opcode_strings[i], shift_type[0b00],
                value + 16, value + 17, value + 24,
                opcode_strings[i], shift_type[0b01],
                (value | 0b010) + 16, (value | 0b010) + 17, (value | 0b010) + 24,
                opcode_strings[i], shift_type[0b10],
                (value | 0b100) + 16, (value | 0b100) + 17, (value | 0b100) + 24,
                opcode_strings[i], shift_type[0b11],
                (value | 0b110) + 16, (value | 0b110) + 17, (value | 0b110) + 24,
                opcode_strings[i], "Rotate (S) #Imm",
                (value | 16 | 0x200), (value | 16 | 0x200) + 0b1111);
        } else {
fprintf(file,
"\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X: case 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X ... 0x%03X:\n\
\t\t// %s %s\n\
\t\tcase 0x%03X ... 0x%03X:\n",
                opcode_strings[i], shift_type[0b00],
                value, value + 1, value + 8, value + 16, value + 17, value + 24,
                opcode_strings[i], shift_type[0b01],
                (value | 0b010), (value | 0b010) + 1, (value | 0b010) + 8, (value | 0b010) + 16, (value | 0b010) + 17, (value | 0b010) + 24,
                opcode_strings[i], shift_type[0b10],
                (value | 0b100), (value | 0b100) + 1, (value | 0b100) + 8, (value | 0b100) + 16, (value | 0b100) + 17, (value | 0b100) + 24,
                opcode_strings[i], shift_type[0b11],
                (value | 0b110), (value | 0b110) + 1, (value | 0b110) + 8, (value | 0b110) + 16, (value | 0b110) + 17, (value | 0b110) + 24,
                opcode_strings[i], "Rotate (!S) #Imm",
                (value | 0x200), (value | 0x200) + 0b1111,
                opcode_strings[i], "Rotate (S) #Imm",
                (value | 16 | 0x200), (value | 16 | 0x200) + 0b1111);
        }

        fbreak(file);
    }
}

void section_4_06(FILE* file) {
    fprintf(file, "\n// %s\n", "4.6");

    const std::array array{
        "MRS (transfer PSR contents to a register)",
        "MSR (transfer register contents to PSR)",
        "MSR (transfer register contents or immdiate value to PSR flag bits only)"
    };

    {
        uint32_t value = 0b00010'0'00'0000;

        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n\t\t// %s %s\n\t\tcase 0x%03X:\n",
                array[0], "CPSR",
                value,
                array[0], "SPSR",
                value | 0x40);
            fbreak(file);
    }

    {
        uint32_t value = 0b00010'0'10'0000;

        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n\t\t// %s %s\n\t\tcase 0x%03X:\n",
                array[1], "CPSR",
                value,
                array[1], "SPSR",
                value | 0x40);
        fbreak(file);
    }

    {
        const uint32_t base_op = 0b00'0'10'0'10'0000;
        {
            uint32_t value = 0b00'1'10'0'10'0000; 
            fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X ... 0x%03X:\n",
                array[2], "CPSR",
                value, value + 0b1111);
            fbreak(file);

            fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X ... 0x%03X:\n",
                array[2], "SPSR",
                (value | 0x40), (value | 0x40) + 0b1111);
            fbreak(file);
        }
    }
}

void section_4_07(FILE* file) {
    fprintf(file, "\n// %s\n", "4.7");

    const std::array array{
        "MUL", "MLA"
    };

    0b000000'0'0'1001;
    {
        // MUL, !S
        uint32_t value = 0b000000'0'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[0], "!S", value);
        fbreak(file);
    }

    {
        // MUL, S
        uint32_t value = 0b000000'0'01'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[0], "1S", value);
        fbreak(file);
    }

    {
        // MLA, !S
        uint32_t value = 0b000000'1'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[1], "!S", value);
        fbreak(file);
    }

    {
        // MLA, S
        uint32_t value = 0b000000'1'1'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[1], "S", value);
        fbreak(file);
    }
}

void section_4_08(FILE* file) {
    fprintf(file, "\n// %s\n", "4.8");

    const std::array array{
        "UMULL", "SMULL",
        "UMLAL", "SMLAL"
    };

    0b00001'0'0'0'1001;
    {
        // UMULL, !S
        uint32_t value = 0b00001'0'0'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[0], "!S", value);
        fbreak(file);
    }

    {
        // UMULL, S
        uint32_t value = 0b00001'0'0'1'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[0], "S", value);
        fbreak(file);
    }

    {
        // SMULL, !S
        uint32_t value = 0b00001'1'0'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[1], "!S", value);
        fbreak(file);
    }

    {
        // SMULL, S
        uint32_t value = 0b00001'1'0'1'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[1], "S", value);
        fbreak(file);
    }

    {
        // UMLAL, !S
        uint32_t value = 0b00001'0'1'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[2], "!S", value);
        fbreak(file);
    }

    {
        // UMLAL, S
        uint32_t value = 0b00001'0'1'1'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[2], "S", value);
        fbreak(file);
    }

    {
        // SMLAL, !S
        uint32_t value = 0b00001'1'1'0'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[3], "!S", value);
        fbreak(file);
    }

    {
        // SMLAL, S
        uint32_t value = 0b00001'1'1'1'1001;
        fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", array[3], "S", value);
        fbreak(file);
    }
}

void section_4_09(FILE* file) {
    fprintf(file, "\n// %s\n", "4.9");

    const std::array shift_type{
        "Logical Left",
        "Logical Right",
        "Arithmetic Right",
        "Rotate Right"
    };


    {
        for (auto i = 0; i < 16; ++i) {
            uint32_t value = 0b01'0'0'0'0'0'0'0000 | (i << 5);
            fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X ... 0x%03X:\n", "STR", "#Imm", value, value | 0b1111);
            fbreak(file);
        }
    }

    {
        for (auto shift = 0b00; shift < 4; ++shift) {
            for (auto i = 0; i < 16; ++i) {
                uint32_t value = 0b01'1'0'0'0'0'0'0000 | (i << 5) | (shift << 1);
                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X: case 0x%03X:\n", "STR", shift_type[shift], value, value | 0b1000);
                fbreak(file);
            }
        }
    }

    {
        for (auto i = 0; i < 16; ++i) {
            uint32_t value = 0b01'0'0'0'0'0'1'0000 | (i << 5);
            fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X ... 0x%03X:\n", "LDR", "#Imm", value, value | 0b1111);
            fbreak(file);
        }
    }

    {
        for (auto shift = 0b00; shift < 4; ++shift) {
            for (auto i = 0; i < 16; ++i) {
                uint32_t value = 0b01'1'0'0'0'0'1'0000 | (i << 5) | (shift << 1);
                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X: case 0x%03X:\n", "LDR", shift_type[shift], value, value | 0b1000);
                fbreak(file);
            }
        }
    }
}

void section_4_10(FILE* file) {
    fprintf(file, "\n// %s\n", "4.10");

    // MCR = 0, MRC = 1
    0b000'0'0'0'0'0'1'01'1;

    const std::array array{
        "SWP", // unused
        "Unsigned halfwords",
        "Signed byte",
        "Signed halfwords"
    };
    
    // store Reg
    {
        for (auto sh = 0b01; sh < 0b100; ++sh) {
            for (auto i = 0; i < 0x8; ++i) {
                uint32_t value = 0b000'0'0'0'0'0'1'00'1 | (sh << 1);
                if (i > 1) {
                    value |= ((i & 0b110) << 6) | ((i & 1) << 5);
                }
                if (i == 1) {
                    value |= 1 << 5;
                }

                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", "STR Reg", array[sh], value);
                fbreak(file);
            }
        }
    }

    // Store #Imm
    {
        for (auto sh = 0b01; sh < 0b100; ++sh) {
            for (auto i = 0; i < 0x8; ++i) {
                uint32_t value = 0b000'0'0'1'0'0'1'00'1 | (sh << 1);
                if (i > 1) {
                    value |= ((i & 0b110) << 6) | ((i & 1) << 5);
                }
                if (i == 1) {
                    value |= 1 << 5;
                }

                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", "STR #Imm", array[sh], value);
                fbreak(file);
            }
        }
    }

    // LDR Reg
    {
        for (auto sh = 0b01; sh < 0b100; ++sh) {
            for (auto i = 0; i < 0x8; ++i) {
                uint32_t value = 0b000'0'0'0'0'1'1'00'1 | (sh << 1);
                if (i > 1) {
                    value |= ((i & 0b110) << 6) | ((i & 1) << 5);
                }
                if (i == 1) {
                    value |= 1 << 5;
                }

                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", "LDR Reg", array[sh], value);
                fbreak(file);
            }
        }
    }

    // Store #Imm
    {
        for (auto sh = 0b01; sh < 0b100; ++sh) {
            for (auto i = 0; i < 0x8; ++i) {
                uint32_t value = 0b000'0'0'1'0'1'1'00'1 | (sh << 1);
                if (i > 1) {
                    value |= ((i & 0b110) << 6) | ((i & 1) << 5);
                }
                if (i == 1) {
                    value |= 1 << 5;
                }

                fprintf(file, "\t\t// %s %s\n\t\tcase 0x%03X:\n", "LDR #Imm", array[sh], value);
                fbreak(file);
            }
        }
    }
}

void section_4_11(FILE* file) {
    fprintf(file, "\n// %s\n", "4.11");

    {
        for (auto i = 0; i < 0x10; ++i) {
            uint32_t value = (0b100'0'0'0'0'0'0000) | (i << 5);

            0b10000;
            fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "STM", value, value | 0b1111);
            fbreak(file);
        }
    }

    {
        for (auto i = 0; i < 0x10; ++i) {
            uint32_t value = (0b100'0'0'0'0'1'0000) | (i << 5);

            0b10000;
            fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "LDM", value, value | 0b1111);
            fbreak(file);
        }
    }
}

void section_4_12(FILE* file) {
    fprintf(file, "\n// %s\n", "4.12");

    {
        // SWP
        uint32_t value = 0b00010'0'001001;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X:\n", "SWP", value);
        fbreak(file);
    }

    {
        // SWPB
        uint32_t value = 0b00010'1'001001;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X:\n", "SWPB", value);
        fbreak(file);
    }
}

void section_4_13(FILE* file) {
    fprintf(file, "\n// %s\n", "4.13");

    {
        // SWI
        uint32_t value = 0b1111'00000000;
        fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n",
            "SWI", value, value | 0b1111'11111111);
        fbreak(file);
    }
}

void section_4_14(FILE* file) {
    fprintf(file, "\n// %s\n", "4.14");

    {
        // CDP
        fprintf(file, "\t\t// %s\n\t\t", "CDP");

        uint32_t value = 0b1110'0000000'0;
        int v = 0;

        while (value != 0b1111'0000000'0) {
            fprintf(file, "case 0x%03X: ", value);
            value += 2;
            ++v;
            if (v == 8) {
                fprintf(file, "\n\t\t");
                v = 0;
            }
        }
    }
    fbreak(file);
}

void section_4_15(FILE* file) {
    fprintf(file, "\n// %s\n", "4.15");

    0b110'0'0'0'0'0'0000;

    const std::array array{
        "LDC NO_WRITE, LEN=0"
    };

    {
        for (auto i = 0; i < 0x10; ++i) {
            uint32_t value = (0b110'0'0'0'0'0'0000) | (i << 5);

            0b10000;
            fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "STC", value, value | 0b1111);
            fbreak(file);
        }
    }

    {
        for (auto i = 0; i < 0x10; ++i) {
            uint32_t value = (0b110'0'0'0'0'1'0000) | (i << 5);

            0b10000;
            fprintf(file, "\t\t// %s\n\t\tcase 0x%03X ... 0x%03X:\n", "LDC", value, value | 0b1111);
            fbreak(file);
        }
    }
}

void section_4_16(FILE* file) {
    fprintf(file, "\n// %s\n", "4.16");

    // MCR = 0, MRC = 1
    {
        // MCR
        fprintf(file, "\t\t// %s\n\t\t", "MCR");

        for (auto i = 0; i < 0x8; ++i) {
            uint32_t value = 0b1110'000'0'000'1 | (i << 5);
            int v = 0;

            while ((value & 0x10) != 0x10) {
                fprintf(file, "case 0x%03X: ", value);
                value += 2;
                ++v;
                if (v == 8) {
                    fprintf(file, "\n\t\t");
                    v = 0;
                }
            }
        }
        fbreak(file);
    }

    {
        // MRC
        fprintf(file, "\t\t// %s\n\t\t", "MRC");

        for (auto i = 0; i < 0x8; ++i) {
            uint32_t value = 0b1110'000'1'000'1 | (i << 5);
            int v = 0;

            while ((value & 0x10) == 0x10) {
                fprintf(file, "case 0x%03X: ", value);
                value += 2;
                ++v;
                if (v == 8) {
                    fprintf(file, "\n\t\t");
                    v = 0;
                }
            }
        }
        fbreak(file);
    }
}

void section_4_17(FILE* file) {
    fprintf(file, "\n// %s\n", "4.17");

    {
        // UNDEFINED INSTRUCTION
        fprintf(file, "\t\t// %s\n\t\t", "UNDEFINED INSTRUCTION");

        uint32_t value = 0b011'00000000'1;
        int v = 0;

        while (value < 0b100'00000000'1) {
            fprintf(file, "case 0x%03X: ", value);
            value += 2;
            ++v;
            if (v == 8) {
                fprintf(file, "\n\t\t");
                v = 0;
            }
        }
    }
    fbreak(file);
}

int main() {
    DumbWrapper file{"test.h", "w"};
    if (!file.IsGood()) {
        std::puts("failed to open output file");
        return -1;
    }

    // this is a very dumb loop to generate a table of binary literals
    // for data processing instructions.
    // blame the shift encoding for this being such a pain...

    fprintf(file.f, "#include <stdint.h>\n#include <stdio.h>\n\nint test(uint32_t op) {\n");
    fprintf(file.f, "\tswitch ((((op >> 16) & 0xFF0) | ((op >> 4) & 0xF)) & 0xFFF) {\n");

    section_4_03(file.f);
    section_4_04(file.f);
    section_4_05(file.f);
    section_4_06(file.f);
    section_4_07(file.f);
    section_4_08(file.f);
    section_4_09(file.f);
    section_4_10(file.f);
    section_4_11(file.f);
    section_4_12(file.f);
    section_4_13(file.f);
    section_4_14(file.f);
    section_4_15(file.f);
    section_4_16(file.f);
    section_4_17(file.f);

    fprintf(file.f, "\ndefault:\n\treturn -1;\n");
    fprintf(file.f, "\n\t}\nreturn 0;\n}\n");

    std::puts("\ndone!");
    return 0;
}

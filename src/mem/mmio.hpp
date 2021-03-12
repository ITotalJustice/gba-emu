#pragma once

#include "io_types.hpp"

#include <cstdint>
#include <cassert>
#include <array>
// #include <vector> // not constexpr yet in gcc or clang...
#include <span>
#include <algorithm>

namespace gba::mem {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

// gba memory mapped (IO) interface
struct MMIO {
    static constexpr auto BIOS_REGION_START = 0x00000000;
    static constexpr auto BIOS_REGION_END = 0x00003FFF;

    static constexpr auto EWRAM_REGION_START = 0x02000000;
    static constexpr auto EWRAM_REGION_END = 0x0203FFFF;

    static constexpr auto IWRAM_REGION_START = 0x03000000;
    static constexpr auto IWRAM_REGION_END = 0x03007FFF;

    static constexpr auto IO_REGION_START = 0x04000000;
    static constexpr auto IO_REGION_END = 0x040003FE;

    static constexpr auto PALETTE_RAM_REGION_START = 0x05000000;
    static constexpr auto PALETTE_RAM_REGION_END = 0x050003FF;

    static constexpr auto VRAM_REGION_START = 0x06000000;
    static constexpr auto VRAM_REGION_END = 0x06017FFF;

    static constexpr auto OAM_REGION_START = 0x07000000;
    static constexpr auto OAM_REGION_END = 0x070003FF;

    constexpr auto load_rom(std::span<const u8> data) {
        if (data.size() > this->rom.size()) {
            assert(0 && "rom is way too beeg");
            return false;
        }

        std::copy(
            data.begin(), data.end(),
            this->rom.begin()
        );

        return true;
    }

    constexpr auto load_sram(std::span<const u8> data) {
        if (data.size() > this->sram.size()) {
            assert(0 && "sram is way too beeg");
            return false;
        }

        std::copy(
            data.begin(), data.end(),
            this->sram.begin()
        );

        return true;
    }

    // helpers for read / write arrays.
    // todo: add requires or static_assert for type (u8, u16, u32...)
    template <typename T>
    constexpr auto read_array(std::span<const u8> array, const u32 addr) -> T {
        if constexpr(sizeof(T) == sizeof(u8)) { // u8
            return array[addr];
        }
        if constexpr(sizeof(T) == sizeof(u16)) { // u16
            const auto hi = array[(addr + 0)];
            const auto lo = array[(addr + 1)];

            return (lo << 8) | hi;
        }
        if constexpr(sizeof(T) == sizeof(u32)) { // u32
            const auto hi_word_hi = array[(addr + 0)];
            const auto hi_word_lo = array[(addr + 1)];
            const auto lo_word_hi = array[(addr + 2)];
            const auto lo_word_lo = array[(addr + 3)];

            return (hi_word_hi << 24) | (hi_word_lo << 16) | (lo_word_hi << 8) | lo_word_lo;
        }
    }

    template <typename T>
    constexpr auto write_array(std::span<u8> array, const u32 addr, const T v) -> void {
        if constexpr(sizeof(T) == sizeof(u8)) { // u8
            array[addr] = v;
        }
        if constexpr(sizeof(T) == sizeof(u16)) { // u16
            array[(addr + 0)] = (v >> 0) & 0xFF;
            array[(addr + 1)] = (v >> 8) & 0xFF;
        }
        if constexpr(sizeof(T) == sizeof(u32)) { // u32
            array[(addr + 0)] = (v >> 0) & 0xFF;
            array[(addr + 1)] = (v >> 8) & 0xFF;
            array[(addr + 2)] = (v >> 16) & 0xFF;
            array[(addr + 3)] = (v >> 24) & 0xFF;
        }
    }

    // the top byte ((a >> 24) 0xF) can be used
    // as an index rather than a huge jump table
    template <typename T>
    constexpr auto read(const u32 addr) -> T {
        switch ((addr >> 8) & 0xFF'FF'FF) {
        // General Internal Memory
            case 0x000000 ... 0x00003F:   // BIOS - System ROM         (16 KBytes)
                break;
            case 0x000040 ... 0x01FFFF:   // Not used
                break;
            case 0x020000 ... 0x02FFFF:   // WRAM - On-board Work RAM  (256 KBytes) 2 Wait
                return this->read_array<T>(this->ewram, addr & 0x3FFFF);
            case 0x030000 ... 0x03FFFF:   // WRAM - On-chip Work RAM   (32 KBytes)
                return this->read_array<T>(this->iwram, addr & 0x7FFF);
            case 0x040000 ... 0x040003:   // I/O Registers (NOTE: this ends in E not F)
                return this->read_io<T>(addr);
            case 0x040004 ... 0x04FFFF:   // Not used
                break;
            
        // Internal Display Memory
            case 0x050000 ... 0x05FFFF:   // BG/OBJ Palette RAM        (1 Kbyte)
                return this->read_array<T>(this->palette_ram, addr & 0x3FF);
            case 0x060000 ... 0x06FFFF:   // VRAM - Video RAM          (96 KBytes)
                return this->read_array<T>(this->vram, addr & 0x17FFF);
            case 0x070000 ... 0x07FFFF:   // OAM - OBJ Attributes      (1 Kbyte)
                return this->read_array<T>(this->oam, addr & 0x3FF);
            
        // External Memory (Game Pak)
            case 0x080000 ... 0x0DFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 0/1/2
                return this->read_array<T>(this->rom, addr & 0x1FFFFFF);
            case 0x0E0000 ... 0x0E00FF:   // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
                if constexpr(sizeof(T) == sizeof(u8)) {
                    return this->read_array<T>(this->sram, addr & 0xFFFF);
                }
                // 16/32-bit access is an edge case
                // todo: handle it!
                if constexpr(sizeof(T) == sizeof(u16)) {
                    assert(0 && "edge case sram 16-bit read access");   
                    return 0xFACE; // silence warnings...
                }
                if constexpr(sizeof(T) == sizeof(u32)) {
                    assert(0 && "edge case sram 32-bit read access");
                    return 0xFACADE; // silence warnings...  
                }
            case 0x0E0100 ... 0x0FFFFF:   // Not used
                break;

        // Unused Memory Area
            default:   // Not used (upper 4bits of address bus unused)
                return 0xFF;
        }

        return 0x0;
    }

    template <typename T>
    constexpr auto write(const u32 addr, const T v) -> void {
        switch ((addr >> 8) & 0xFF'FF'FF) {
        // General Internal Memory
            case 0x000000 ... 0x00003F:   // BIOS - System ROM         (16 KBytes)
                break;
            case 0x000040 ... 0x01FFFF:   // Not used
                break;
            case 0x020000 ... 0x02FFFF:   // WRAM - On-board Work RAM  (256 KBytes) 2 Wait
                this->write_array<T>(this->ewram, addr & 0x3FFFF, v);
                break;
            case 0x030000 ... 0x03FFFF:   // WRAM - On-chip Work RAM   (32 KBytes)
                this->write_array<T>(this->iwram, addr & 0x7FFF, v);
                break;
            case 0x040000 ... 0x04FFFF:   // I/O Registers (NOTE: this ends in E not F)
                this->write_io<T>(addr, v);
                break;
            
        // Internal Display Memory
            case 0x050000 ... 0x05FFFF:   // BG/OBJ Palette RAM        (1 Kbyte)
                this->write_array<T>(this->palette_ram, addr & 0x3FF, v);
                break;
            case 0x060000 ... 0x06FFFF:   // VRAM - Video RAM          (96 KBytes)
                this->write_array<T>(this->vram, addr & 0x17FFF, v);
                break;
            case 0x070000 ... 0x07FFFF:   // OAM - OBJ Attributes      (1 Kbyte)
                this->write_array<T>(this->oam, addr & 0x3FF, v);
                break;
            
        // External Memory (Game Pak)
            case 0x080000 ... 0x0DFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 0/1/2
                assert(0 && "trying to write to rom addr...");
                break;
            case 0x0E0000 ... 0x0E00FF:   // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
                if constexpr(sizeof(T) == sizeof(u8)) {
                    this->write_array<T>(this->sram, addr & 0xFFFF, v);
                }
                // 16/32-bit access is an edge case
                // todo: handle it!
                if constexpr(sizeof(T) == sizeof(u16)) {
                    assert(0 && "edge case sram 16-bit write access");    
                }
                if constexpr(sizeof(T) == sizeof(u32)) {
                    assert(0 && "edge case sram 32-bit write access");    
                }
                break;
            case 0x0E0100 ... 0x0FFFFF:   // Not used
                break;

        // Unused Memory Area
            default:   // Not used (upper 4bits of address bus unused)
                break;
        }
    }

    template <typename T>
    constexpr auto read_io(const u32 addr) -> T {
        switch (addr & 0x7FF) {
        // LCD I/O Registers
            case 0x000:  // 2    R/W  DISPCNT   LCD Control
                return this->dispcnt.read();
            case 0x002:  // 2    R/W  -         Undocumented - Green Swap
                return this->green_swap.read();
            case 0x004:  // 2    R/W  DISPSTAT  General LCD Status (STAT,LYC)
                return this->dispstat.read();
            case 0x006:  // 2    R    VCOUNT    Vertical Counter (LY)
                return this->vcount.read();
            case 0x008:  // 2    R/W  BG0CNT    BG0 Control
                return this->bg0cnt.read();
            case 0x00A:  // 2    R/W  BG1CNT    BG1 Control
                return this->bg1cnt.read();
            case 0x00C:  // 2    R/W  BG2CNT    BG2 Control
                return this->bg2cnt.read();
            case 0x00E:  // 2    R/W  BG3CNT    BG3 Control
                return this->bg3cnt.read();
            case 0x010:  // 2    W    BG0HOFS   BG0 X-Offset
                return this->bg0hofs.read();
            case 0x012:  // 2    W    BG0VOFS   BG0 Y-Offset
                return this->bg0vofs.read();
            case 0x014:  // 2    W    BG1HOFS   BG1 X-Offset
                return this->bg1hofs.read();
            case 0x016:  // 2    W    BG1VOFS   BG1 Y-Offset
                return this->bg1vofs.read();
            case 0x018:  // 2    W    BG2HOFS   BG2 X-Offset
                return this->bg2hofs.read();
            case 0x01A:  // 2    W    BG2VOFS   BG2 Y-Offset
                return this->bg2vofs.read();
            case 0x01C:  // 2    W    BG3HOFS   BG3 X-Offset
                return this->bg3hofs.read();
            case 0x01E:  // 2    W    BG3VOFS   BG3 Y-Offset
                return this->bg3vofs.read();
            case 0x020:  // 2    W    BG2PA     BG2 Rotation/Scaling Parameter A (dx)
                break;
            case 0x022:  // 2    W    BG2PB     BG2 Rotation/Scaling Parameter B (dmx)
                break;
            case 0x024:  // 2    W    BG2PC     BG2 Rotation/Scaling Parameter C (dy)
                break;
            case 0x026:  // 2    W    BG2PD     BG2 Rotation/Scaling Parameter D (dmy)
                break;
            case 0x028:  // 4    W    BG2X      BG2 Reference Point X-Coordinate
                break;
            case 0x02C:  // 4    W    BG2Y      BG2 Reference Point Y-Coordinate
                break;
            case 0x030:  // 2    W    BG3PA     BG3 Rotation/Scaling Parameter A (dx)
                break;
            case 0x032:  // 2    W    BG3PB     BG3 Rotation/Scaling Parameter B (dmx)
                break;
            case 0x034:  // 2    W    BG3PC     BG3 Rotation/Scaling Parameter C (dy)
                break;
            case 0x036:  // 2    W    BG3PD     BG3 Rotation/Scaling Parameter D (dmy)
                break;
            case 0x038:  // 4    W    BG3X      BG3 Reference Point X-Coordinate
                break;
            case 0x03C:  // 4    W    BG3Y      BG3 Reference Point Y-Coordinate
                break;
            case 0x040:  // 2    W    WIN0H     Window 0 Horizontal Dimensions
                break;
            case 0x042:  // 2    W    WIN1H     Window 1 Horizontal Dimensions
                break;
            case 0x044:  // 2    W    WIN0V     Window 0 Vertical Dimensions
                break;
            case 0x046:  // 2    W    WIN1V     Window 1 Vertical Dimensions
                break;
            case 0x048:  // 2    R/W  WININ     Inside of Window 0 and 1
                break;
            case 0x04A:  // 2    R/W  WINOUT    Inside of OBJ Window & Outside of Windows
                break;
            case 0x04C:  // 2    W    MOSAIC    Mosaic Size
                break;
            case 0x04E:  //      -    -         Not used
                break;
            case 0x050:  // 2    R/W  BLDCNT    Color Special Effects Selection
                break;
            case 0x052:  // 2    R/W  BLDALPHA  Alpha Blending Coefficients
                break;
            case 0x054:  // 2    W    BLDY      Brightness (Fade-In/Out) Coefficient
                break;
            case 0x056:  //      -    -         Not used
                break;
                
        // Sound Registers
            case 0x060:  // 2  R/W  SOUND1CNT_L Channel 1 Sweep register       (NR10)
                break;
            case 0x062:  // 2  R/W  SOUND1CNT_H Channel 1 Duty/Length/Envelope (NR11, NR12)
                break;
            case 0x064:  // 2  R/W  SOUND1CNT_X Channel 1 Frequency/Control    (NR13, NR14)
                break;
            case 0x066:  //    -    -           Not used
                break;
            case 0x068:  // 2  R/W  SOUND2CNT_L Channel 2 Duty/Length/Envelope (NR21, NR22)
                break;
            case 0x06A:  //    -    -           Not used
                break;
            case 0x06C:  // 2  R/W  SOUND2CNT_H Channel 2 Frequency/Control    (NR23, NR24)
                break;
            case 0x06E:  //    -    -           Not used
                break;
            case 0x070:  // 2  R/W  SOUND3CNT_L Channel 3 Stop/Wave RAM select (NR30)
                break;
            case 0x072:  // 2  R/W  SOUND3CNT_H Channel 3 Length/Volume        (NR31, NR32)
                break;
            case 0x074:  // 2  R/W  SOUND3CNT_X Channel 3 Frequency/Control    (NR33, NR34)
                break;
            case 0x076:  //    -    -           Not used
                break;
            case 0x078:  // 2  R/W  SOUND4CNT_L Channel 4 Length/Envelope      (NR41, NR42)
                break;
            case 0x07A:  //    -    -           Not used
                break;
            case 0x07C:  // 2  R/W  SOUND4CNT_H Channel 4 Frequency/Control    (NR43, NR44)
                break;
            case 0x07E:  //    -    -           Not used
                break;
            case 0x080:  // 2  R/W  SOUNDCNT_L  Control Stereo/Volume/Enable   (NR50, NR51)
                break;
            case 0x082:  // 2  R/W  SOUNDCNT_H  Control Mixing/DMA Control
                break;
            case 0x084:  // 2  R/W  SOUNDCNT_X  Control Sound on/off           (NR52)
                break;
            case 0x086:  //    -    -           Not used
                break;
            case 0x088:  // 2  BIOS SOUNDBIAS   Sound PWM Control
                break;
            case 0x08A:  // ..   -    -         Not used
                break;
            case 0x090:  // 2x10h R/W  WAVE_RAM  Channel 3 Wave Pattern RAM (2 banks!!)
                break;
            case 0x0A0:  // 4    W    FIFO_A    Channel A FIFO, Data 0-3
                break;
            case 0x0A4:  // 4    W    FIFO_B    Channel B FIFO, Data 0-3
                break;
            case 0x0A8:  //      -    -         Not used
                break;

        // DMA Transfer Channels
            case 0x0B0:  // 4    W    DMA0SAD   DMA 0 Source Address
                break;
            case 0x0B4:  // 4    W    DMA0DAD   DMA 0 Destination Address
                break;
            case 0x0B8:  // 2    W    DMA0CNT_L DMA 0 Word Count
                break;
            case 0x0BA:  // 2    R/W  DMA0CNT_H DMA 0 Control
                break;
            case 0x0BC:  // 4    W    DMA1SAD   DMA 1 Source Address
                break;
            case 0x0C0:  // 4    W    DMA1DAD   DMA 1 Destination Address
                break;
            case 0x0C4:  // 2    W    DMA1CNT_L DMA 1 Word Count
                break;
            case 0x0C6:  // 2    R/W  DMA1CNT_H DMA 1 Control
                break;
            case 0x0C8:  // 4    W    DMA2SAD   DMA 2 Source Address
                break;
            case 0x0CC:  // 4    W    DMA2DAD   DMA 2 Destination Address
                break;
            case 0x0D0:  // 2    W    DMA2CNT_L DMA 2 Word Count
                break;
            case 0x0D2:  // 2    R/W  DMA2CNT_H DMA 2 Control
                break;
            case 0x0D4:  // 4    W    DMA3SAD   DMA 3 Source Address
                break;
            case 0x0D8:  // 4    W    DMA3DAD   DMA 3 Destination Address
                break;
            case 0x0DC:  // 2    W    DMA3CNT_L DMA 3 Word Count
                break;
            case 0x0DE:  // 2    R/W  DMA3CNT_H DMA 3 Control
                break;
            case 0x0E0:  //      -    -         Not used
                break;

        // Timer Registers
            case 0x100:  // 2    R/W  TM0CNT_L  Timer 0 Counter/Reload
                break;
            case 0x102:  // 2    R/W  TM0CNT_H  Timer 0 Control
                break;
            case 0x104:  // 2    R/W  TM1CNT_L  Timer 1 Counter/Reload
                break;
            case 0x106:  // 2    R/W  TM1CNT_H  Timer 1 Control
                break;
            case 0x108:  // 2    R/W  TM2CNT_L  Timer 2 Counter/Reload
                break;
            case 0x10A:  // 2    R/W  TM2CNT_H  Timer 2 Control
                break;
            case 0x10C:  // 2    R/W  TM3CNT_L  Timer 3 Counter/Reload
                break;
            case 0x10E:  // 2    R/W  TM3CNT_H  Timer 3 Control
                break;
            case 0x110:  //      -    -         Not used
                break;

        // Serial Communication (1)
            case 0x120:  // 4    R/W  SIODATA32 SIO Data (Normal-32bit Mode; shared with below)
                break;
            // case 0x120:  // 2    R/W  SIOMULTI0 SIO Data 0 (Parent)    (Multi-Player Mode)
            //     break;
            case 0x122:  // 2    R/W  SIOMULTI1 SIO Data 1 (1st Child) (Multi-Player Mode)
                break;
            case 0x124:  // 2    R/W  SIOMULTI2 SIO Data 2 (2nd Child) (Multi-Player Mode)
                break;
            case 0x126:  // 2    R/W  SIOMULTI3 SIO Data 3 (3rd Child) (Multi-Player Mode)
                break;
            case 0x128:  // 2    R/W  SIOCNT    SIO Control Register
                break;
            case 0x12A:  // 2    R/W  SIOMLT_SEND SIO Data (Local of MultiPlayer; shared below)
                break;
            // case 0x12A:  // 2    R/W  SIODATA8  SIO Data (Normal-8bit and UART Mode)
            //     break;
            case 0x12C:  //      -    -         Not used
                break;

        // Keypad Input
            case 0x130:  // 2    R    KEYINPUT  Key Status
                break;
            case 0x132:  // 2    R/W  KEYCNT    Key Interrupt Control
                break;

        // Serial Communication (2)
            case 0x134:  // 2    R/W  RCNT      SIO Mode Select/General Purpose Data
                break;
            case 0x136:  // -    -    IR        Ancient - Infrared Register (Prototypes only)
                break;
            case 0x138:  //      -    -         Not used
                break;
            case 0x140:  // 2    R/W  JOYCNT    SIO JOY Bus Control
                break;
            case 0x142:  //      -    -         Not used
                break;
            case 0x150:  // 4    R/W  JOY_RECV  SIO JOY Bus Receive Data
                break;
            case 0x154:  // 4    R/W  JOY_TRANS SIO JOY Bus Transmit Data
                break;
            case 0x158:  // 2    R/?  JOYSTAT   SIO JOY Bus Receive Status
                break;
            case 0x15A:  //      -    -         Not used
                break;

        // Interrupt, Waitstate, and Power-Down Control
            case 0x200:  // 2    R/W  IE        Interrupt Enable Register
                break;
            case 0x202:  // 2    R/W  IF        Interrupt Request Flags / IRQ Acknowledge
                break;
            case 0x204:  // 2    R/W  WAITCNT   Game Pak Waitstate Control
                break;
            case 0x206:  //      -    -         Not used
                break;
            case 0x208:  // 2    R/W  IME       Interrupt Master Enable Register
                break;
            case 0x20A:  //      -    -         Not used
                break;
            case 0x300:  // 1    R/W  POSTFLG   Undocumented - Post Boot Flag
                break;
            case 0x301:  // 1    W    HALTCNT   Undocumented - Power Down Control
                break;
            case 0x302:  //      -    -         Not used
                break;
            case 0x410:  // ?    ?    ?         Undocumented - Purpose Unknown / Bug ??? 0FFh
                break;
            case 0x411:  //      -    -         Not used
            
            // unreachable
            case 0x800:  // 4    R/W  ?         Undocumented - Internal Memory Control (R/W)
                break;
            // unreachable
            case 0x804:  //      -    -         Not used
                break;
        }

        return 0x0;
    }

    template <typename T>
    constexpr auto write_io(const u32 addr, const T v) -> void {
        switch (addr & 0x7FF) {
        // LCD I/O Registers
            case 0x000:  // 2    R/W  DISPCNT   LCD Control
                this->dispcnt.write(v);
                break;
            case 0x002:  // 2    R/W  -         Undocumented - Green Swap
                this->green_swap.write(v);
                break;
            case 0x004:  // 2    R/W  DISPSTAT  General LCD Status (STAT,LYC)
                this->dispstat.write(v);
                break;
            case 0x006:  // 2    R    VCOUNT    Vertical Counter (LY)
                break;
            case 0x008:  // 2    R/W  BG0CNT    BG0 Control
                this->bg0cnt.write(v);
                break;
            case 0x00A:  // 2    R/W  BG1CNT    BG1 Control
                this->bg1cnt.write(v);
                break;
            case 0x00C:  // 2    R/W  BG2CNT    BG2 Control
                this->bg2cnt.write(v);
                break;
            case 0x00E:  // 2    R/W  BG3CNT    BG3 Control
                this->bg3cnt.write(v);
                break;
            case 0x010:  // 2    W    BG0HOFS   BG0 X-Offset
                this->bg0hofs.write(v);
                break;
            case 0x012:  // 2    W    BG0VOFS   BG0 Y-Offset
                this->bg0vofs.write(v);
                break;
            case 0x014:  // 2    W    BG1HOFS   BG1 X-Offset
                this->bg1hofs.write(v);
                break;
            case 0x016:  // 2    W    BG1VOFS   BG1 Y-Offset
                this->bg1vofs.write(v);
                break;
            case 0x018:  // 2    W    BG2HOFS   BG2 X-Offset
                this->bg2hofs.write(v);
                break;
            case 0x01A:  // 2    W    BG2VOFS   BG2 Y-Offset
                this->bg2vofs.write(v);
                break;
            case 0x01C:  // 2    W    BG3HOFS   BG3 X-Offset
                this->bg3hofs.write(v);
                break;
            case 0x01E:  // 2    W    BG3VOFS   BG3 Y-Offset
                this->bg3vofs.write(v);
                break;
            case 0x020:  // 2    W    BG2PA     BG2 Rotation/Scaling Parameter A (dx)
                break;
            case 0x022:  // 2    W    BG2PB     BG2 Rotation/Scaling Parameter B (dmx)
                break;
            case 0x024:  // 2    W    BG2PC     BG2 Rotation/Scaling Parameter C (dy)
                break;
            case 0x026:  // 2    W    BG2PD     BG2 Rotation/Scaling Parameter D (dmy)
                break;
            case 0x028:  // 4    W    BG2X      BG2 Reference Point X-Coordinate
                break;
            case 0x02C:  // 4    W    BG2Y      BG2 Reference Point Y-Coordinate
                break;
            case 0x030:  // 2    W    BG3PA     BG3 Rotation/Scaling Parameter A (dx)
                break;
            case 0x032:  // 2    W    BG3PB     BG3 Rotation/Scaling Parameter B (dmx)
                break;
            case 0x034:  // 2    W    BG3PC     BG3 Rotation/Scaling Parameter C (dy)
                break;
            case 0x036:  // 2    W    BG3PD     BG3 Rotation/Scaling Parameter D (dmy)
                break;
            case 0x038:  // 4    W    BG3X      BG3 Reference Point X-Coordinate
                break;
            case 0x03C:  // 4    W    BG3Y      BG3 Reference Point Y-Coordinate
                break;
            case 0x040:  // 2    W    WIN0H     Window 0 Horizontal Dimensions
                break;
            case 0x042:  // 2    W    WIN1H     Window 1 Horizontal Dimensions
                break;
            case 0x044:  // 2    W    WIN0V     Window 0 Vertical Dimensions
                break;
            case 0x046:  // 2    W    WIN1V     Window 1 Vertical Dimensions
                break;
            case 0x048:  // 2    R/W  WININ     Inside of Window 0 and 1
                break;
            case 0x04A:  // 2    R/W  WINOUT    Inside of OBJ Window & Outside of Windows
                break;
            case 0x04C:  // 2    W    MOSAIC    Mosaic Size
                break;
            case 0x04E:  //      -    -         Not used
                break;
            case 0x050:  // 2    R/W  BLDCNT    Color Special Effects Selection
                break;
            case 0x052:  // 2    R/W  BLDALPHA  Alpha Blending Coefficients
                break;
            case 0x054:  // 2    W    BLDY      Brightness (Fade-In/Out) Coefficient
                break;
            case 0x056:  //      -    -         Not used
                break;
                
        // Sound Registers
            case 0x060:  // 2  R/W  SOUND1CNT_L Channel 1 Sweep register       (NR10)
                break;
            case 0x062:  // 2  R/W  SOUND1CNT_H Channel 1 Duty/Length/Envelope (NR11, NR12)
                break;
            case 0x064:  // 2  R/W  SOUND1CNT_X Channel 1 Frequency/Control    (NR13, NR14)
                break;
            case 0x066:  //    -    -           Not used
                break;
            case 0x068:  // 2  R/W  SOUND2CNT_L Channel 2 Duty/Length/Envelope (NR21, NR22)
                break;
            case 0x06A:  //    -    -           Not used
                break;
            case 0x06C:  // 2  R/W  SOUND2CNT_H Channel 2 Frequency/Control    (NR23, NR24)
                break;
            case 0x06E:  //    -    -           Not used
                break;
            case 0x070:  // 2  R/W  SOUND3CNT_L Channel 3 Stop/Wave RAM select (NR30)
                break;
            case 0x072:  // 2  R/W  SOUND3CNT_H Channel 3 Length/Volume        (NR31, NR32)
                break;
            case 0x074:  // 2  R/W  SOUND3CNT_X Channel 3 Frequency/Control    (NR33, NR34)
                break;
            case 0x076:  //    -    -           Not used
                break;
            case 0x078:  // 2  R/W  SOUND4CNT_L Channel 4 Length/Envelope      (NR41, NR42)
                break;
            case 0x07A:  //    -    -           Not used
                break;
            case 0x07C:  // 2  R/W  SOUND4CNT_H Channel 4 Frequency/Control    (NR43, NR44)
                break;
            case 0x07E:  //    -    -           Not used
                break;
            case 0x080:  // 2  R/W  SOUNDCNT_L  Control Stereo/Volume/Enable   (NR50, NR51)
                break;
            case 0x082:  // 2  R/W  SOUNDCNT_H  Control Mixing/DMA Control
                break;
            case 0x084:  // 2  R/W  SOUNDCNT_X  Control Sound on/off           (NR52)
                break;
            case 0x086:  //    -    -           Not used
                break;
            case 0x088:  // 2  BIOS SOUNDBIAS   Sound PWM Control
                break;
            case 0x08A:  // ..   -    -         Not used
                break;
            case 0x090:  // 2x10h R/W  WAVE_RAM  Channel 3 Wave Pattern RAM (2 banks!!)
                break;
            case 0x0A0:  // 4    W    FIFO_A    Channel A FIFO, Data 0-3
                break;
            case 0x0A4:  // 4    W    FIFO_B    Channel B FIFO, Data 0-3
                break;
            case 0x0A8:  //      -    -         Not used
                break;

        // DMA Transfer Channels
            case 0x0B0:  // 4    W    DMA0SAD   DMA 0 Source Address
                break;
            case 0x0B4:  // 4    W    DMA0DAD   DMA 0 Destination Address
                break;
            case 0x0B8:  // 2    W    DMA0CNT_L DMA 0 Word Count
                break;
            case 0x0BA:  // 2    R/W  DMA0CNT_H DMA 0 Control
                break;
            case 0x0BC:  // 4    W    DMA1SAD   DMA 1 Source Address
                break;
            case 0x0C0:  // 4    W    DMA1DAD   DMA 1 Destination Address
                break;
            case 0x0C4:  // 2    W    DMA1CNT_L DMA 1 Word Count
                break;
            case 0x0C6:  // 2    R/W  DMA1CNT_H DMA 1 Control
                break;
            case 0x0C8:  // 4    W    DMA2SAD   DMA 2 Source Address
                break;
            case 0x0CC:  // 4    W    DMA2DAD   DMA 2 Destination Address
                break;
            case 0x0D0:  // 2    W    DMA2CNT_L DMA 2 Word Count
                break;
            case 0x0D2:  // 2    R/W  DMA2CNT_H DMA 2 Control
                break;
            case 0x0D4:  // 4    W    DMA3SAD   DMA 3 Source Address
                break;
            case 0x0D8:  // 4    W    DMA3DAD   DMA 3 Destination Address
                break;
            case 0x0DC:  // 2    W    DMA3CNT_L DMA 3 Word Count
                break;
            case 0x0DE:  // 2    R/W  DMA3CNT_H DMA 3 Control
                break;
            case 0x0E0:  //      -    -         Not used
                break;

        // Timer Registers
            case 0x100:  // 2    R/W  TM0CNT_L  Timer 0 Counter/Reload
                break;
            case 0x102:  // 2    R/W  TM0CNT_H  Timer 0 Control
                break;
            case 0x104:  // 2    R/W  TM1CNT_L  Timer 1 Counter/Reload
                break;
            case 0x106:  // 2    R/W  TM1CNT_H  Timer 1 Control
                break;
            case 0x108:  // 2    R/W  TM2CNT_L  Timer 2 Counter/Reload
                break;
            case 0x10A:  // 2    R/W  TM2CNT_H  Timer 2 Control
                break;
            case 0x10C:  // 2    R/W  TM3CNT_L  Timer 3 Counter/Reload
                break;
            case 0x10E:  // 2    R/W  TM3CNT_H  Timer 3 Control
                break;
            case 0x110:  //      -    -         Not used
                break;

        // Serial Communication (1)
            case 0x120:  // 4    R/W  SIODATA32 SIO Data (Normal-32bit Mode; shared with below)
                break;
            // case 0x120:  // 2    R/W  SIOMULTI0 SIO Data 0 (Parent)    (Multi-Player Mode)
            //     break;
            case 0x122:  // 2    R/W  SIOMULTI1 SIO Data 1 (1st Child) (Multi-Player Mode)
                break;
            case 0x124:  // 2    R/W  SIOMULTI2 SIO Data 2 (2nd Child) (Multi-Player Mode)
                break;
            case 0x126:  // 2    R/W  SIOMULTI3 SIO Data 3 (3rd Child) (Multi-Player Mode)
                break;
            case 0x128:  // 2    R/W  SIOCNT    SIO Control Register
                break;
            case 0x12A:  // 2    R/W  SIOMLT_SEND SIO Data (Local of MultiPlayer; shared below)
                break;
            // case 0x12A:  // 2    R/W  SIODATA8  SIO Data (Normal-8bit and UART Mode)
            //     break;
            case 0x12C:  //      -    -         Not used
                break;

        // Keypad Input
            case 0x130:  // 2    R    KEYINPUT  Key Status
                break;
            case 0x132:  // 2    R/W  KEYCNT    Key Interrupt Control
                break;

        // Serial Communication (2)
            case 0x134:  // 2    R/W  RCNT      SIO Mode Select/General Purpose Data
                break;
            case 0x136:  // -    -    IR        Ancient - Infrared Register (Prototypes only)
                break;
            case 0x138:  //      -    -         Not used
                break;
            case 0x140:  // 2    R/W  JOYCNT    SIO JOY Bus Control
                break;
            case 0x142:  //      -    -         Not used
                break;
            case 0x150:  // 4    R/W  JOY_RECV  SIO JOY Bus Receive Data
                break;
            case 0x154:  // 4    R/W  JOY_TRANS SIO JOY Bus Transmit Data
                break;
            case 0x158:  // 2    R/?  JOYSTAT   SIO JOY Bus Receive Status
                break;
            case 0x15A:  //      -    -         Not used
                break;

        // Interrupt, Waitstate, and Power-Down Control
            case 0x200:  // 2    R/W  IE        Interrupt Enable Register
                break;
            case 0x202:  // 2    R/W  IF        Interrupt Request Flags / IRQ Acknowledge
                break;
            case 0x204:  // 2    R/W  WAITCNT   Game Pak Waitstate Control
                break;
            case 0x206:  //      -    -         Not used
                break;
            case 0x208:  // 2    R/W  IME       Interrupt Master Enable Register
                break;
            case 0x20A:  //      -    -         Not used
                break;
            case 0x300:  // 1    R/W  POSTFLG   Undocumented - Post Boot Flag
                break;
            case 0x301:  // 1    W    HALTCNT   Undocumented - Power Down Control
                break;
            case 0x302:  //      -    -         Not used
                break;
            case 0x410:  // ?    ?    ?         Undocumented - Purpose Unknown / Bug ??? 0FFh
                break;
            case 0x411:  //      -    -         Not used
            
            // unreachable
            case 0x800:  // 4    R/W  ?         Undocumented - Internal Memory Control (R/W)
                break;
            // unreachable
            case 0x804:  //      -    -         Not used
                break;
        }
    }

    // 32mb(max), 16-bus
    std::array<u8, 0x2000000> rom{};

    // 64kb(max), 8-bus
    std::array<u8, 0x10000> sram{};

    // 256kb, 16-bit bus
    std::array<u8, 0x40000> ewram{};
    
    // 32kb, 32-bit bus
    std::array<u8, 0x8000> iwram{};

    // 1kb, 16-bit
    std::array<u8, 0x400> palette_ram{};

    // 96kb, 16-bit bus
    std::array<u8, 0x18000> vram{};

    // 1kb, 32-bit
    std::array<u8, 0x400> oam{};

    // IO registers...
    DISPCNT dispcnt;
    GreenSwap green_swap;
    DISPSTAT dispstat;
    VCOUNT vcount;
    
    BG0CNT bg0cnt;
    BG1CNT bg1cnt;
    BG2CNT bg2cnt;
    BG3CNT bg3cnt;

    BG0HOFS bg0hofs;
    BG0VOFS bg0vofs;
    BG1HOFS bg1hofs;
    BG1VOFS bg1vofs;
    BG2HOFS bg2hofs;
    BG2VOFS bg2vofs;
    BG3HOFS bg3hofs;
    BG3VOFS bg3vofs;

    // todo: many many more registers...
};

} // namespace gba::mem

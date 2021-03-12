#pragma once

#include "../bit.hpp"

#include <cstdint>
#include <cassert>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

namespace gba::mem {

// SOURCE: https://problemkaputt.de/gbatek.htm#gbamemorymap
void map1(const u32 addr) {
    switch (addr) {
    // General Internal Memory
        case 0x00000000 ... 0x00003FFF:   // BIOS - System ROM         (16 KBytes)
            break;
        case 0x00004000 ... 0x01FFFFFF:   // Not used
            break;
        case 0x02000000 ... 0x0203FFFF:   // WRAM - On-board Work RAM  (256 KBytes) 2 Wait
            break;
        case 0x02040000 ... 0x02FFFFFF:   // Not used
            break;
        case 0x03000000 ... 0x03007FFF:   // WRAM - On-chip Work RAM   (32 KBytes)
            break;
        case 0x03008000 ... 0x03FFFFFF:   // Not used
            break;
        case 0x04000000 ... 0x040003FE:   // I/O Registers
            break;
        case 0x04000400 ... 0x04FFFFFF:   // Not used
            break;
        
    // Internal Display Memory
        case 0x05000000 ... 0x050003FF:   // BG/OBJ Palette RAM        (1 Kbyte)
            break;
        case 0x05000400 ... 0x05FFFFFF:   // Not used
            break;
        case 0x06000000 ... 0x06017FFF:   // VRAM - Video RAM          (96 KBytes)
            break;
        case 0x06018000 ... 0x06FFFFFF:   // Not used
            break;
        case 0x07000000 ... 0x070003FF:   // OAM - OBJ Attributes      (1 Kbyte)
            break;
        case 0x07000400 ... 0x07FFFFFF:   // Not used
            break;
    
    // External Memory (Game Pak)
        case 0x08000000 ... 0x09FFFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 0
            break;
        case 0x0A000000 ... 0x0BFFFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 1
            break;
        case 0x0C000000 ... 0x0DFFFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 2
            break;
        case 0x0E000000 ... 0x0E00FFFF:   // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
            break;
        case 0x0E010000 ... 0x0FFFFFFF:   // Not used
            break;

    // Unused Memory Area
        case 0x10000000 ... 0xFFFFFFFF:   // Not used (upper 4bits of address bus unused)
            break;
    }
}

// this version uses bits 27-7
void map2(const u32 addr) {
    switch ((addr >> 8) & 0xF'FF'FF) {
    // General Internal Memory
        case 0x00000 ... 0x0003F:   // BIOS - System ROM         (16 KBytes)
            break;
        case 0x00040 ... 0x1FFFF:   // Not used
            break;
        case 0x20000 ... 0x203FF:   // WRAM - On-board Work RAM  (256 KBytes) 2 Wait
            break;
        case 0x20400 ... 0x2FFFF:   // Not used
            break;
        case 0x30000 ... 0x3007F:   // WRAM - On-chip Work RAM   (32 KBytes)
            break;
        case 0x30080 ... 0x3FFFF:   // Not used
            break;
        case 0x40000 ... 0x40003:   // I/O Registers (NOTE: this ends in E not F)
            break;
        case 0x40004 ... 0x4FFFF:   // Not used
            break;
        
    // Internal Display Memory
        case 0x50000 ... 0x50003:   // BG/OBJ Palette RAM        (1 Kbyte)
            break;
        case 0x50004 ... 0x5FFFF:   // Not used
            break;
        case 0x60000 ... 0x6017F:   // VRAM - Video RAM          (96 KBytes)
            break;
        case 0x60180 ... 0x6FFFF:   // Not used
            break;
        case 0x70000 ... 0x70003:   // OAM - OBJ Attributes      (1 Kbyte)
            break;
        case 0x70004 ... 0x7FFFF:   // Not used
            break;
        
    // External Memory (Game Pak)
        case 0x80000 ... 0x9FFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 0
            break;
        case 0xA0000 ... 0xBFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 1
            break;
        case 0xC0000 ... 0xDFFFF:   // Game Pak ROM/FlashROM (max 32MB) - Wait State 2
            break;
        case 0xE0000 ... 0xE00FF:   // Game Pak SRAM    (max 64 KBytes) - 8bit Bus width
            break;
        case 0xE0100 ... 0xFFFFF:   // Not used
            break;

    // Unused Memory Area
        default:   // Not used (upper 4bits of address bus unused)
            break;
    }
}

void io_map1(const u32 addr) {
    switch (addr) {
    // LCD I/O Registers
        case 0x4000000:  // 2    R/W  DISPCNT   LCD Control
        case 0x4000002:  // 2    R/W  -         Undocumented - Green Swap
        case 0x4000004:  // 2    R/W  DISPSTAT  General LCD Status (STAT,LYC)
        case 0x4000006:  // 2    R    VCOUNT    Vertical Counter (LY)
        case 0x4000008:  // 2    R/W  BG0CNT    BG0 Control
        case 0x400000A:  // 2    R/W  BG1CNT    BG1 Control
        case 0x400000C:  // 2    R/W  BG2CNT    BG2 Control
        case 0x400000E:  // 2    R/W  BG3CNT    BG3 Control
        case 0x4000010:  // 2    W    BG0HOFS   BG0 X-Offset
        case 0x4000012:  // 2    W    BG0VOFS   BG0 Y-Offset
        case 0x4000014:  // 2    W    BG1HOFS   BG1 X-Offset
        case 0x4000016:  // 2    W    BG1VOFS   BG1 Y-Offset
        case 0x4000018:  // 2    W    BG2HOFS   BG2 X-Offset
        case 0x400001A:  // 2    W    BG2VOFS   BG2 Y-Offset
        case 0x400001C:  // 2    W    BG3HOFS   BG3 X-Offset
        case 0x400001E:  // 2    W    BG3VOFS   BG3 Y-Offset
        case 0x4000020:  // 2    W    BG2PA     BG2 Rotation/Scaling Parameter A (dx)
        case 0x4000022:  // 2    W    BG2PB     BG2 Rotation/Scaling Parameter B (dmx)
        case 0x4000024:  // 2    W    BG2PC     BG2 Rotation/Scaling Parameter C (dy)
        case 0x4000026:  // 2    W    BG2PD     BG2 Rotation/Scaling Parameter D (dmy)
        case 0x4000028:  // 4    W    BG2X      BG2 Reference Point X-Coordinate
        case 0x400002C:  // 4    W    BG2Y      BG2 Reference Point Y-Coordinate
        case 0x4000030:  // 2    W    BG3PA     BG3 Rotation/Scaling Parameter A (dx)
        case 0x4000032:  // 2    W    BG3PB     BG3 Rotation/Scaling Parameter B (dmx)
        case 0x4000034:  // 2    W    BG3PC     BG3 Rotation/Scaling Parameter C (dy)
        case 0x4000036:  // 2    W    BG3PD     BG3 Rotation/Scaling Parameter D (dmy)
        case 0x4000038:  // 4    W    BG3X      BG3 Reference Point X-Coordinate
        case 0x400003C:  // 4    W    BG3Y      BG3 Reference Point Y-Coordinate
        case 0x4000040:  // 2    W    WIN0H     Window 0 Horizontal Dimensions
        case 0x4000042:  // 2    W    WIN1H     Window 1 Horizontal Dimensions
        case 0x4000044:  // 2    W    WIN0V     Window 0 Vertical Dimensions
        case 0x4000046:  // 2    W    WIN1V     Window 1 Vertical Dimensions
        case 0x4000048:  // 2    R/W  WININ     Inside of Window 0 and 1
        case 0x400004A:  // 2    R/W  WINOUT    Inside of OBJ Window & Outside of Windows
        case 0x400004C:  // 2    W    MOSAIC    Mosaic Size
        case 0x400004E:  //      -    -         Not used
        case 0x4000050:  // 2    R/W  BLDCNT    Color Special Effects Selection
        case 0x4000052:  // 2    R/W  BLDALPHA  Alpha Blending Coefficients
        case 0x4000054:  // 2    W    BLDY      Brightness (Fade-In/Out) Coefficient
        case 0x4000056:  //      -    -         Not used
        
    // Sound Registers
        case 0x4000060:  // 2  R/W  SOUND1CNT_L Channel 1 Sweep register       (NR10)
        case 0x4000062:  // 2  R/W  SOUND1CNT_H Channel 1 Duty/Length/Envelope (NR11, NR12)
        case 0x4000064:  // 2  R/W  SOUND1CNT_X Channel 1 Frequency/Control    (NR13, NR14)
        case 0x4000066:  //    -    -           Not used
        case 0x4000068:  // 2  R/W  SOUND2CNT_L Channel 2 Duty/Length/Envelope (NR21, NR22)
        case 0x400006A:  //    -    -           Not used
        case 0x400006C:  // 2  R/W  SOUND2CNT_H Channel 2 Frequency/Control    (NR23, NR24)
        case 0x400006E:  //    -    -           Not used
        case 0x4000070:  // 2  R/W  SOUND3CNT_L Channel 3 Stop/Wave RAM select (NR30)
        case 0x4000072:  // 2  R/W  SOUND3CNT_H Channel 3 Length/Volume        (NR31, NR32)
        case 0x4000074:  // 2  R/W  SOUND3CNT_X Channel 3 Frequency/Control    (NR33, NR34)
        case 0x4000076:  //    -    -           Not used
        case 0x4000078:  // 2  R/W  SOUND4CNT_L Channel 4 Length/Envelope      (NR41, NR42)
        case 0x400007A:  //    -    -           Not used
        case 0x400007C:  // 2  R/W  SOUND4CNT_H Channel 4 Frequency/Control    (NR43, NR44)
        case 0x400007E:  //    -    -           Not used
        case 0x4000080:  // 2  R/W  SOUNDCNT_L  Control Stereo/Volume/Enable   (NR50, NR51)
        case 0x4000082:  // 2  R/W  SOUNDCNT_H  Control Mixing/DMA Control
        case 0x4000084:  // 2  R/W  SOUNDCNT_X  Control Sound on/off           (NR52)
        case 0x4000086:  //    -    -           Not used
        case 0x4000088:  // 2  BIOS SOUNDBIAS   Sound PWM Control
        case 0x400008A:  // ..   -    -         Not used
        case 0x4000090:  // 2x10h R/W  WAVE_RAM  Channel 3 Wave Pattern RAM (2 banks!!)
        case 0x40000A0:  // 4    W    FIFO_A    Channel A FIFO, Data 0-3
        case 0x40000A4:  // 4    W    FIFO_B    Channel B FIFO, Data 0-3
        case 0x40000A8:  //      -    -         Not used
        
    // DMA Transfer Channels
        case 0x40000B0:  // 4    W    DMA0SAD   DMA 0 Source Address
        case 0x40000B4:  // 4    W    DMA0DAD   DMA 0 Destination Address
        case 0x40000B8:  // 2    W    DMA0CNT_L DMA 0 Word Count
        case 0x40000BA:  // 2    R/W  DMA0CNT_H DMA 0 Control
        case 0x40000BC:  // 4    W    DMA1SAD   DMA 1 Source Address
        case 0x40000C0:  // 4    W    DMA1DAD   DMA 1 Destination Address
        case 0x40000C4:  // 2    W    DMA1CNT_L DMA 1 Word Count
        case 0x40000C6:  // 2    R/W  DMA1CNT_H DMA 1 Control
        case 0x40000C8:  // 4    W    DMA2SAD   DMA 2 Source Address
        case 0x40000CC:  // 4    W    DMA2DAD   DMA 2 Destination Address
        case 0x40000D0:  // 2    W    DMA2CNT_L DMA 2 Word Count
        case 0x40000D2:  // 2    R/W  DMA2CNT_H DMA 2 Control
        case 0x40000D4:  // 4    W    DMA3SAD   DMA 3 Source Address
        case 0x40000D8:  // 4    W    DMA3DAD   DMA 3 Destination Address
        case 0x40000DC:  // 2    W    DMA3CNT_L DMA 3 Word Count
        case 0x40000DE:  // 2    R/W  DMA3CNT_H DMA 3 Control
        case 0x40000E0:  //      -    -         Not used
        
    // Timer Registers
        case 0x4000100:  // 2    R/W  TM0CNT_L  Timer 0 Counter/Reload
        case 0x4000102:  // 2    R/W  TM0CNT_H  Timer 0 Control
        case 0x4000104:  // 2    R/W  TM1CNT_L  Timer 1 Counter/Reload
        case 0x4000106:  // 2    R/W  TM1CNT_H  Timer 1 Control
        case 0x4000108:  // 2    R/W  TM2CNT_L  Timer 2 Counter/Reload
        case 0x400010A:  // 2    R/W  TM2CNT_H  Timer 2 Control
        case 0x400010C:  // 2    R/W  TM3CNT_L  Timer 3 Counter/Reload
        case 0x400010E:  // 2    R/W  TM3CNT_H  Timer 3 Control
        case 0x4000110:  //      -    -         Not used
        
    // Serial Communication (1)
        case 0x4000120:  // 4    R/W  SIODATA32 SIO Data (Normal-32bit Mode; shared with below)
        // case 0x4000120:  // 2    R/W  SIOMULTI0 SIO Data 0 (Parent)    (Multi-Player Mode)
        case 0x4000122:  // 2    R/W  SIOMULTI1 SIO Data 1 (1st Child) (Multi-Player Mode)
        case 0x4000124:  // 2    R/W  SIOMULTI2 SIO Data 2 (2nd Child) (Multi-Player Mode)
        case 0x4000126:  // 2    R/W  SIOMULTI3 SIO Data 3 (3rd Child) (Multi-Player Mode)
        case 0x4000128:  // 2    R/W  SIOCNT    SIO Control Register
        case 0x400012A:  // 2    R/W  SIOMLT_SEND SIO Data (Local of MultiPlayer; shared below)
        // case 0x400012A:  // 2    R/W  SIODATA8  SIO Data (Normal-8bit and UART Mode)
        case 0x400012C:  //      -    -         Not used
        
    // Keypad Input
        case 0x4000130:  // 2    R    KEYINPUT  Key Status
        case 0x4000132:  // 2    R/W  KEYCNT    Key Interrupt Control
        
    // Serial Communication (2)
        case 0x4000134:  // 2    R/W  RCNT      SIO Mode Select/General Purpose Data
        case 0x4000136:  // -    -    IR        Ancient - Infrared Register (Prototypes only)
        case 0x4000138:  //      -    -         Not used
        case 0x4000140:  // 2    R/W  JOYCNT    SIO JOY Bus Control
        case 0x4000142:  //      -    -         Not used
        case 0x4000150:  // 4    R/W  JOY_RECV  SIO JOY Bus Receive Data
        case 0x4000154:  // 4    R/W  JOY_TRANS SIO JOY Bus Transmit Data
        case 0x4000158:  // 2    R/?  JOYSTAT   SIO JOY Bus Receive Status
        case 0x400015A:  //      -    -         Not used
        
    // Interrupt, Waitstate, and Power-Down Control
        case 0x4000200:  // 2    R/W  IE        Interrupt Enable Register
        case 0x4000202:  // 2    R/W  IF        Interrupt Request Flags / IRQ Acknowledge
        case 0x4000204:  // 2    R/W  WAITCNT   Game Pak Waitstate Control
        case 0x4000206:  //      -    -         Not used
        case 0x4000208:  // 2    R/W  IME       Interrupt Master Enable Register
        case 0x400020A:  //      -    -         Not used
        case 0x4000300:  // 1    R/W  POSTFLG   Undocumented - Post Boot Flag
        case 0x4000301:  // 1    W    HALTCNT   Undocumented - Power Down Control
        case 0x4000302:  //      -    -         Not used
        case 0x4000410:  // ?    ?    ?         Undocumented - Purpose Unknown / Bug ??? 0FFh
        case 0x4000411:  //      -    -         Not used
        case 0x4000800:  // 4    R/W  ?         Undocumented - Internal Memory Control (R/W)
        case 0x4000804:  //      -    -         Not used
        // 4xx0800h  4    R/W  ?         Mirrors of 4000800h (repeated each 64K)
    
        default:
            break;
    }
}

void io_map2(const u32 addr) {
    switch (addr & 0x7FF) {
    // LCD I/O Registers
        case 0x000:  // 2    R/W  DISPCNT   LCD Control
            break;
        case 0x002:  // 2    R/W  -         Undocumented - Green Swap
            break;
        case 0x004:  // 2    R/W  DISPSTAT  General LCD Status (STAT,LYC)
            break;
        case 0x006:  // 2    R    VCOUNT    Vertical Counter (LY)
            break;
        case 0x008:  // 2    R/W  BG0CNT    BG0 Control
            break;
        case 0x00A:  // 2    R/W  BG1CNT    BG1 Control
            break;
        case 0x00C:  // 2    R/W  BG2CNT    BG2 Control
            break;
        case 0x00E:  // 2    R/W  BG3CNT    BG3 Control
            break;
        case 0x010:  // 2    W    BG0HOFS   BG0 X-Offset
            break;
        case 0x012:  // 2    W    BG0VOFS   BG0 Y-Offset
            break;
        case 0x014:  // 2    W    BG1HOFS   BG1 X-Offset
            break;
        case 0x016:  // 2    W    BG1VOFS   BG1 Y-Offset
            break;
        case 0x018:  // 2    W    BG2HOFS   BG2 X-Offset
            break;
        case 0x01A:  // 2    W    BG2VOFS   BG2 Y-Offset
            break;
        case 0x01C:  // 2    W    BG3HOFS   BG3 X-Offset
            break;
        case 0x01E:  // 2    W    BG3VOFS   BG3 Y-Offset
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

} // namespace gba::mem

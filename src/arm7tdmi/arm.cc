#include <stdint.h>
#include <stdio.h>

int test(uint32_t op) {
	switch ((((op >> 16) & 0xFF0) | ((op >> 4) & 0xF)) & 0xFFF) {

// 4.3
		// BX
		case 0x121:
			break;

// 4.4
		// B
		case 0xA00 ... 0xAFF:
			break;
		// BL
		case 0xB00 ... 0xBFF:
			break;

// 4.5
		// AND Logical Left
		case 0x000: case 0x001: case 0x008: case 0x010: case 0x011: case 0x018:
		// AND Logical Right
		case 0x002: case 0x003: case 0x00A: case 0x012: case 0x013: case 0x01A:
		// AND Arithmetic Right
		case 0x004: case 0x005: case 0x00C: case 0x014: case 0x015: case 0x01C:
		// AND Rotate Right
		case 0x006: case 0x007: case 0x00E: case 0x016: case 0x017: case 0x01E:
		// AND Rotate (!S) #Imm
		case 0x200 ... 0x20F:
		// AND Rotate (S) #Imm
		case 0x210 ... 0x21F:
			break;
		// EOR Logical Left
		case 0x020: case 0x021: case 0x028: case 0x030: case 0x031: case 0x038:
		// EOR Logical Right
		case 0x022: case 0x023: case 0x02A: case 0x032: case 0x033: case 0x03A:
		// EOR Arithmetic Right
		case 0x024: case 0x025: case 0x02C: case 0x034: case 0x035: case 0x03C:
		// EOR Rotate Right
		case 0x026: case 0x027: case 0x02E: case 0x036: case 0x037: case 0x03E:
		// EOR Rotate (!S) #Imm
		case 0x220 ... 0x22F:
		// EOR Rotate (S) #Imm
		case 0x230 ... 0x23F:
			break;
		// SUB Logical Left
		case 0x040: case 0x041: case 0x048: case 0x050: case 0x051: case 0x058:
		// SUB Logical Right
		case 0x042: case 0x043: case 0x04A: case 0x052: case 0x053: case 0x05A:
		// SUB Arithmetic Right
		case 0x044: case 0x045: case 0x04C: case 0x054: case 0x055: case 0x05C:
		// SUB Rotate Right
		case 0x046: case 0x047: case 0x04E: case 0x056: case 0x057: case 0x05E:
		// SUB Rotate (!S) #Imm
		case 0x240 ... 0x24F:
		// SUB Rotate (S) #Imm
		case 0x250 ... 0x25F:
			break;
		// RSB Logical Left
		case 0x060: case 0x061: case 0x068: case 0x070: case 0x071: case 0x078:
		// RSB Logical Right
		case 0x062: case 0x063: case 0x06A: case 0x072: case 0x073: case 0x07A:
		// RSB Arithmetic Right
		case 0x064: case 0x065: case 0x06C: case 0x074: case 0x075: case 0x07C:
		// RSB Rotate Right
		case 0x066: case 0x067: case 0x06E: case 0x076: case 0x077: case 0x07E:
		// RSB Rotate (!S) #Imm
		case 0x260 ... 0x26F:
		// RSB Rotate (S) #Imm
		case 0x270 ... 0x27F:
			break;
		// ADD Logical Left
		case 0x080: case 0x081: case 0x088: case 0x090: case 0x091: case 0x098:
		// ADD Logical Right
		case 0x082: case 0x083: case 0x08A: case 0x092: case 0x093: case 0x09A:
		// ADD Arithmetic Right
		case 0x084: case 0x085: case 0x08C: case 0x094: case 0x095: case 0x09C:
		// ADD Rotate Right
		case 0x086: case 0x087: case 0x08E: case 0x096: case 0x097: case 0x09E:
		// ADD Rotate (!S) #Imm
		case 0x280 ... 0x28F:
		// ADD Rotate (S) #Imm
		case 0x290 ... 0x29F:
			break;
		// ADC Logical Left
		case 0x0A0: case 0x0A1: case 0x0A8: case 0x0B0: case 0x0B1: case 0x0B8:
		// ADC Logical Right
		case 0x0A2: case 0x0A3: case 0x0AA: case 0x0B2: case 0x0B3: case 0x0BA:
		// ADC Arithmetic Right
		case 0x0A4: case 0x0A5: case 0x0AC: case 0x0B4: case 0x0B5: case 0x0BC:
		// ADC Rotate Right
		case 0x0A6: case 0x0A7: case 0x0AE: case 0x0B6: case 0x0B7: case 0x0BE:
		// ADC Rotate (!S) #Imm
		case 0x2A0 ... 0x2AF:
		// ADC Rotate (S) #Imm
		case 0x2B0 ... 0x2BF:
			break;
		// SBC Logical Left
		case 0x0C0: case 0x0C1: case 0x0C8: case 0x0D0: case 0x0D1: case 0x0D8:
		// SBC Logical Right
		case 0x0C2: case 0x0C3: case 0x0CA: case 0x0D2: case 0x0D3: case 0x0DA:
		// SBC Arithmetic Right
		case 0x0C4: case 0x0C5: case 0x0CC: case 0x0D4: case 0x0D5: case 0x0DC:
		// SBC Rotate Right
		case 0x0C6: case 0x0C7: case 0x0CE: case 0x0D6: case 0x0D7: case 0x0DE:
		// SBC Rotate (!S) #Imm
		case 0x2C0 ... 0x2CF:
		// SBC Rotate (S) #Imm
		case 0x2D0 ... 0x2DF:
			break;
		// RCS Logical Left
		case 0x0E0: case 0x0E1: case 0x0E8: case 0x0F0: case 0x0F1: case 0x0F8:
		// RCS Logical Right
		case 0x0E2: case 0x0E3: case 0x0EA: case 0x0F2: case 0x0F3: case 0x0FA:
		// RCS Arithmetic Right
		case 0x0E4: case 0x0E5: case 0x0EC: case 0x0F4: case 0x0F5: case 0x0FC:
		// RCS Rotate Right
		case 0x0E6: case 0x0E7: case 0x0EE: case 0x0F6: case 0x0F7: case 0x0FE:
		// RCS Rotate (!S) #Imm
		case 0x2E0 ... 0x2EF:
		// RCS Rotate (S) #Imm
		case 0x2F0 ... 0x2FF:
			break;
		// TST Logical Left
		case 0x110: case 0x111: case 0x118:
		// TST Logical Right
		case 0x112: case 0x113: case 0x11A:
		// TST Arithmetic Right
		case 0x114: case 0x115: case 0x11C:
		// TST Rotate Right
		case 0x116: case 0x117: case 0x11E:
		// TST Rotate (S) #Imm
		case 0x310 ... 0x31F:
			break;
		// TEQ Logical Left
		case 0x130: case 0x131: case 0x138:
		// TEQ Logical Right
		case 0x132: case 0x133: case 0x13A:
		// TEQ Arithmetic Right
		case 0x134: case 0x135: case 0x13C:
		// TEQ Rotate Right
		case 0x136: case 0x137: case 0x13E:
		// TEQ Rotate (S) #Imm
		case 0x330 ... 0x33F:
			break;
		// CMP Logical Left
		case 0x150: case 0x151: case 0x158:
		// CMP Logical Right
		case 0x152: case 0x153: case 0x15A:
		// CMP Arithmetic Right
		case 0x154: case 0x155: case 0x15C:
		// CMP Rotate Right
		case 0x156: case 0x157: case 0x15E:
		// CMP Rotate (S) #Imm
		case 0x350 ... 0x35F:
			break;
		// CMN Logical Left
		case 0x170: case 0x171: case 0x178:
		// CMN Logical Right
		case 0x172: case 0x173: case 0x17A:
		// CMN Arithmetic Right
		case 0x174: case 0x175: case 0x17C:
		// CMN Rotate Right
		case 0x176: case 0x177: case 0x17E:
		// CMN Rotate (S) #Imm
		case 0x370 ... 0x37F:
			break;
		// ORR Logical Left
		case 0x180: case 0x181: case 0x188: case 0x190: case 0x191: case 0x198:
		// ORR Logical Right
		case 0x182: case 0x183: case 0x18A: case 0x192: case 0x193: case 0x19A:
		// ORR Arithmetic Right
		case 0x184: case 0x185: case 0x18C: case 0x194: case 0x195: case 0x19C:
		// ORR Rotate Right
		case 0x186: case 0x187: case 0x18E: case 0x196: case 0x197: case 0x19E:
		// ORR Rotate (!S) #Imm
		case 0x380 ... 0x38F:
		// ORR Rotate (S) #Imm
		case 0x390 ... 0x39F:
			break;
		// MOV Logical Left
		case 0x1A0: case 0x1A1: case 0x1A8: case 0x1B0: case 0x1B1: case 0x1B8:
		// MOV Logical Right
		case 0x1A2: case 0x1A3: case 0x1AA: case 0x1B2: case 0x1B3: case 0x1BA:
		// MOV Arithmetic Right
		case 0x1A4: case 0x1A5: case 0x1AC: case 0x1B4: case 0x1B5: case 0x1BC:
		// MOV Rotate Right
		case 0x1A6: case 0x1A7: case 0x1AE: case 0x1B6: case 0x1B7: case 0x1BE:
		// MOV Rotate (!S) #Imm
		case 0x3A0 ... 0x3AF:
		// MOV Rotate (S) #Imm
		case 0x3B0 ... 0x3BF:
			break;
		// BIC Logical Left
		case 0x1C0: case 0x1C1: case 0x1C8: case 0x1D0: case 0x1D1: case 0x1D8:
		// BIC Logical Right
		case 0x1C2: case 0x1C3: case 0x1CA: case 0x1D2: case 0x1D3: case 0x1DA:
		// BIC Arithmetic Right
		case 0x1C4: case 0x1C5: case 0x1CC: case 0x1D4: case 0x1D5: case 0x1DC:
		// BIC Rotate Right
		case 0x1C6: case 0x1C7: case 0x1CE: case 0x1D6: case 0x1D7: case 0x1DE:
		// BIC Rotate (!S) #Imm
		case 0x3C0 ... 0x3CF:
		// BIC Rotate (S) #Imm
		case 0x3D0 ... 0x3DF:
			break;
		// MVN Logical Left
		case 0x1E0: case 0x1E1: case 0x1E8: case 0x1F0: case 0x1F1: case 0x1F8:
		// MVN Logical Right
		case 0x1E2: case 0x1E3: case 0x1EA: case 0x1F2: case 0x1F3: case 0x1FA:
		// MVN Arithmetic Right
		case 0x1E4: case 0x1E5: case 0x1EC: case 0x1F4: case 0x1F5: case 0x1FC:
		// MVN Rotate Right
		case 0x1E6: case 0x1E7: case 0x1EE: case 0x1F6: case 0x1F7: case 0x1FE:
		// MVN Rotate (!S) #Imm
		case 0x3E0 ... 0x3EF:
		// MVN Rotate (S) #Imm
		case 0x3F0 ... 0x3FF:
			break;

// 4.6
		// MRS (transfer PSR contents to a register) CPSR
		case 0x100:
		// MRS (transfer PSR contents to a register) SPSR
		case 0x140:
			break;
		// MSR (transfer register contents to PSR) CPSR
		case 0x120:
		// MSR (transfer register contents to PSR) SPSR
		case 0x160:
			break;
		// MSR (transfer register contents or immdiate value to PSR flag bits only) CPSR
		case 0x320 ... 0x32F:
			break;
		// MSR (transfer register contents or immdiate value to PSR flag bits only) SPSR
		case 0x360 ... 0x36F:
			break;

// 4.7
		// MUL !S
		case 0x009:
			break;
		// MUL 1S
		case 0x019:
			break;
		// MLA !S
		case 0x029:
			break;
		// MLA S
		case 0x039:
			break;

// 4.8
		// UMULL !S
		case 0x089:
			break;
		// UMULL S
		case 0x099:
			break;
		// SMULL !S
		case 0x0C9:
			break;
		// SMULL S
		case 0x0D9:
			break;
		// UMLAL !S
		case 0x0A9:
			break;
		// UMLAL S
		case 0x0B9:
			break;
		// SMLAL !S
		case 0x0E9:
			break;
		// SMLAL S
		case 0x0F9:
			break;

// 4.9
		// STR #Imm
		case 0x400 ... 0x40F:
			break;
		// STR #Imm
		case 0x420 ... 0x42F:
			break;
		// STR #Imm
		case 0x440 ... 0x44F:
			break;
		// STR #Imm
		case 0x460 ... 0x46F:
			break;
		// STR #Imm
		case 0x480 ... 0x48F:
			break;
		// STR #Imm
		case 0x4A0 ... 0x4AF:
			break;
		// STR #Imm
		case 0x4C0 ... 0x4CF:
			break;
		// STR #Imm
		case 0x4E0 ... 0x4EF:
			break;
		// STR #Imm
		case 0x500 ... 0x50F:
			break;
		// STR #Imm
		case 0x520 ... 0x52F:
			break;
		// STR #Imm
		case 0x540 ... 0x54F:
			break;
		// STR #Imm
		case 0x560 ... 0x56F:
			break;
		// STR #Imm
		case 0x580 ... 0x58F:
			break;
		// STR #Imm
		case 0x5A0 ... 0x5AF:
			break;
		// STR #Imm
		case 0x5C0 ... 0x5CF:
			break;
		// STR #Imm
		case 0x5E0 ... 0x5EF:
			break;
		// STR Logical Left
		case 0x600: case 0x608:
			break;
		// STR Logical Left
		case 0x620: case 0x628:
			break;
		// STR Logical Left
		case 0x640: case 0x648:
			break;
		// STR Logical Left
		case 0x660: case 0x668:
			break;
		// STR Logical Left
		case 0x680: case 0x688:
			break;
		// STR Logical Left
		case 0x6A0: case 0x6A8:
			break;
		// STR Logical Left
		case 0x6C0: case 0x6C8:
			break;
		// STR Logical Left
		case 0x6E0: case 0x6E8:
			break;
		// STR Logical Left
		case 0x700: case 0x708:
			break;
		// STR Logical Left
		case 0x720: case 0x728:
			break;
		// STR Logical Left
		case 0x740: case 0x748:
			break;
		// STR Logical Left
		case 0x760: case 0x768:
			break;
		// STR Logical Left
		case 0x780: case 0x788:
			break;
		// STR Logical Left
		case 0x7A0: case 0x7A8:
			break;
		// STR Logical Left
		case 0x7C0: case 0x7C8:
			break;
		// STR Logical Left
		case 0x7E0: case 0x7E8:
			break;
		// STR Logical Right
		case 0x602: case 0x60A:
			break;
		// STR Logical Right
		case 0x622: case 0x62A:
			break;
		// STR Logical Right
		case 0x642: case 0x64A:
			break;
		// STR Logical Right
		case 0x662: case 0x66A:
			break;
		// STR Logical Right
		case 0x682: case 0x68A:
			break;
		// STR Logical Right
		case 0x6A2: case 0x6AA:
			break;
		// STR Logical Right
		case 0x6C2: case 0x6CA:
			break;
		// STR Logical Right
		case 0x6E2: case 0x6EA:
			break;
		// STR Logical Right
		case 0x702: case 0x70A:
			break;
		// STR Logical Right
		case 0x722: case 0x72A:
			break;
		// STR Logical Right
		case 0x742: case 0x74A:
			break;
		// STR Logical Right
		case 0x762: case 0x76A:
			break;
		// STR Logical Right
		case 0x782: case 0x78A:
			break;
		// STR Logical Right
		case 0x7A2: case 0x7AA:
			break;
		// STR Logical Right
		case 0x7C2: case 0x7CA:
			break;
		// STR Logical Right
		case 0x7E2: case 0x7EA:
			break;
		// STR Arithmetic Right
		case 0x604: case 0x60C:
			break;
		// STR Arithmetic Right
		case 0x624: case 0x62C:
			break;
		// STR Arithmetic Right
		case 0x644: case 0x64C:
			break;
		// STR Arithmetic Right
		case 0x664: case 0x66C:
			break;
		// STR Arithmetic Right
		case 0x684: case 0x68C:
			break;
		// STR Arithmetic Right
		case 0x6A4: case 0x6AC:
			break;
		// STR Arithmetic Right
		case 0x6C4: case 0x6CC:
			break;
		// STR Arithmetic Right
		case 0x6E4: case 0x6EC:
			break;
		// STR Arithmetic Right
		case 0x704: case 0x70C:
			break;
		// STR Arithmetic Right
		case 0x724: case 0x72C:
			break;
		// STR Arithmetic Right
		case 0x744: case 0x74C:
			break;
		// STR Arithmetic Right
		case 0x764: case 0x76C:
			break;
		// STR Arithmetic Right
		case 0x784: case 0x78C:
			break;
		// STR Arithmetic Right
		case 0x7A4: case 0x7AC:
			break;
		// STR Arithmetic Right
		case 0x7C4: case 0x7CC:
			break;
		// STR Arithmetic Right
		case 0x7E4: case 0x7EC:
			break;
		// STR Rotate Right
		case 0x606: case 0x60E:
			break;
		// STR Rotate Right
		case 0x626: case 0x62E:
			break;
		// STR Rotate Right
		case 0x646: case 0x64E:
			break;
		// STR Rotate Right
		case 0x666: case 0x66E:
			break;
		// STR Rotate Right
		case 0x686: case 0x68E:
			break;
		// STR Rotate Right
		case 0x6A6: case 0x6AE:
			break;
		// STR Rotate Right
		case 0x6C6: case 0x6CE:
			break;
		// STR Rotate Right
		case 0x6E6: case 0x6EE:
			break;
		// STR Rotate Right
		case 0x706: case 0x70E:
			break;
		// STR Rotate Right
		case 0x726: case 0x72E:
			break;
		// STR Rotate Right
		case 0x746: case 0x74E:
			break;
		// STR Rotate Right
		case 0x766: case 0x76E:
			break;
		// STR Rotate Right
		case 0x786: case 0x78E:
			break;
		// STR Rotate Right
		case 0x7A6: case 0x7AE:
			break;
		// STR Rotate Right
		case 0x7C6: case 0x7CE:
			break;
		// STR Rotate Right
		case 0x7E6: case 0x7EE:
			break;
		// LDR #Imm
		case 0x410 ... 0x41F:
			break;
		// LDR #Imm
		case 0x430 ... 0x43F:
			break;
		// LDR #Imm
		case 0x450 ... 0x45F:
			break;
		// LDR #Imm
		case 0x470 ... 0x47F:
			break;
		// LDR #Imm
		case 0x490 ... 0x49F:
			break;
		// LDR #Imm
		case 0x4B0 ... 0x4BF:
			break;
		// LDR #Imm
		case 0x4D0 ... 0x4DF:
			break;
		// LDR #Imm
		case 0x4F0 ... 0x4FF:
			break;
		// LDR #Imm
		case 0x510 ... 0x51F:
			break;
		// LDR #Imm
		case 0x530 ... 0x53F:
			break;
		// LDR #Imm
		case 0x550 ... 0x55F:
			break;
		// LDR #Imm
		case 0x570 ... 0x57F:
			break;
		// LDR #Imm
		case 0x590 ... 0x59F:
			break;
		// LDR #Imm
		case 0x5B0 ... 0x5BF:
			break;
		// LDR #Imm
		case 0x5D0 ... 0x5DF:
			break;
		// LDR #Imm
		case 0x5F0 ... 0x5FF:
			break;
		// LDR Logical Left
		case 0x610: case 0x618:
			break;
		// LDR Logical Left
		case 0x630: case 0x638:
			break;
		// LDR Logical Left
		case 0x650: case 0x658:
			break;
		// LDR Logical Left
		case 0x670: case 0x678:
			break;
		// LDR Logical Left
		case 0x690: case 0x698:
			break;
		// LDR Logical Left
		case 0x6B0: case 0x6B8:
			break;
		// LDR Logical Left
		case 0x6D0: case 0x6D8:
			break;
		// LDR Logical Left
		case 0x6F0: case 0x6F8:
			break;
		// LDR Logical Left
		case 0x710: case 0x718:
			break;
		// LDR Logical Left
		case 0x730: case 0x738:
			break;
		// LDR Logical Left
		case 0x750: case 0x758:
			break;
		// LDR Logical Left
		case 0x770: case 0x778:
			break;
		// LDR Logical Left
		case 0x790: case 0x798:
			break;
		// LDR Logical Left
		case 0x7B0: case 0x7B8:
			break;
		// LDR Logical Left
		case 0x7D0: case 0x7D8:
			break;
		// LDR Logical Left
		case 0x7F0: case 0x7F8:
			break;
		// LDR Logical Right
		case 0x612: case 0x61A:
			break;
		// LDR Logical Right
		case 0x632: case 0x63A:
			break;
		// LDR Logical Right
		case 0x652: case 0x65A:
			break;
		// LDR Logical Right
		case 0x672: case 0x67A:
			break;
		// LDR Logical Right
		case 0x692: case 0x69A:
			break;
		// LDR Logical Right
		case 0x6B2: case 0x6BA:
			break;
		// LDR Logical Right
		case 0x6D2: case 0x6DA:
			break;
		// LDR Logical Right
		case 0x6F2: case 0x6FA:
			break;
		// LDR Logical Right
		case 0x712: case 0x71A:
			break;
		// LDR Logical Right
		case 0x732: case 0x73A:
			break;
		// LDR Logical Right
		case 0x752: case 0x75A:
			break;
		// LDR Logical Right
		case 0x772: case 0x77A:
			break;
		// LDR Logical Right
		case 0x792: case 0x79A:
			break;
		// LDR Logical Right
		case 0x7B2: case 0x7BA:
			break;
		// LDR Logical Right
		case 0x7D2: case 0x7DA:
			break;
		// LDR Logical Right
		case 0x7F2: case 0x7FA:
			break;
		// LDR Arithmetic Right
		case 0x614: case 0x61C:
			break;
		// LDR Arithmetic Right
		case 0x634: case 0x63C:
			break;
		// LDR Arithmetic Right
		case 0x654: case 0x65C:
			break;
		// LDR Arithmetic Right
		case 0x674: case 0x67C:
			break;
		// LDR Arithmetic Right
		case 0x694: case 0x69C:
			break;
		// LDR Arithmetic Right
		case 0x6B4: case 0x6BC:
			break;
		// LDR Arithmetic Right
		case 0x6D4: case 0x6DC:
			break;
		// LDR Arithmetic Right
		case 0x6F4: case 0x6FC:
			break;
		// LDR Arithmetic Right
		case 0x714: case 0x71C:
			break;
		// LDR Arithmetic Right
		case 0x734: case 0x73C:
			break;
		// LDR Arithmetic Right
		case 0x754: case 0x75C:
			break;
		// LDR Arithmetic Right
		case 0x774: case 0x77C:
			break;
		// LDR Arithmetic Right
		case 0x794: case 0x79C:
			break;
		// LDR Arithmetic Right
		case 0x7B4: case 0x7BC:
			break;
		// LDR Arithmetic Right
		case 0x7D4: case 0x7DC:
			break;
		// LDR Arithmetic Right
		case 0x7F4: case 0x7FC:
			break;
		// LDR Rotate Right
		case 0x616: case 0x61E:
			break;
		// LDR Rotate Right
		case 0x636: case 0x63E:
			break;
		// LDR Rotate Right
		case 0x656: case 0x65E:
			break;
		// LDR Rotate Right
		case 0x676: case 0x67E:
			break;
		// LDR Rotate Right
		case 0x696: case 0x69E:
			break;
		// LDR Rotate Right
		case 0x6B6: case 0x6BE:
			break;
		// LDR Rotate Right
		case 0x6D6: case 0x6DE:
			break;
		// LDR Rotate Right
		case 0x6F6: case 0x6FE:
			break;
		// LDR Rotate Right
		case 0x716: case 0x71E:
			break;
		// LDR Rotate Right
		case 0x736: case 0x73E:
			break;
		// LDR Rotate Right
		case 0x756: case 0x75E:
			break;
		// LDR Rotate Right
		case 0x776: case 0x77E:
			break;
		// LDR Rotate Right
		case 0x796: case 0x79E:
			break;
		// LDR Rotate Right
		case 0x7B6: case 0x7BE:
			break;
		// LDR Rotate Right
		case 0x7D6: case 0x7DE:
			break;
		// LDR Rotate Right
		case 0x7F6: case 0x7FE:
			break;

// 4.10
		// STR Reg Unsigned halfwords
		case 0x00B:
			break;
		// STR Reg Unsigned halfwords
		case 0x02B:
			break;
		// STR Reg Unsigned halfwords
		case 0x08B:
			break;
		// STR Reg Unsigned halfwords
		case 0x0AB:
			break;
		// STR Reg Unsigned halfwords
		case 0x10B:
			break;
		// STR Reg Unsigned halfwords
		case 0x12B:
			break;
		// STR Reg Unsigned halfwords
		case 0x18B:
			break;
		// STR Reg Unsigned halfwords
		case 0x1AB:
			break;
		// STR Reg Signed byte
		case 0x00D:
			break;
		// STR Reg Signed byte
		case 0x02D:
			break;
		// STR Reg Signed byte
		case 0x08D:
			break;
		// STR Reg Signed byte
		case 0x0AD:
			break;
		// STR Reg Signed byte
		case 0x10D:
			break;
		// STR Reg Signed byte
		case 0x12D:
			break;
		// STR Reg Signed byte
		case 0x18D:
			break;
		// STR Reg Signed byte
		case 0x1AD:
			break;
		// STR Reg Signed halfwords
		case 0x00F:
			break;
		// STR Reg Signed halfwords
		case 0x02F:
			break;
		// STR Reg Signed halfwords
		case 0x08F:
			break;
		// STR Reg Signed halfwords
		case 0x0AF:
			break;
		// STR Reg Signed halfwords
		case 0x10F:
			break;
		// STR Reg Signed halfwords
		case 0x12F:
			break;
		// STR Reg Signed halfwords
		case 0x18F:
			break;
		// STR Reg Signed halfwords
		case 0x1AF:
			break;
		// STR #Imm Unsigned halfwords
		case 0x04B:
			break;
		// STR #Imm Unsigned halfwords
		case 0x06B:
			break;
		// STR #Imm Unsigned halfwords
		case 0x0CB:
			break;
		// STR #Imm Unsigned halfwords
		case 0x0EB:
			break;
		// STR #Imm Unsigned halfwords
		case 0x14B:
			break;
		// STR #Imm Unsigned halfwords
		case 0x16B:
			break;
		// STR #Imm Unsigned halfwords
		case 0x1CB:
			break;
		// STR #Imm Unsigned halfwords
		case 0x1EB:
			break;
		// STR #Imm Signed byte
		case 0x04D:
			break;
		// STR #Imm Signed byte
		case 0x06D:
			break;
		// STR #Imm Signed byte
		case 0x0CD:
			break;
		// STR #Imm Signed byte
		case 0x0ED:
			break;
		// STR #Imm Signed byte
		case 0x14D:
			break;
		// STR #Imm Signed byte
		case 0x16D:
			break;
		// STR #Imm Signed byte
		case 0x1CD:
			break;
		// STR #Imm Signed byte
		case 0x1ED:
			break;
		// STR #Imm Signed halfwords
		case 0x04F:
			break;
		// STR #Imm Signed halfwords
		case 0x06F:
			break;
		// STR #Imm Signed halfwords
		case 0x0CF:
			break;
		// STR #Imm Signed halfwords
		case 0x0EF:
			break;
		// STR #Imm Signed halfwords
		case 0x14F:
			break;
		// STR #Imm Signed halfwords
		case 0x16F:
			break;
		// STR #Imm Signed halfwords
		case 0x1CF:
			break;
		// STR #Imm Signed halfwords
		case 0x1EF:
			break;
		// LDR Reg Unsigned halfwords
		case 0x01B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x03B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x09B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x0BB:
			break;
		// LDR Reg Unsigned halfwords
		case 0x11B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x13B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x19B:
			break;
		// LDR Reg Unsigned halfwords
		case 0x1BB:
			break;
		// LDR Reg Signed byte
		case 0x01D:
			break;
		// LDR Reg Signed byte
		case 0x03D:
			break;
		// LDR Reg Signed byte
		case 0x09D:
			break;
		// LDR Reg Signed byte
		case 0x0BD:
			break;
		// LDR Reg Signed byte
		case 0x11D:
			break;
		// LDR Reg Signed byte
		case 0x13D:
			break;
		// LDR Reg Signed byte
		case 0x19D:
			break;
		// LDR Reg Signed byte
		case 0x1BD:
			break;
		// LDR Reg Signed halfwords
		case 0x01F:
			break;
		// LDR Reg Signed halfwords
		case 0x03F:
			break;
		// LDR Reg Signed halfwords
		case 0x09F:
			break;
		// LDR Reg Signed halfwords
		case 0x0BF:
			break;
		// LDR Reg Signed halfwords
		case 0x11F:
			break;
		// LDR Reg Signed halfwords
		case 0x13F:
			break;
		// LDR Reg Signed halfwords
		case 0x19F:
			break;
		// LDR Reg Signed halfwords
		case 0x1BF:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x05B:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x07B:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x0DB:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x0FB:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x15B:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x17B:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x1DB:
			break;
		// LDR #Imm Unsigned halfwords
		case 0x1FB:
			break;
		// LDR #Imm Signed byte
		case 0x05D:
			break;
		// LDR #Imm Signed byte
		case 0x07D:
			break;
		// LDR #Imm Signed byte
		case 0x0DD:
			break;
		// LDR #Imm Signed byte
		case 0x0FD:
			break;
		// LDR #Imm Signed byte
		case 0x15D:
			break;
		// LDR #Imm Signed byte
		case 0x17D:
			break;
		// LDR #Imm Signed byte
		case 0x1DD:
			break;
		// LDR #Imm Signed byte
		case 0x1FD:
			break;
		// LDR #Imm Signed halfwords
		case 0x05F:
			break;
		// LDR #Imm Signed halfwords
		case 0x07F:
			break;
		// LDR #Imm Signed halfwords
		case 0x0DF:
			break;
		// LDR #Imm Signed halfwords
		case 0x0FF:
			break;
		// LDR #Imm Signed halfwords
		case 0x15F:
			break;
		// LDR #Imm Signed halfwords
		case 0x17F:
			break;
		// LDR #Imm Signed halfwords
		case 0x1DF:
			break;
		// LDR #Imm Signed halfwords
		case 0x1FF:
			break;

// 4.11
		// STM
		case 0x800 ... 0x80F:
			break;
		// STM
		case 0x820 ... 0x82F:
			break;
		// STM
		case 0x840 ... 0x84F:
			break;
		// STM
		case 0x860 ... 0x86F:
			break;
		// STM
		case 0x880 ... 0x88F:
			break;
		// STM
		case 0x8A0 ... 0x8AF:
			break;
		// STM
		case 0x8C0 ... 0x8CF:
			break;
		// STM
		case 0x8E0 ... 0x8EF:
			break;
		// STM
		case 0x900 ... 0x90F:
			break;
		// STM
		case 0x920 ... 0x92F:
			break;
		// STM
		case 0x940 ... 0x94F:
			break;
		// STM
		case 0x960 ... 0x96F:
			break;
		// STM
		case 0x980 ... 0x98F:
			break;
		// STM
		case 0x9A0 ... 0x9AF:
			break;
		// STM
		case 0x9C0 ... 0x9CF:
			break;
		// STM
		case 0x9E0 ... 0x9EF:
			break;
		// LDM
		case 0x810 ... 0x81F:
			break;
		// LDM
		case 0x830 ... 0x83F:
			break;
		// LDM
		case 0x850 ... 0x85F:
			break;
		// LDM
		case 0x870 ... 0x87F:
			break;
		// LDM
		case 0x890 ... 0x89F:
			break;
		// LDM
		case 0x8B0 ... 0x8BF:
			break;
		// LDM
		case 0x8D0 ... 0x8DF:
			break;
		// LDM
		case 0x8F0 ... 0x8FF:
			break;
		// LDM
		case 0x910 ... 0x91F:
			break;
		// LDM
		case 0x930 ... 0x93F:
			break;
		// LDM
		case 0x950 ... 0x95F:
			break;
		// LDM
		case 0x970 ... 0x97F:
			break;
		// LDM
		case 0x990 ... 0x99F:
			break;
		// LDM
		case 0x9B0 ... 0x9BF:
			break;
		// LDM
		case 0x9D0 ... 0x9DF:
			break;
		// LDM
		case 0x9F0 ... 0x9FF:
			break;

// 4.12
		// SWP
		case 0x109:
			break;
		// SWPB
		case 0x149:
			break;

// 4.13
		// SWI
		case 0xF00 ... 0xFFF:
			break;

// 4.14
		// CDP
		case 0xE00: case 0xE02: case 0xE04: case 0xE06: case 0xE08: case 0xE0A: case 0xE0C: case 0xE0E: 
		case 0xE10: case 0xE12: case 0xE14: case 0xE16: case 0xE18: case 0xE1A: case 0xE1C: case 0xE1E: 
		case 0xE20: case 0xE22: case 0xE24: case 0xE26: case 0xE28: case 0xE2A: case 0xE2C: case 0xE2E: 
		case 0xE30: case 0xE32: case 0xE34: case 0xE36: case 0xE38: case 0xE3A: case 0xE3C: case 0xE3E: 
		case 0xE40: case 0xE42: case 0xE44: case 0xE46: case 0xE48: case 0xE4A: case 0xE4C: case 0xE4E: 
		case 0xE50: case 0xE52: case 0xE54: case 0xE56: case 0xE58: case 0xE5A: case 0xE5C: case 0xE5E: 
		case 0xE60: case 0xE62: case 0xE64: case 0xE66: case 0xE68: case 0xE6A: case 0xE6C: case 0xE6E: 
		case 0xE70: case 0xE72: case 0xE74: case 0xE76: case 0xE78: case 0xE7A: case 0xE7C: case 0xE7E: 
		case 0xE80: case 0xE82: case 0xE84: case 0xE86: case 0xE88: case 0xE8A: case 0xE8C: case 0xE8E: 
		case 0xE90: case 0xE92: case 0xE94: case 0xE96: case 0xE98: case 0xE9A: case 0xE9C: case 0xE9E: 
		case 0xEA0: case 0xEA2: case 0xEA4: case 0xEA6: case 0xEA8: case 0xEAA: case 0xEAC: case 0xEAE: 
		case 0xEB0: case 0xEB2: case 0xEB4: case 0xEB6: case 0xEB8: case 0xEBA: case 0xEBC: case 0xEBE: 
		case 0xEC0: case 0xEC2: case 0xEC4: case 0xEC6: case 0xEC8: case 0xECA: case 0xECC: case 0xECE: 
		case 0xED0: case 0xED2: case 0xED4: case 0xED6: case 0xED8: case 0xEDA: case 0xEDC: case 0xEDE: 
		case 0xEE0: case 0xEE2: case 0xEE4: case 0xEE6: case 0xEE8: case 0xEEA: case 0xEEC: case 0xEEE: 
		case 0xEF0: case 0xEF2: case 0xEF4: case 0xEF6: case 0xEF8: case 0xEFA: case 0xEFC: case 0xEFE: 
					break;

// 4.15
		// STC
		case 0xC00 ... 0xC0F:
			break;
		// STC
		case 0xC20 ... 0xC2F:
			break;
		// STC
		case 0xC40 ... 0xC4F:
			break;
		// STC
		case 0xC60 ... 0xC6F:
			break;
		// STC
		case 0xC80 ... 0xC8F:
			break;
		// STC
		case 0xCA0 ... 0xCAF:
			break;
		// STC
		case 0xCC0 ... 0xCCF:
			break;
		// STC
		case 0xCE0 ... 0xCEF:
			break;
		// STC
		case 0xD00 ... 0xD0F:
			break;
		// STC
		case 0xD20 ... 0xD2F:
			break;
		// STC
		case 0xD40 ... 0xD4F:
			break;
		// STC
		case 0xD60 ... 0xD6F:
			break;
		// STC
		case 0xD80 ... 0xD8F:
			break;
		// STC
		case 0xDA0 ... 0xDAF:
			break;
		// STC
		case 0xDC0 ... 0xDCF:
			break;
		// STC
		case 0xDE0 ... 0xDEF:
			break;
		// LDC
		case 0xC10 ... 0xC1F:
			break;
		// LDC
		case 0xC30 ... 0xC3F:
			break;
		// LDC
		case 0xC50 ... 0xC5F:
			break;
		// LDC
		case 0xC70 ... 0xC7F:
			break;
		// LDC
		case 0xC90 ... 0xC9F:
			break;
		// LDC
		case 0xCB0 ... 0xCBF:
			break;
		// LDC
		case 0xCD0 ... 0xCDF:
			break;
		// LDC
		case 0xCF0 ... 0xCFF:
			break;
		// LDC
		case 0xD10 ... 0xD1F:
			break;
		// LDC
		case 0xD30 ... 0xD3F:
			break;
		// LDC
		case 0xD50 ... 0xD5F:
			break;
		// LDC
		case 0xD70 ... 0xD7F:
			break;
		// LDC
		case 0xD90 ... 0xD9F:
			break;
		// LDC
		case 0xDB0 ... 0xDBF:
			break;
		// LDC
		case 0xDD0 ... 0xDDF:
			break;
		// LDC
		case 0xDF0 ... 0xDFF:
			break;

// 4.16
		// MCR
		case 0xE01: case 0xE03: case 0xE05: case 0xE07: case 0xE09: case 0xE0B: case 0xE0D: case 0xE0F: 
		case 0xE21: case 0xE23: case 0xE25: case 0xE27: case 0xE29: case 0xE2B: case 0xE2D: case 0xE2F: 
		case 0xE41: case 0xE43: case 0xE45: case 0xE47: case 0xE49: case 0xE4B: case 0xE4D: case 0xE4F: 
		case 0xE61: case 0xE63: case 0xE65: case 0xE67: case 0xE69: case 0xE6B: case 0xE6D: case 0xE6F: 
		case 0xE81: case 0xE83: case 0xE85: case 0xE87: case 0xE89: case 0xE8B: case 0xE8D: case 0xE8F: 
		case 0xEA1: case 0xEA3: case 0xEA5: case 0xEA7: case 0xEA9: case 0xEAB: case 0xEAD: case 0xEAF: 
		case 0xEC1: case 0xEC3: case 0xEC5: case 0xEC7: case 0xEC9: case 0xECB: case 0xECD: case 0xECF: 
		case 0xEE1: case 0xEE3: case 0xEE5: case 0xEE7: case 0xEE9: case 0xEEB: case 0xEED: case 0xEEF: 
					break;
		// MRC
		case 0xE11: case 0xE13: case 0xE15: case 0xE17: case 0xE19: case 0xE1B: case 0xE1D: case 0xE1F: 
		case 0xE31: case 0xE33: case 0xE35: case 0xE37: case 0xE39: case 0xE3B: case 0xE3D: case 0xE3F: 
		case 0xE51: case 0xE53: case 0xE55: case 0xE57: case 0xE59: case 0xE5B: case 0xE5D: case 0xE5F: 
		case 0xE71: case 0xE73: case 0xE75: case 0xE77: case 0xE79: case 0xE7B: case 0xE7D: case 0xE7F: 
		case 0xE91: case 0xE93: case 0xE95: case 0xE97: case 0xE99: case 0xE9B: case 0xE9D: case 0xE9F: 
		case 0xEB1: case 0xEB3: case 0xEB5: case 0xEB7: case 0xEB9: case 0xEBB: case 0xEBD: case 0xEBF: 
		case 0xED1: case 0xED3: case 0xED5: case 0xED7: case 0xED9: case 0xEDB: case 0xEDD: case 0xEDF: 
		case 0xEF1: case 0xEF3: case 0xEF5: case 0xEF7: case 0xEF9: case 0xEFB: case 0xEFD: case 0xEFF: 
					break;

// 4.17
		// UNDEFINED INSTRUCTION
		case 0x601: case 0x603: case 0x605: case 0x607: case 0x609: case 0x60B: case 0x60D: case 0x60F: 
		case 0x611: case 0x613: case 0x615: case 0x617: case 0x619: case 0x61B: case 0x61D: case 0x61F: 
		case 0x621: case 0x623: case 0x625: case 0x627: case 0x629: case 0x62B: case 0x62D: case 0x62F: 
		case 0x631: case 0x633: case 0x635: case 0x637: case 0x639: case 0x63B: case 0x63D: case 0x63F: 
		case 0x641: case 0x643: case 0x645: case 0x647: case 0x649: case 0x64B: case 0x64D: case 0x64F: 
		case 0x651: case 0x653: case 0x655: case 0x657: case 0x659: case 0x65B: case 0x65D: case 0x65F: 
		case 0x661: case 0x663: case 0x665: case 0x667: case 0x669: case 0x66B: case 0x66D: case 0x66F: 
		case 0x671: case 0x673: case 0x675: case 0x677: case 0x679: case 0x67B: case 0x67D: case 0x67F: 
		case 0x681: case 0x683: case 0x685: case 0x687: case 0x689: case 0x68B: case 0x68D: case 0x68F: 
		case 0x691: case 0x693: case 0x695: case 0x697: case 0x699: case 0x69B: case 0x69D: case 0x69F: 
		case 0x6A1: case 0x6A3: case 0x6A5: case 0x6A7: case 0x6A9: case 0x6AB: case 0x6AD: case 0x6AF: 
		case 0x6B1: case 0x6B3: case 0x6B5: case 0x6B7: case 0x6B9: case 0x6BB: case 0x6BD: case 0x6BF: 
		case 0x6C1: case 0x6C3: case 0x6C5: case 0x6C7: case 0x6C9: case 0x6CB: case 0x6CD: case 0x6CF: 
		case 0x6D1: case 0x6D3: case 0x6D5: case 0x6D7: case 0x6D9: case 0x6DB: case 0x6DD: case 0x6DF: 
		case 0x6E1: case 0x6E3: case 0x6E5: case 0x6E7: case 0x6E9: case 0x6EB: case 0x6ED: case 0x6EF: 
		case 0x6F1: case 0x6F3: case 0x6F5: case 0x6F7: case 0x6F9: case 0x6FB: case 0x6FD: case 0x6FF: 
		case 0x701: case 0x703: case 0x705: case 0x707: case 0x709: case 0x70B: case 0x70D: case 0x70F: 
		case 0x711: case 0x713: case 0x715: case 0x717: case 0x719: case 0x71B: case 0x71D: case 0x71F: 
		case 0x721: case 0x723: case 0x725: case 0x727: case 0x729: case 0x72B: case 0x72D: case 0x72F: 
		case 0x731: case 0x733: case 0x735: case 0x737: case 0x739: case 0x73B: case 0x73D: case 0x73F: 
		case 0x741: case 0x743: case 0x745: case 0x747: case 0x749: case 0x74B: case 0x74D: case 0x74F: 
		case 0x751: case 0x753: case 0x755: case 0x757: case 0x759: case 0x75B: case 0x75D: case 0x75F: 
		case 0x761: case 0x763: case 0x765: case 0x767: case 0x769: case 0x76B: case 0x76D: case 0x76F: 
		case 0x771: case 0x773: case 0x775: case 0x777: case 0x779: case 0x77B: case 0x77D: case 0x77F: 
		case 0x781: case 0x783: case 0x785: case 0x787: case 0x789: case 0x78B: case 0x78D: case 0x78F: 
		case 0x791: case 0x793: case 0x795: case 0x797: case 0x799: case 0x79B: case 0x79D: case 0x79F: 
		case 0x7A1: case 0x7A3: case 0x7A5: case 0x7A7: case 0x7A9: case 0x7AB: case 0x7AD: case 0x7AF: 
		case 0x7B1: case 0x7B3: case 0x7B5: case 0x7B7: case 0x7B9: case 0x7BB: case 0x7BD: case 0x7BF: 
		case 0x7C1: case 0x7C3: case 0x7C5: case 0x7C7: case 0x7C9: case 0x7CB: case 0x7CD: case 0x7CF: 
		case 0x7D1: case 0x7D3: case 0x7D5: case 0x7D7: case 0x7D9: case 0x7DB: case 0x7DD: case 0x7DF: 
		case 0x7E1: case 0x7E3: case 0x7E5: case 0x7E7: case 0x7E9: case 0x7EB: case 0x7ED: case 0x7EF: 
		case 0x7F1: case 0x7F3: case 0x7F5: case 0x7F7: case 0x7F9: case 0x7FB: case 0x7FD: case 0x7FF: 
					break;

default:
	return -1;

	}
return 0;
}

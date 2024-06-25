#include "common.h"

// extend the sign of 32 bit number to 64 bit
// get the sign at bit 31 of data, then set
// all bits > 31 to this value
uint64_t signExtend(uint32_t data) {
	return data | ((((uint64_t) msb32(data) << 32) - 1) << 32);
}

// get bit n of 64-bit number
uint64_t getBit64(uint64_t data, uint8_t n) {
	return (data & ((uint64_t) 1 << n)) >> n;
}

// get bit n of 32-bit number
uint32_t getBit32(uint32_t data, uint8_t n) {
	return (data & ((uint32_t) 1 << n)) >> n;
}

// get msb of 64-bit number
uint64_t msb64(uint64_t data) {
	return getBit64(data, sizeof(uint64_t) * 8 - 1);
}

// get msb of 32-bit number
uint32_t msb32(uint32_t data) {
	return getBit32(data, sizeof(uint32_t) * 8 - 1);
}

// get bit field of n bits starting at offset
uint64_t getBitField64(uint64_t data, uint8_t offset, uint8_t n) {
	assert(offset + n <= sizeof(data) * 8);
	return (data & ((((uint64_t) 1 << n) - 1) << offset)) >> offset;
}

// get bit field of n bits starting at offset
uint32_t getBitField32(uint32_t data, uint8_t offset, uint8_t n) {
	assert(offset + n <= sizeof(data) * 8);
	return (data & ((((uint32_t) 1 << n) - 1) << offset)) >> offset;
}

// write 32 bit data to register, write 0 to upper 32 bits
bool writeToReg32(uint32_t reg, uint32_t data) {
	if (reg >= NUM_GEN_REGS) {
		return false;
	}
	cpu.regs[reg].d_32[0] = data;
	cpu.regs[reg].d_32[1] = 0;
	printf("writeToReg32: reg X%d = %x\n", reg, data);
	return true;
}

// write 32 bit data to register, keep upper 32 bits
bool writeToReg32Keep(uint32_t reg, uint32_t data) {
	if (reg >= NUM_GEN_REGS) {
		return false;
	}
	cpu.regs[reg].d_32[0] = data;
	// do not overwrite old value
	// cpu.regs[reg].d_32[1] = 0;
	printf("writeToReg32Keep: reg X%d = %x\n", reg, data);
	return true;
}

// write 64 bit data to register
bool writeToReg64(uint32_t reg, uint64_t data) {
	if (reg >= NUM_GEN_REGS) {
		return false;
	}
	cpu.regs[reg].d_64 = data;
	printf("writeToReg64: reg X%d = %lx\n", reg, data);
	return true;
}

// Print bits of binary from data
void printBinary(uint64_t data, bool extend) {
	if (extend) {
		for (int i = 63; i >= 0; i--) {
			printf("%lu", getBit64(data, i));
		}
		return;
	} else {
		for (int i = 31; i >= 0; i--) {
			printf("%d", getBit32(data, i));
		}
	}
	printf("\n");
}

// (data = 1101, Binary = 11, Denary) (n = 4) -> 
// msb == 1
// 11 - (2 * 1 * 2^3) = -5
// Get the signed denary value of an unsigned "n" bit long value
int getTwosComplementValue(uint64_t data, int n) {
	int msb = data >> (n - 1);
	return (data - (msb * (1 << n)));
}

void checkMemoryIndex(int index) {
	if (index > 2 << 21) {
		fprintf(stderr, "checkMemoryIndex: invalid memory access");
		exit(1);
	}
}

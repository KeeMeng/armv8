// This file declares all the constants and data types used in the emulator.
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// define macros for accessing registers
#define READ_REG64(index) ((index < NUM_GEN_REGS) ? cpu.regs[index].d_64 : 0)
#define READ_REG32(index) ((index < NUM_GEN_REGS) ? cpu.regs[index].d_32[0] : 0)
#define ACCESS_MEM64(mem_index) (*(uint64_t*) &cpu.mem[mem_index])
#define ACCESS_MEM32(mem_index) (*(uint32_t*) &cpu.mem[mem_index])
#define WRITE_REG32(reg, data) writeToReg32(reg, data)
#define WRITE_REG32Keep(reg, data) writeToReg32Keep(reg, data)
#define WRITE_REG64(reg, data) writeToReg64(reg, data)

// number of general purpose registers
#define NUM_GEN_REGS 31
#define ENTRY_ADDR 0

// declare structures related to the ARMv8 architecture
typedef union {
	uint64_t d_64;
	uint32_t d_32[2];
	uint16_t d_16[4];
} Reg;

typedef struct {
	bool n;
	bool z;
	bool c;
	bool v;
} Pstate;

typedef struct {
	uint8_t mem[2 << 21];
	Reg regs[NUM_GEN_REGS];
	Reg pc;
	Pstate pstate;
	bool run_flag;
	bool jmp_flag;
} Armv8;

// declare functions
uint64_t signExtend(uint32_t data);
uint64_t getBit64(uint64_t data, uint8_t n);
uint32_t getBit32(uint32_t bit, uint8_t n);
uint64_t msb64(uint64_t data);
uint32_t msb32(uint32_t data);
uint64_t getBitField64(uint64_t data, uint8_t offset, uint8_t n);
uint32_t getBitField32(uint32_t data, uint8_t offset, uint8_t n);
bool writeToReg32(uint32_t reg, uint32_t data);
bool writeToReg32Keep(uint32_t reg, uint32_t data);
bool writeToReg64(uint32_t reg, uint64_t data);
void printBinary(uint64_t data, bool extend);
int getTwosComplementValue(uint64_t data, int n);
void checkMemoryIndex(int index);

// global variables
extern uint32_t currentInstruction;
extern Armv8 cpu;

#include "common.h"
#include "de_sdt.h"

static void execRegisterOffset(void);
static void execPrefixOffset(void);
static void execUnsignedOffset(void);
static void execLoadLiteral(void);

/*
	Decode load store instructions
	Determine whether the instruction is
	a single data transfer or load literal
	instruction and call the corresponding
	function.
*/
void decodeExecuteSDT(void) {
	single_data_transfer ci = *(single_data_transfer*) &currentInstruction;
	if (ci.msb) { 
		if (ci.u) {
			execUnsignedOffset();
		} else if (getBit32(currentInstruction, 21)) {
			execRegisterOffset();
		} else {
			execPrefixOffset();
		}
	} else {
		execLoadLiteral();
	}
}

/*
	Execute load store instruction of type single data transfer
	Determine the type of data transfer and make the corresponding
	changes to the processor.
*/
void execRegisterOffset(void) {
	printf("execRegisterOffset()\n");
	single_data_transfer ci = *(single_data_transfer*) &currentInstruction;

	uint32_t base_reg = ci.xn;
	uint32_t target_reg = ci.rt;
	uint32_t xm = getBitField32(currentInstruction, 16, 5);
	uint32_t mem_index = (READ_REG64(base_reg) + READ_REG64(xm));
	checkMemoryIndex(mem_index);
	if (ci.sf) {
		uint64_t* target_memory = &ACCESS_MEM64(mem_index);
		if (ci.l) {
			WRITE_REG64(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG64(target_reg);
		}
	} else {
		uint32_t* target_memory = &ACCESS_MEM32(mem_index);
		if (ci.l) {
			WRITE_REG32(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG32(target_reg);
		}
	}
}

void execPrefixOffset(void) {
	printf("execPrefixOffset()\n");
	single_data_transfer ci = *(single_data_transfer*) &currentInstruction;

	uint32_t base_reg = ci.xn;
	uint32_t target_reg = ci.rt;
	bool iBit = getBit32(currentInstruction, 11);
	uint64_t simm9 = getTwosComplementValue(getBitField32(currentInstruction, 12, 9), 9);
	uint32_t mem_index = READ_REG64(base_reg) + (iBit ? simm9 : 0);
	checkMemoryIndex(mem_index);

	if (ci.sf) {
		uint64_t* target_memory = &ACCESS_MEM64(mem_index);
		if (ci.l) {
			WRITE_REG64(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG64(target_reg);
		}
	} else {
		uint32_t* target_memory = &ACCESS_MEM32(mem_index);
		if (ci.l) {
			WRITE_REG32(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG32(target_reg);
		}
	}

	WRITE_REG64(base_reg, READ_REG64(base_reg) + simm9);
}

void execUnsignedOffset(void) {
	printf("execUnsignedOffset()\n");
	single_data_transfer ci = *(single_data_transfer*) &currentInstruction;
	
	uint32_t base_reg = ci.xn;
	uint32_t target_reg = ci.rt;
	if (ci.sf) {
		uint32_t mem_index = READ_REG64(base_reg) + 8 * (ci.offset);
		checkMemoryIndex(mem_index);
		uint64_t* target_memory = &ACCESS_MEM64(mem_index);
		if (ci.l) {
			WRITE_REG64(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG64(target_reg);
		}
	} else {
		uint32_t mem_index = READ_REG64(base_reg) + 4 * (ci.offset);
		checkMemoryIndex(mem_index);
		uint32_t* target_memory = &ACCESS_MEM32(mem_index);
		if (ci.l) {
			WRITE_REG32(target_reg, *target_memory);
		} else {
			*target_memory = READ_REG32(target_reg);
		}
	}
}

/*
	Execute load store instruction of type load literal
	Load literal value into the register.
*/
void execLoadLiteral(void) {
	printf("execLoadLiteral()\n");
	load_literal ci = *(load_literal*) &currentInstruction;

	int simm19 = getTwosComplementValue(ci.simm19, 19);
	uint32_t target_reg = ci.rt;
	uint32_t mem_index = cpu.pc.d_64 + (int64_t) 4 * (simm19);
	checkMemoryIndex(mem_index);
	uint64_t target_memory = ACCESS_MEM64(mem_index);
	WRITE_REG64(target_reg, target_memory);
	if (!(ci.sf)) { cpu.regs[target_reg].d_32[1] = 0; }
}

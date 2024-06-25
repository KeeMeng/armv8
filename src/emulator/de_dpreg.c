#include "common.h"
#include "de_dpreg.h"

static void execMult(void);
static void execLogic(void);
static void execLogic32(void);
static void execLogic64(void);
static void execArithmetic(void);
static void execArithmetic32(void);
static void execArithmetic64(void);

/*
	Decode data processing instruction register.
	Determine whether the data processing
	register instruction is an arithmetic,
	logical or multiply operation and run the
	corresponding execute function.

	Pre: the instruction is of the instruction group
	data processing register, i.e. op0 = x101
*/
void decodeExecuteDPReg(void) {
	// assert that instruction is of the data processing register group
	assert(getBit32(currentInstruction, 27) && (!getBit32(currentInstruction, 26)) & getBit32(currentInstruction, 25));

	// get M at bit 28
	// when m = 1, multiply
	if (getBit32(currentInstruction, 28)) {
		// multiply
		execMult();
	} else {
		// m != 1, so check bits 24 and 21
		// to determine arithmetic or logic
		if (getBit32(currentInstruction, 24) && (!getBit32(currentInstruction, 21))) {
			// arithmetic
			execArithmetic();
		} else if (!getBit32(currentInstruction, 24)) {
			// bit logic
			execLogic();
		}
	}
}

uint64_t rotateRight64(uint64_t data, uint8_t shift) {
	printf("rotateRight64()\n");
	uint64_t rightPart = data >> shift;
	uint64_t leftPart = data << (sizeof(data) * 8 - shift);
	return leftPart + rightPart;
}

uint32_t rotateRight32(uint32_t data, uint8_t shift) {
	printf("rotateRight32()\n");
	uint32_t rightPart = data >> shift;
	uint32_t leftPart = data << (sizeof(data) * 8 - shift);
	return leftPart + rightPart;
}

uint64_t arithmeticShiftRight64(uint64_t data, uint8_t shift) {
	printf("arithmeticShiftRight64()\n");
	if (msb64(data)) {
		return ((((uint64_t) 1 << shift) - 1) << (sizeof(data) * 8 - shift)) | (data >> shift);	
	} else {
		return data >> shift;
	}
}

uint32_t arithmeticShiftRight32(uint32_t data, uint8_t shift) {
	printf("arithmeticShiftRight32()\n");
	if (msb32(data)) {
		return ((((uint32_t) 1 << shift) - 1) << (sizeof(data) * 8 - shift)) | (data >> shift);	
	} else {
		return data >> shift;
	}
}

/*
	Shift operand
	Check the current instruction and shift the operand accordingly.

	We truncate before shifting.
	So if we shift 32 bits, we only modify the bottom 32 bits.
*/
uint64_t shiftOperand64(uint64_t data, uint8_t amount, uint8_t shiftType) {
	switch (shiftType) {
		case 0:
			// logical shift left
			printf("shiftOperand64: logical shift left\n");
			data = data << amount;
			break;
		case 1:
			// logical shift right
			printf("shiftOperand64: logical shift right\n");
			data = data >> amount;
			break;
		case 2:
			// arithmetic shift right
			printf("shiftOperand64: arithmetic shift right\n");
			data = arithmeticShiftRight64(data, amount);
			break;
		case 3:
			// rotate right
			printf("shiftOperand64: rotate right\n");
			data = rotateRight64(data, amount);
			break;
	}
	return data;
}

uint32_t shiftOperand32(uint32_t data, uint8_t amount, uint8_t shiftType) {
	switch (shiftType) {
		case 0:
			// logical shift left
			printf("shiftOperand32: logical shift left\n");
			data = data << amount;
			break;
		case 1:
			// logical shift right
			printf("shiftOperand32: logical shift right\n");
			data = data >> amount;
			break;
		case 2:
			// arithmetic shift right
			printf("shiftOperand32: arithmetic shift right\n");
			data = arithmeticShiftRight32(data, amount);
			break;
		case 3:
			// rotate right
			printf("shiftOperand32: rotate right\n");
			data = rotateRight32(data, amount);
			break;
	}
	return data;
}

/*
	Execute arithmetic instruction (DPReg)
	Determine the specific arithmetic operation to perform,
	then make the corresponding changes to the processor
	memory, registers and pstate.
*/
void execArithmetic(void) {
	// check size bit
	if (msb32(currentInstruction)) {
		// 64-bit execution
		execArithmetic64();	
	} else {
		// 32-bit execution
		execArithmetic32();
	}
}

// 64-bit data processing register instruction for 64-bit
void execArithmetic64(void) {
	// get instruction as structure
	data_process_reg dpr = *(data_process_reg*) &currentInstruction;

	// get operandsqq
	uint64_t op1 = READ_REG64(dpr.rn);
	uint64_t op2 = READ_REG64(dpr.rm);

	// get shift type
	uint8_t shiftType = getBitField32(currentInstruction, 22, 2);
	uint8_t shiftAmount = dpr.operand;

	// shift op2
	op2 = shiftOperand64(op2, shiftAmount, shiftType);

	// compute result
	uint64_t result;

	if (dpr.opc < 2) {
		// add
		result = op1 + op2;
	} else {
		// sub
		result = op1 - op2;
	}

	// store result in 64-bit register
	WRITE_REG64(dpr.rd, result);

	// update PSTATE flags
	if (dpr.opc == 1 || dpr.opc == 3) {
		// set sign flag
		cpu.pstate.n = getBit64(result, sizeof(result) * 8 - 1);
		cpu.pstate.z = (result == 0);
			
		// set carry and overflow flags
		if (dpr.opc == 1) {
			// addition
				
			// carry flag - check for unsigned overflow
			cpu.pstate.c = (result < op1 || result < op2);

			// overflow flag - check for signed overflow
			cpu.pstate.v = (msb64(op1) && msb64(op2) && !msb64(result)) || (!msb64(op1) && !msb64(op2) && msb64(result));
		} else {
			// subtraction - check if borrow was produced
			cpu.pstate.c = (op2 <= op1);

			// overflow flag - check for signed underflow
			cpu.pstate.v = (msb64(op1) && !msb64(op2) && !msb64(result)) || (!msb64(op1) && msb64(op2) && msb64(result));
		}
	}
}

// 32-bit data processing register instruction for 32-bit
void execArithmetic32(void) {
	// get instruction as structure
	data_process_reg dpr = *(data_process_reg*) &currentInstruction;

	// get operands
	uint32_t op1 = READ_REG32(dpr.rn);
	uint32_t op2 = READ_REG32(dpr.rm);

	// get shift type
	uint8_t shiftType = getBitField32(currentInstruction, 22, 2);
	uint8_t shiftAmount = dpr.operand;

	// shift op2
	op2 = shiftOperand32(op2, shiftAmount, shiftType);

	// compute result
	uint32_t result;

	if (dpr.opc < 2) {
		// add
		result = op1 + op2;
	} else {
		// sub
		result = op1 - op2;
	}
	
	// store result in 64-bit register
	WRITE_REG32(dpr.rd, result);

	// update PSTATE flags
	if (dpr.opc == 1 || dpr.opc == 3) {
		// set sign flag
		cpu.pstate.n = getBit32(result, sizeof(result) * 8 - 1);
		cpu.pstate.z = (result == 0);
			
		// set carry and overflow flags
		if (dpr.opc == 1) {
			// addition
				
			// carry flag - check for unsigned overflow
			cpu.pstate.c = (result < op1 || result < op2);

			// overflow flag - check for signed overflow
			cpu.pstate.v = (msb32(op1) && msb32(op2) && !msb32(result)) || (!msb32(op1) && !msb32(op2) && msb32(result));
		} else {
			// subtraction - check if borrow was produced
			cpu.pstate.c = (op2 <= op1);

			// overflow flag - check for signed underflow
			cpu.pstate.v = (msb32(op1) && !msb32(op2) && !msb32(result)) || (!msb32(op1) && msb32(op2) && msb32(result));
		}
	}
}

/* 
	Execute bit logic instruction (DPReg)
	Determine the specific logical operation to perform,
	then make the corresponding changes to the processor
	memory, registers and pstate.
*/
void execLogic(void) {
	// check size bit
	if (msb32(currentInstruction)) {
		// 64-bit execution
		execLogic64();	
	} else {
		// 32-bit execution
		execLogic32();
	}
}

// 64-bit data processing register instruction for 64-bit
void execLogic64(void) {
	// get instruction as structure
	data_process_reg dpr = *(data_process_reg*) &currentInstruction;

	// get operands
	uint64_t op1 = READ_REG64(dpr.rn);
	uint64_t op2 = READ_REG64(dpr.rm);

	// get shift type
	uint8_t shiftType = getBitField64(currentInstruction, 22, 2);
	uint8_t shiftAmount = dpr.operand;

	// shift op2
	op2 = shiftOperand64(op2, shiftAmount, shiftType);

	// negate if N bit is 1
	if (getBit64(currentInstruction, 21)) {
		op2 = ~op2;
	}

	// compute result
	uint64_t result;

	switch (dpr.opc) {
		case 1:
			// or
			printf("execLogic32: orr/orn\n");
			result = op1 | op2;
			break;
		case 2:
			// xor
			printf("execLogic32: eor/eon\n");
			result = op1 ^ op2;
			break;
		case 0:
		case 3:
			// and
			printf("execLogic32: and/ands/bic/bics\n");
			result = op1 & op2;
			break;
	}

	// store result in 64-bit register
	WRITE_REG64(dpr.rd, result);

	// update PSTATE if in and
	if (dpr.opc == 3) {
		cpu.pstate.n = msb64(result);
		cpu.pstate.z = (result == 0);
		cpu.pstate.c = false;
		cpu.pstate.v = false;
	}
}

// 32-bit data processing register instruction for 32-bit
void execLogic32(void) {
	// get instruction as structure
	data_process_reg dpr = *(data_process_reg*) &currentInstruction;

	// get operands
	uint32_t op1 = READ_REG32(dpr.rn);
	uint32_t op2 = READ_REG32(dpr.rm);

	// get shift type
	uint8_t shiftType = getBitField32(currentInstruction, 22, 2);
	uint8_t shiftAmount = dpr.operand;

	// shift op2
	op2 = shiftOperand32(op2, shiftAmount, shiftType);

	// negate if N bit is 1
	if (getBit32(currentInstruction, 21)) {
		op2 = ~op2;
	}

	// compute result
	uint32_t result;

	switch (dpr.opc) {
		case 1:
			// or
			printf("execLogic32: orr/orn\n");
			result = op1 | op2;
			break;
		case 2:
			// xor
			printf("execLogic32: eor/eon\n");
			result = op1 ^ op2;
			break;
		case 0:
		case 3:
			// and
			printf("execLogic32: and/ands/bic/bics\n");
			result = op1 & op2;
			break;
	}

	// store result in 32-bit register
	WRITE_REG32(dpr.rd, result);

	// update PSTATE if in and
	if (dpr.opc == 3) {
		cpu.pstate.n = msb32(result);
		cpu.pstate.z = (result == 0);
		cpu.pstate.c = false;
		cpu.pstate.v = false;
	}
}

/*
	Execute multiply instruction (DPReg)
	Determine the specific multiply operation to
	perform and make the corresponding changes to
	the processor.
*/
void execMult(void) {
	printf("execMult()\n");
	// get instruction as structure
	data_process_reg dpr = *(data_process_reg*) &currentInstruction;

	assert(dpr.rd <= NUM_GEN_REGS);
	
	// get product sign
	// if bit 15 = 1 we subtract rn * rm from ra. If 0 we add.
	uint64_t negateFlag = getBit32(currentInstruction, 15);
	
	// get register
	uint8_t baseReg = getBitField32(currentInstruction, 10, 5);

	// check if in 32-bit or 64-bit mode
	if (msb32(currentInstruction)) {
		// get 64-bit base value
		uint64_t result = READ_REG64(baseReg);
	
		// get 64-bit operands
		uint64_t op1 = READ_REG64(dpr.rn);
		uint64_t op2 = READ_REG64(dpr.rm);

		result += op1 * op2 * (negateFlag ? -1 : 1);

		// store result in 64 bit register
		WRITE_REG64(dpr.rd, result);
	} else {
		// get 32-bit base value
		uint32_t result = READ_REG32(baseReg);
		
		// get 32-bit operands
		uint32_t op1 = READ_REG32(dpr.rn);
		uint32_t op2 = READ_REG32(dpr.rm);

		result += op1 * op2 * (negateFlag ? -1 : 1);

		// store result in 32 bit register
		WRITE_REG32(dpr.rd, result);
	}
	// Note: The spec does not say whether we set the pstate flags.
}

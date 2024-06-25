#include "common.h"
#include "de_dpimm.h"

static void execArithmetic(void);
static void execWMove(void);

/*
	Decode data processing immediate instruction.
	Precondition: the instruction stored in the cpu's
	currentInstruction is of the data processing immediate group.
	This function determines whether the instruction
	is an arithmetic instruction or a wide move instruction
	and calls the corresponding execution function.
*/
void decodeExecuteDPImm(void) {
	arithmetic dpimm = *(arithmetic*) &currentInstruction;
	// opi is at the same bits for both arithmetic and wide move
	switch (dpimm.opi) {
		case 2:
			// 010 = arithmetic
			execArithmetic();
			break;
		case 5:
			// 101 = wide move
			execWMove();
			break;
	}
}

/*
	Execute arithmetic instruction (DPImm).
	Precondition: the instruction is an arithmetic
	instruction of the data processing immediate
	group. This function determines the specific
	arithmetic instruction and executes it,
	modifying the CPU's state accordingly.
*/
void execArithmetic(void) {
	// convert current instruction to dpImm structure
	arithmetic dpimm_arith = *(arithmetic*) &currentInstruction;

	uint32_t op2 = dpimm_arith.sh ? dpimm_arith.imm12 << 12 : dpimm_arith.imm12;

	if (msb32(currentInstruction)) {
		// 64-bit
		printf("execDPImmArithmetic: executing 64bit Imm instruction.");
		uint64_t result;
		uint64_t op1 = READ_REG64(dpimm_arith.rn);

		if (dpimm_arith.opc <= 1) {
			// addition
			result = op1 + op2;
		} else {
			// subtraction
			result = op1 - op2;
		}
		
		// set result
		WRITE_REG64(dpimm_arith.rd, result);
		
		// update pstate if applicable
		if (dpimm_arith.opc == 1) {
			// update condition flags if adds
			cpu.pstate.n = getBit64(result, sizeof(result) * 8 - 1);
			cpu.pstate.z = (result == 0);

			// one if the addition produced a carry (unsigned overflow)
			cpu.pstate.c = (result < READ_REG64(dpimm_arith.rn));
			
			// signed overflow if the result is negative and rn is positive
			cpu.pstate.v = (result < 0) && (READ_REG64(dpimm_arith.rn) > 0);
		} else if (dpimm_arith.opc == 3) {
			// update condition flags if subs
			cpu.pstate.n = getBit64(result, sizeof(result) * 8 - 1);
			cpu.pstate.z = (result == 0);
			
			// zero if the subtraction produced a borrow
			cpu.pstate.c = (op2 <= op1);
			
			// signed overflow if the result is positive and rn is negative
			cpu.pstate.v = (result > 0) && (READ_REG64(dpimm_arith.rn) < 0);
		}
	} else {
		// 32-bit
		printf("execDPImmArithmetic: executing 32bit Imm instruction.");
		uint32_t result;
		uint32_t op1 = READ_REG32(dpimm_arith.rn);

		if (dpimm_arith.opc <= 1) {
			// addition
			result = op1 + op2;
		} else {
			// subtraction
			result = op1 - op2;
		}

		// set result
		WRITE_REG32(dpimm_arith.rd, result);

		// update pstate if applicable
		if (dpimm_arith.opc == 1) {
			// update condition flags if adds
			cpu.pstate.n = getBit32(result, sizeof(result) * 8 - 1);
			cpu.pstate.z = (result == 0);
			
			// one if the addition produced a carry (unsigned overflow)
			cpu.pstate.c = (result < READ_REG32(dpimm_arith.rn));

			// signed overflow if the result is negative and rn is positive
			cpu.pstate.v = (result < 0) && (READ_REG32(dpimm_arith.rn) > 0);
		} else if (dpimm_arith.opc == 3) {
			// update condition flags if subs
			cpu.pstate.n = getBit32(result, sizeof(result) * 8 - 1);
			cpu.pstate.z = (result == 0);
			
			// zero if the subtraction produced a borrow
			cpu.pstate.c = (op2 <= op1);

			// signed overflow if the result is positive and rn is negative
			cpu.pstate.v = (result > 0) && (READ_REG32(dpimm_arith.rn) < 0);
		}
	}
}

/*
	Execute wide move instruction (DPImm).
	Precondition: the instruction is a wide move
	instruction of the data processing immediate group.
	This function determines the specific move
	instruction and executes it, modifying the
	CPU's state accordingly.
*/
void execWMove(void) {
	wide_move dpimm_wm = *(wide_move*) &currentInstruction;
	switch (dpimm_wm.opc) {
		case 0:
			// movn
			printf("execDPImmWMove: movn\n");
			WRITE_REG64(dpimm_wm.rd, ~((uint64_t) dpimm_wm.imm16 << (dpimm_wm.hw * 16)));
			break;
		case 2:
			// movz
			printf("execDPImmWMove: movz\n");
			WRITE_REG64(dpimm_wm.rd, (uint64_t) dpimm_wm.imm16 << (dpimm_wm.hw * 16));
			break;
		case 3:
			// movk
			printf("execDPImmWMove: movk\n");
			// set the bits [(hw * 16) + 15 : hw * 16] to imm16
			if (dpimm_wm.rd != NUM_GEN_REGS) {
				cpu.regs[dpimm_wm.rd].d_16[dpimm_wm.hw] = dpimm_wm.imm16;
			}
			break;
	}

	// set top 32 bits to 0 for 32 bit registers
	if (dpimm_wm.rd != NUM_GEN_REGS && dpimm_wm.sf == 0 && (dpimm_wm.opc == 0 || dpimm_wm.opc == 3)) {
		cpu.regs[dpimm_wm.rd].d_32[1] = 0;
	}
}

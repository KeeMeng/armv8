#include "common.h"
#include "fde.h"
#include "de_dpreg.h"
#include "de_dpimm.h"
#include "de_sdt.h"
#include "de_branch.h"

static InstructionGroup getInstructionGroup(void);

/*
	Fetch
	This function gets the next instruction. This is given
	by the 4 bytes in memory at the address stored in PC.
	The instruction is stored in the instruction register.
	(Note that the currentInstruction is not an actual register in the ARM
	processor but a useful value we store globally).
*/
void fetch(void) {
	// read 4 bytes starting at pc
	printf("fetch: PC value: %lu\n", cpu.pc.d_64);
	currentInstruction = ACCESS_MEM32(cpu.pc.d_64);
}

/*
	Decode - starts the decode-execute pipeline.
	Start by calling the getInstructionGroup function.

	This function also checks for halt and changes the running
	flag in the processor structure respectively.
*/
void decode(void) {
	// check if current instruction is halt
	if (currentInstruction == 0x8a000000) {
		printf("decode: instruction is halt\n");
		cpu.run_flag = false;

		// do not increment program counter
		cpu.jmp_flag = true;
	} else {
		switch (getInstructionGroup()) {
			case DPImm:
				printf("decode: instruction group is data processing immediate\n");
				decodeExecuteDPImm();
				break;
			case DPReg:
				printf("decode: instruction group is data processing register\n");
				decodeExecuteDPReg();
				break;
			case SDT:
				printf("decode: instruction group is load/store\n");
				decodeExecuteSDT();
				break;
			case Branch:
				printf("decode: instruction group is branch\n");
				decodeExecuteBranch();
				break;
		}
	}
}

/*
	Determine the instruction group.
	Then call the decode function for
	the corresponding group.
*/
InstructionGroup getInstructionGroup(void) {
	int32_t op0[4] = { getBit32(currentInstruction, 28), getBit32(currentInstruction, 27), getBit32(currentInstruction, 26), getBit32(currentInstruction, 25) };
	printf("getInstructionGroup: op0 = [%x, %x, %x, %x]\n", op0[0], op0[1], op0[2], op0[3]);
	if (op0[0] && !op0[1] && !op0[2]) {
		// 100X
		return DPImm;
	} else if (op0[1] && !op0[2] && op0[3]) {
		// X101
		return DPReg;
	} else if (op0[1] && !op0[3]) {
		// X1X0
		return SDT;
	} else if (op0[0] && !op0[1] && op0[2]) {
		// 101X
		return Branch;
	} else {
		fprintf(stderr, "getInstructionGroup: failed to match op0\n");
		exit(1);
	}
}

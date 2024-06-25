#include "assembleDefs.h"
#include "tr_sdt.h"

// to translate Single Data Transfer (load/store) instructions
uint32_t translateLSInstructions(AssemblyInstruction dAssLn) {
	// initialise the instruction
	uint32_t instruction = 0;

	// the target register makes up bits 0-4 of the instruction
	uint32_t rt = dAssLn.operands[0].data;
	instruction += rt;

	if (dAssLn.addressMode == normal) {
		// load literal branch
		instruction += 0x18000000;

		// the offset makes up bits 5-23 of the instruction
		uint32_t imm19 = dAssLn.operands[1].data;
		int32_t simm19 = imm19 & (int) ((1 << 21) - 1) >> 2;
		instruction += simm19 << 5;

	} else {
		// the SDT branch
		instruction += 0xb8000000;
		printf("SDT Branch: current instruction: %x\n", instruction);
		uint32_t xn = dAssLn.operands[1].data << 5;
		instruction += xn;
		printf("Added xn: current instruction: %x\n", instruction);

		if (dAssLn.addressMode == RO) {
			assert(dAssLn.numOperands == 3);
			instruction += 0x81a << 10;
			uint32_t xm = dAssLn.operands[2].data << 16;
			instruction += xm;

		} else if (dAssLn.addressMode == Pre || dAssLn.addressMode == Post) {
			assert(dAssLn.numOperands == 3);
			instruction += 1 << 10;
			printf("Set 10th bit: current instruction: %x\n", instruction);
			uint32_t imm9 = dAssLn.operands[2].data;
			uint32_t simm9 = imm9 & (int) ((1 << 9) - 1);
			instruction += simm9 << 12;
			printf("added simm9: %x current instruction: %x\n", simm9, instruction);

			if (dAssLn.addressMode == Pre) { instruction += 1 << 11; }

		} else if (dAssLn.addressMode == UO) {
			instruction += 1 << 24;
			uint32_t offset = dAssLn.operands[2].data;
			if (dAssLn.operands[0].type == Xi) {
				offset = offset >> 3;
			} else {
				offset = offset >> 2;
			}
			instruction += offset << 10;
		} else {
			instruction += 1 << 24;
		}

		// load instructions have the "L" (22nd) bit set to 1
		if (dAssLn.name == ldr) { instruction += 1 << 22; }
	}

	// 64-bit register instructions have the "sf" (30th) bit set to 1
	if (dAssLn.operands[0].type == Xi) { instruction += 1 << 30; }

	printf("\n");
	return instruction;
}

#include "assembleDefs.h"
#include "tr_branch.h"

// for translating branch instructions
uint32_t translateBranchInstructions(AssemblyInstruction dAssLn) {
	uint32_t instruction = 0;

	if (dAssLn.name == b) {
		// 0 0 0 1 0 1 simm26(26)
		// set upper bits to 0 0 0 1 0 1 
		instruction = 5 << 26;
		printf("After setting upper fixed bits, instruction is = %x\n", instruction);

		// set simm26 to offset
		uint32_t offset;

		offset = mask(dAssLn.operands[0].data, 26);
		printf("offset = %x\n", offset);
		instruction += offset;
		printf("After setting simm26 bit, instruction is = %x\n", instruction);

	} else if (dAssLn.name == br) {
		// 1 1 0 1 0 1 1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 xn(5) 0 0 0 0 0 
		instruction = 0xD61F0000;
		
		// set xn
		instruction += mask(dAssLn.operands[0].data, 5) << 5;
	} else {
		// 0 1 0 1 0 1 0 0 simm19(19) 0 cond(4)
		// set upper bits for all b.cond to 0 1 0 1 0 1 0 0
		instruction = 0x54000000;

		// set lower bits to cond
		switch (dAssLn.name) {
		case beq:
			// encoding = 0000 (0)
			instruction += 0;
			break;
		case bne:
			// encoding = 0001 (1)
			instruction += 1;
			break;
		case bge:
			// encoding = 1010 (10)
			instruction += 10;
			break;
		case blt:
			// encoding = 1011 (11)
			instruction += 11;
			break;
		case bgt:
			// encoding = 1100 (12)
			instruction += 12;
			break;
		case ble:
			// encoding = 1101 (13)
			instruction += 13;
			break;
		case bal:
			// encoding = 1110 (14)
			instruction += 14;
			break;
		default:
			fprintf(stderr, "Error in switchcase function translateBranchInstructions\n");
			break;
		}
		
		// set simm19 to offset
		uint32_t offset;
		printf("dAssLn.operands[0].data: %d\n", dAssLn.operands[0].data);
		printf("dAssLn.address: %d\n", dAssLn.address);
		offset = mask(dAssLn.operands[0].data, 19);
		printf("offset: %d\n", offset);
		instruction += offset << 5;
	}
	
	return instruction;
}

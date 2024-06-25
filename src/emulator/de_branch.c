#include "common.h"
#include "de_branch.h"

static bool checkBranchCond(int encoding);

/* 
	Decode branch instruction
	Determine whether the branch is unconditional,
	register or conditional. Then run the corresponding
	execution function.
*/
void decodeExecuteBranch(void) {
	switch (getBitField32(currentInstruction, 30, 2)) {
		case 0: {
			// Unconditional branching
			printf("decodeExecuteBranch: reached unconditional branching.\n");
			unconditional_branch ubranch = *(unconditional_branch*) &currentInstruction;
			uint64_t offset = getTwosComplementValue(ubranch.simm26, 26);
			cpu.pc.d_64 += offset * 4;
			cpu.jmp_flag = true;
			break;
		}
		case 1: {
			// Conditional branching
			printf("decodeExecuteBranch: reached conditional branching.\n");
			conditional_branch cbranch = *(conditional_branch*) &currentInstruction;
			uint64_t offset = getTwosComplementValue(cbranch.simm19, 19);
			if (checkBranchCond(cbranch.cond)) {
				cpu.pc.d_64 += offset * 4;
				cpu.jmp_flag = true;
			}
			break;
		}
		case 3: {
			// Register branching
			printf("decodeExecuteBranch: reached register branching.\n");
			reg_branch rbranch = *(reg_branch*) &currentInstruction;
			cpu.pc.d_64 = cpu.regs[rbranch.xn].d_64;
			cpu.jmp_flag = true;
			break;
		}
		default:
			fprintf(stderr, "decodeExecuteBranch: error with matching bit fields");
	}
}

// Return true if the branch instruction condition is satisfied.
bool checkBranchCond(int encoding) {
	Pstate ps = cpu.pstate;
	switch (encoding) {
		case 0:  return (ps.z == 1);
		case 1:  return (ps.z == 0);
		case 10: return ps.n == ps.v;
		case 11: return ps.n != ps.v;
		case 12: return ((ps.z == 0) && (ps.n == ps.v));
		case 13: return !((ps.z == 0) && (ps.n == ps.v));
		case 14: return true;
		default: 
			fprintf(stderr, "Error with switch case in function checkBranchCond().");
			return false;
	}
}

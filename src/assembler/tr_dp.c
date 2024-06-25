#include "assembleDefs.h"
#include "tr_dp.h"

void calcImmArithHelper(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	// create and assume operand type
	operand rn = dAssLn.operands[1];
	assert(rn.type == Xi || rn.type == Wi);
	operand imm = dAssLn.operands[2];
	assert(imm.type == literal);

	// set opi and bits 23-25
	*binInstr += 2 << 23;
	printf("After setting opi bit, instruction is = %x\n", *binInstr);

	// check for a "sh" bit and set bit 22
	if (dAssLn.numOperands == 4) {
		operand shiftOp = dAssLn.operands[3];
		*binInstr += (shiftOp.data == 12) ? 1 << 22 : 0;
	}
	printf("After setting sh bit, instruction is = %x\n", *binInstr);

	// set imm12 and bits 10-21
	*binInstr += imm.data << 10;
	printf("After setting imm12 bit, instruction is = %x\n", *binInstr);

	// set rn and bits 5-9
	*binInstr += rn.data << 5;
	printf("After setting rn bit, instruction is = %x\n", *binInstr);
}

void calcImmWideMoveHelper(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	// create and assume operand type
	operand imm = dAssLn.operands[1];
	assert(imm.type == literal);

	// set opi and bits 23-25
	*binInstr += 5 << 23;
	printf("After setting opi bit, instruction is = %x\n", *binInstr);

	// set "hw" and bits 21 and 22 if a third optional arg is passed
	printf("Num of operands: %d\n", dAssLn.numOperands);
	if (dAssLn.numOperands == 3) {
		operand shiftOp = dAssLn.operands[2];
		assert(shiftOp.type == shift);
		*binInstr += (shiftOp.data / 16) << 21;
	}
	printf("After setting 'hw' bit, instruction is = %x\n", *binInstr);

	// set "imm16" and bits 5-20
	*binInstr += imm.data << 5;
	printf("Imm: %x\n", imm.data);
	printf("After setting 'imm' bit, instruction is = %x\n", *binInstr);
}

// calculate binary for immediate DP instructions
void calculateImmInstructions(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	// create and assume operand type
	operand rd = dAssLn.operands[0];

	// determine sf flag and set bit 31
	*binInstr += (rd.type == Xi) ? 1 << 31 : 0;
	printf("After setting sf bit, instruction is = %x\n", *binInstr);

	// determine opc and set bits 29 and 30 
	switch (dAssLn.name) {
	case add: 
	case movn:
		break;
	case adds:
		*binInstr += 1 << 29;
		break;
	case sub: 
	case movz:
		*binInstr += 2 << 29;
		break;
	case subs:
	case movk: 
		*binInstr += 3 << 29;
		break;
	default:
		fprintf(stderr, "Potential error with switchcase when translating immediate DP into binary.");
	}
	printf("After setting opc bit, instruction is = %x\n", *binInstr);

	// apply fixed bits at bits 26-28
	*binInstr += 4 << 26;
	printf("After setting fixed bit, instruction is = %x\n", *binInstr);

	// calculate operand and opi (bits 5-25) for either arith or wide move
	AssMneumonic am = dAssLn.name;
	if (am == movn || am == movz || am == movk) {
		calcImmWideMoveHelper(dAssLn, binInstr);
	} else {
		calcImmArithHelper(dAssLn, binInstr);
	}
}

// helper to apply opr to instruction 
void calculateRegInstrOpr(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	if (dAssLn.numOperands == 4) {
		operand shiftOp = dAssLn.operands[3];
		printf("shift is %d\n", shiftOp.shift);
		switch (shiftOp.shift) {
		case lsl:
			break;
		case lsr:
			*binInstr += 1 << 22;
			break;
		case asr:
			*binInstr += 2 << 22;
			break;
		case ror:
			*binInstr += 3 << 22;
			break;
		default:
			fprintf(stderr, "Error in calculateRegInstrOpr functions.");
		}
	}
}

// helper to apply operand to instruction 
void calculateRegInstrOperand(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	AssMneumonic am = dAssLn.name;
	if (am == madd || am == msub) {
		if (am == msub) {
			*binInstr += 1 << 15;
		}
		operand ra = dAssLn.operands[3];
		*binInstr += ra.data << 10;
	}
	else if (dAssLn.numOperands == 4) {
		operand shiftOp = dAssLn.operands[3];
		*binInstr += shiftOp.data << 10;
	}
}

// calculate binary for register DP instructions
void calculateRegInstructions(AssemblyInstruction dAssLn, uint32_t* binInstr) {
	// <Rd>, <Rn>, <Rm>{, <shift> #<imm>}
	// sf(1) opc(2) M(1) 1 0 1 opr(4) rm(5) operand(6) rn(5) rd(5)
	operand rn = dAssLn.operands[1];
	operand rm = dAssLn.operands[2];

	// determine sf flag and set bit 31
	*binInstr += (rn.type == Xi) ? 1 << 31 : 0;
	printf("After setting sf bit, instruction is = %x\n", *binInstr);

	// determine opc and set bits 29 and 30 
	switch (dAssLn.name) {
	case add: 
	case and:
	case bic:
		break;
	case adds:
	case orr:
	case orn:
		*binInstr += 1 << 29;
		break;
	case sub: 
	case eor:
	case eon:
		*binInstr += 2 << 29;
		break;
	case subs:
	case ands:
	case bics: 
		*binInstr += 3 << 29;
		break;
	default:
		fprintf(stderr, "Potential error with switchcase when translating immediate DP into binary.");
	}
	printf("After setting opc bit, instruction is = %x\n", *binInstr);

	// set M (bit 28)
	AssMneumonic am = dAssLn.name;
	if (am == madd || am == msub) {
		// Set M
		*binInstr += 1 << 28;
	}
	printf("After setting M bit, instruction is = %x\n", *binInstr);

	// set fixed bits (bit 25-27)
	*binInstr += 5 << 25;
	printf("After setting fixed bits, instruction is = %x\n", *binInstr);

	// set opr (bits 21-24)
	// arith: opr = 1xx0
	// logic: opr = 0xxx
	// mult:  opr = 1000
	if (am == add || am == adds || am == sub || am == subs) {
		// arith operations
		calculateRegInstrOpr(dAssLn, binInstr);
		*binInstr += 8 << 21;
	}
	else if (am == madd || am == msub) {
		// mult operations
		*binInstr += 8 << 21;
	}
	else {
		// logical operations - and(s), bic(s), eor, orr, eon, orn
		calculateRegInstrOpr(dAssLn, binInstr);

		// set N bit (bit 21)
		if (am == bic || am == orn || am == eon || am == bics ) {
			*binInstr += 1 << 21;
		}
	}
	printf("After setting opr bits, instruction is = %x\n", *binInstr);

	// Set rm (bits 16-20)
	*binInstr += rm.data << 16;
	printf("After setting rm, instruction is = %x\n", *binInstr);

	// set operand (bits 10-15)
	calculateRegInstrOperand(dAssLn, binInstr);
	printf("After setting operand, instruction is = %x\n", *binInstr);

	// set rn (bits 5-9)
	*binInstr += rn.data << 5;
	printf("After setting rn, instruction is = %x\n", *binInstr);
}

// generate a zero register 
operand zeroRegisterGenerator(operand op) {
	operand rzr;
	rzr.data = 31;
	rzr.shift = noShift;
	assert(op.type == Xi || op.type == Wi);
	if (op.type == Xi) {
		rzr.type = Xi;
	} else {
		rzr.type = Wi;
	}
	return rzr;
}

// generate a new assembly instruction with 3 operands
AssemblyInstruction newAssemblyInstruction3Op(AssMneumonic name, operand op1, operand op2, operand op3) {
	AssemblyInstruction newAI;
	newAI.numOperands = 0;

	newAI.name = name;
	newAI.operands[0] = op1;
	newAI.operands[1] = op2;
	newAI.operands[2] = op3;
	newAI.numOperands = 3;

	return newAI;
}

// generate a new assembly instruction with 4 operands
AssemblyInstruction newAssemblyInstruction4Op(AssMneumonic name, operand op1, operand op2, operand op3, operand op4) {
	AssemblyInstruction newAI = newAssemblyInstruction3Op(name, op1, op2, op3);
	newAI.operands[3] = op4;
	newAI.numOperands = 4;
	return newAI;
}

// check for an alias and redirect to corresponding operation if alias is found
uint32_t checkDPAlias(AssemblyInstruction dAssLn) {
	operand op1 = dAssLn.operands[0];
	operand op2 = dAssLn.operands[1];
	operand op3 = dAssLn.operands[2];
	AssMneumonic aliasAm;
	switch (dAssLn.name) {
	case cmp:
		aliasAm = subs;
		break;
	case cmn:
		aliasAm = adds;
		break;
	case neg:
		aliasAm = sub;
		break;
	case negs:
		aliasAm = subs;
		break;
	case tst:
		aliasAm = ands;
		break;
	case mvn:
		aliasAm = orn;
		break;
	case mov:
		aliasAm = orr;
		break;
	case mul:
		aliasAm = madd;
		break;
	case mneg:
		aliasAm = msub;
		break;
	default:
		return 0;
	}

	AssMneumonic am = dAssLn.name;
	if (am == cmp || am == cmn || am == tst) {
		if (dAssLn.numOperands == 3) {
			return translateDPInstructions(newAssemblyInstruction4Op(aliasAm, zeroRegisterGenerator(op1), op1, op2, op3));
		} else {
			return translateDPInstructions(newAssemblyInstruction3Op(aliasAm, zeroRegisterGenerator(op1), op1, op2));
		}
	} else if (am == neg || am == negs || am == mvn || am == mov) {
		if (dAssLn.numOperands == 3) {
			return translateDPInstructions(newAssemblyInstruction4Op(aliasAm, op1, zeroRegisterGenerator(op1), op2, op3));
		} else {
			return translateDPInstructions(newAssemblyInstruction3Op(aliasAm, op1, zeroRegisterGenerator(op1), op2));
		}
	} else if (am == mul || am == mneg) {
		return translateDPInstructions(newAssemblyInstruction4Op(aliasAm, op1, op2, op3, zeroRegisterGenerator(op1)));
	} else {
		fprintf(stderr, "Error in checkDPAlias function.");
		return 0;
	}
}

//to translate Data Processing instructions:
uint32_t translateDPInstructions(AssemblyInstruction dAssLn) {
	// check for alias
	printAssemblyInstruction(dAssLn);
	uint32_t aliasFound = checkDPAlias(dAssLn);
	printf("After checking for alias, alias found = %x\n", aliasFound);
	if (aliasFound != 0) { return aliasFound; }

	// initialise the instruction with destination register (bits 0-4)
	operand rd = dAssLn.operands[0];
	uint32_t instruction = rd.data;
	printf("After setting rd, instr = %x\n", instruction);
	if (dAssLn.name == err) {
		fprintf(stderr, "Error in if statmenet in function translateDPInstructions.");
	}

	switch (dAssLn.name) {
	// arithmetic DP instructions
	case add:
	case adds:
	case sub:
	case subs:
		if (dAssLn.operands[2].type == literal) {
			calculateImmInstructions(dAssLn, &instruction);
		} else {
			calculateRegInstructions(dAssLn, &instruction);
		}
		break;
	// wide immediate move
	case movn:
	case movz:
	case movk:
		calculateImmInstructions(dAssLn, &instruction);
		break;
	// logic and Mult DP instructions
	case and:
	case ands:
	case bic:
	case bics:
	case eor:
	case orr:
	case eon:
	case orn:
	case madd:
	case msub:
		calculateRegInstructions(dAssLn, &instruction);
		break;
	default:
		fprintf(stderr, "Error in switchcase in function translateDPInstructions.");
	}
	printf("Instruction has been translated to %x\n\n", instruction);

	return instruction;
}

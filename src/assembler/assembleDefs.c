#include "assembleDefs.h"

int32_t mask(uint32_t data, int n) {
	return data & ((1 << n) - 1);
}

AssMneumonicMapping amneumonicPairs[] = {
	{"Unknown Instruction", err},
	{"str", str}, {"ldr", ldr}, 
	{"b", b}, {"b.eq", beq}, {"b.ne", bne}, {"b.ge", bge}, 
	{"b.lt", blt}, {"b.gt", bgt}, {"b.le", ble}, 
	{"b.al", bal}, {"br", br}, {".int", intd}, {"udf", intd},
	{"add", add}, {"adds", adds}, {"sub", sub}, {"subs", subs}, 
	{"cmp", cmp}, {"cmn", cmn}, {"neg", neg}, {"negs", negs}, 
	{"and", and}, {"ands", ands}, {"bic", bic}, {"bics", bics}, 
	{"eor", eor}, {"orr", orr}, {"eon", eon}, {"orn", orn}, 
	{"tst", tst}, {"movk", movk}, {"movn", movn}, {"movz", movz}, 
	{"mov", mov}, {"mvn", mvn}, {"madd", madd}, {"msub", msub}, 
	{"mul", mul}, {"mneg", mneg}, 
	{NULL, 0} // end marker
};

ShiftMneumonicMapping smneumonicPairs[] = {
	{"noShift", noShift},
	{"lsl", lsl}, {"lsr", lsr}, {"asr", asr}, {"ror", ror},
	{NULL, 0} // end marker
};

OperandTypeMapping omneumonicPairs[] = {
	{Xi, "Xi"}, {Wi, "Wi"},
	{shift, "shift"}, {literal, "literal"}
};

LdrStrTypeMapping ldrStrTypeMappings[] = {
	{normal, "normal"},
	{ZUO, "Zero Unsigned Offset"},
	{UO, "Unsigned Offset"},
	{Pre, "Pre-Indexed"},
	{Post, "Post-Indexed"},
	{RO, "Register Offset"}
	// add more mappings if needed
};

AssMneumonic assStringToEnum(char str[]) {
	for (int i = 0; amneumonicPairs[i].name != NULL; i++) {
		if (strcmp(amneumonicPairs[i].name, str) == 0) {
			return amneumonicPairs[i].value;
		}
	}
	// default case for unknown strings
	return err;
}

char* assEnumToString(AssMneumonic amn) {
	for (int i = 0; amneumonicPairs[i].name != NULL; i++) {
		if (amneumonicPairs[i].value == amn) {
			return amneumonicPairs[i].name;
		}
	}
	// default case for unknown mneumonics
	return "Unknown Instruction";
}

ShiftMneumonic shiftStringToEnum(char str[]) {
	for (int i = 0; smneumonicPairs[i].name != NULL; i++) {
		if (strcmp(smneumonicPairs[i].name, str) == 0) {
			return smneumonicPairs[i].value;
		}
	}
	// default case for unknown strings
	return -1;
}

char* shiftEnumToString(ShiftMneumonic smn) {
	for (int i = 0; smneumonicPairs[i].name != NULL; i++) {
		if (smneumonicPairs[i].value == smn) {
			return smneumonicPairs[i].name;
		}
	}
	// default case for unknown mneumonics
	return "Unknown Instruction";
}

char* opEnumToString(operandType smn) {
	for (int i = 0; omneumonicPairs[i].name != NULL; i++) {
		if (omneumonicPairs[i].value == smn) {
			return omneumonicPairs[i].name;
		}
	}
	return "Unknown Instruction"; // default case for unknown mneumonics
}

char* ldrEnumToString(LdrStrType lst) {
	for (int i = 0; ldrStrTypeMappings[i].name != NULL; i++) {
		if (ldrStrTypeMappings[i].value == lst) {
			return ldrStrTypeMappings[i].name;
		}
	}
	// default case for unknown mneumonics
	return "Unknown Instruction";
}

void printAssemblyInstruction(AssemblyInstruction dAssLn) {
	printf("Instruction name: %s\n", assEnumToString(dAssLn.name));
	printf("Instruction addressing mode: %s\n", ldrEnumToString(dAssLn.addressMode));
	printf("Instruction address: %d\n", dAssLn.address);
	printf("Instruction operands: \n");
		
	if (dAssLn.numOperands == 0) {
		printf("No operands\n");
	} else {
		for (int i = 0; i < dAssLn.numOperands; i++) {
			ShiftMneumonic shiftType;
			operand currentOp = dAssLn.operands[i];
			if ((shiftType = dAssLn.operands[i].shift) != noShift) {
				printf("[%d]Operand Type: %s\nShift Type: %s\nOperand Value:%d\n", \
				i, opEnumToString(currentOp.type), shiftEnumToString(currentOp.shift), currentOp.data);
			} else {
				printf("[%d]Operand Type: %s\nOperand Value:%d\n", i, opEnumToString(currentOp.type), currentOp.data);
			}
		}
	}
}

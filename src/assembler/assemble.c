#include "assembleDefs.h"
#include "assIoHandler.h"
#include "tr_branch.h"
#include "tr_dp.h"
#include "tr_sdt.h"
#include "hashTable.h"

#define DELIMITERS " \t\n,"
#define REG_CHECK(op, s) if (strcmp("w", s) == 0) { op.type = Wi; } else { op.type = Xi; }

// define global variables
char* fileBuffer = NULL;
HashTable* symbolTable = NULL;

/*
construct a binary file writer
building a symbol table Abstract data type
the assembler: 
	- an assembly file reader
	- an instruction parser (string line to instruction)
	- tokenizer (breaking up the string into it label, opcode and operand)
	- instruction assembler 
		- data processing instructions
		- load and store instructions
		- branch instructions
	- one or two pass assembly process
*/

// for translating the .int directives
uint32_t translatingDirectives(AssemblyInstruction dAssLn) {
	assert(dAssLn.name == intd);
	return dAssLn.operands[0].data;
}

/*
	this function will take in an assembly instruction, and it will call the corresponding function to decode 
	the assembly instruction. the result will be a number to represent the binary instruction.
*/
uint32_t assemblyInstructionGenerator(AssemblyInstruction dAssLn) {
	// run the appropriate translate function depending on the mnemonic: (eg, using switch)
	uint32_t returnVal;
	switch (dAssLn.name) {
	case str:
	case ldr:
		// if the mneumonic is str or ldr:
		returnVal = translateLSInstructions(dAssLn);
		break;
	case b:
	case beq:
	case bne:
	case bge:
	case blt:
	case bgt:
	case ble:
	case bal:
	case br:
		// if the mneumonic is b, bcond or br:
		returnVal = translateBranchInstructions(dAssLn);
		break;
	case intd:
		// if the mneumonic is intd:
		returnVal = translatingDirectives(dAssLn);
		break;
	default:
		// otherwise, mneumonic is data processing instruction:
		returnVal = translateDPInstructions(dAssLn);
		break;
	}
	return returnVal;
}

/*
	this function (rename pending) will use the global instruction buffer to generate the output file
	only containing binary code, and will be the final output of the program. It will look up 
	symbols in the symbol tree data structure to replace labels with the appropriate addresses, 
	and convert all other nstructions to binary
*/
bool checkRegLit(char* stri, int* num, operand* returnOp) {
	char f1[2] = "a";

	if (sscanf(stri, "#0x%x", (uint32_t*) num) == 1) {
		// if the operand is just a number:
		returnOp->type = literal;
		returnOp->data = *num;
		return true;
	}

	if (sscanf(stri, "0x%x", (uint32_t*) num) == 1) {
		// if the operand is just a number:
		returnOp->type = literal;
		returnOp->data = *num;
		return true;
	}

	if (sscanf(stri, "%d", num) == 1) {
		// if the operand is just a number:
		returnOp->type = literal;
		returnOp->data = *num;
		return true;
	}

	if (sscanf(stri, "%c%d", f1, num) == 2) {
		if (strcmp("w", f1) == 0) {
			// if the operand is a W register:
			returnOp->type = Wi;
		} else if (strcmp("x", f1) == 0) {
			// if the operand is a X register:
			returnOp->type = Xi;
		} else if (strcmp("#", f1) == 0) {
			returnOp->type = literal;
		} else {
			return false;
		}
		
		returnOp->data = *num;
		return true;
	}
	return false;
}

void checkLSOP(char* stri, AssemblyInstruction* instr) {
	printf("%s\n", stri);
	operand returnOp1;
	returnOp1.shift = noShift;
	operand returnOp2;
	returnOp2.shift = noShift;
	int n1, n2;
	char s1[3], s2[3];

	if (sscanf(stri, "[%c%d, #0x%x]", s1, &n1, (uint32_t*) &n2) == 3) {
		// check for format [xn, #m] or [xn, #m]! (Hex literals)

		// check for format [xn, #m]
		if (stri[strlen(stri) - 1] != '!') {
			REG_CHECK(returnOp1, s1);
			returnOp1.data = n1;
			instr->numOperands++;
			instr->operands[1] = returnOp1;
			returnOp2.type = literal;
			returnOp2.data = n2;
			instr->numOperands++;
			instr->operands[2] = returnOp2;
			instr->addressMode = UO;

		// check for format [xn, #m]!
		} else {
			REG_CHECK(returnOp1, s1);
			returnOp1.data = n1;
			instr->numOperands++;
			instr->operands[1] = returnOp1;
			returnOp2.type = literal;
			returnOp2.data = n2;
			instr->numOperands++;
			instr->operands[2] = returnOp2;
			instr->addressMode = Pre;
		}

	} else if (sscanf(stri, "[%c%d, #%d]", s1, &n1, &n2) == 3) {
		// check for format [xn, #m] or [xn, #m]! (Decimal literals)

		if (stri[strlen(stri) - 1] != '!') {
			// check for format [xn, #m]
			REG_CHECK(returnOp1, s1);
			returnOp1.data = n1;
			instr->numOperands++;
			instr->operands[1] = returnOp1;
			returnOp2.type = literal;
			returnOp2.data = n2;
			instr->numOperands++;
			instr->operands[2] = returnOp2;
			instr->addressMode = UO;

		} else {
			// check for format [xn, #m]!
			REG_CHECK(returnOp1, s1);
			returnOp1.data = n1;
			instr->numOperands++;
			instr->operands[1] = returnOp1;
			returnOp2.type = literal;
			returnOp2.data = n2;
			instr->numOperands++;
			instr->operands[2] = returnOp2;
			instr->addressMode = Pre;
		}

	} else if (sscanf(stri, "[%c%d], #0x%x", s1, &n1, (uint32_t*) &n2) == 3) {
		// check for format [xn], #m (Hex literal)
		REG_CHECK(returnOp1, s1);
		returnOp1.data = n1;
		instr->numOperands+=2;
		instr->operands[1] = returnOp1;
		returnOp2.type = literal;
		returnOp2.data = n2;
		instr->operands[2] = returnOp2;
		instr->addressMode = Post;

	} else if (sscanf(stri, "[%c%d], #%d", s1, &n1, &n2) == 3) {
		// check for format [xn], #m (Decimal literal)
		REG_CHECK(returnOp1, s1);
		returnOp1.data = n1;
		instr->numOperands+=2;
		instr->operands[1] = returnOp1;
		returnOp2.type = literal;
		returnOp2.data = n2;
		instr->operands[2] = returnOp2;
		instr->addressMode = Post;

	} else if (sscanf(stri, "[%c%d, %c%d]", s1, &n1, s2, &n2) == 4) {
		// check for format [xn, xm]
		REG_CHECK(returnOp1, s1);
		returnOp1.data = n1;
		instr->numOperands++;
		instr->operands[1] = returnOp1;
		REG_CHECK(returnOp2, s2);
		returnOp2.data = n2;
		instr->numOperands++;
		instr->operands[2] = returnOp2;
		instr->addressMode = RO;

	} else if (sscanf(stri, "[%c%d]", s1, &n1) == 2) {
		// check for format [xn]
		REG_CHECK(returnOp1, s1);
		returnOp1.data = n1;
		instr->numOperands++;
		instr->operands[1] = returnOp1;
		instr->addressMode = ZUO;

	} else if (sscanf(stri, "#0x%x", (uint32_t*) &n1) == 1) {
		// for Hex literal
		returnOp1.type = literal;
		returnOp1.data = n1;
		instr->operands[1] = returnOp1;
		instr->numOperands++;

	} else if (sscanf(stri, "#%d", &n1) == 1) {
		// for Decimal literal
		returnOp1.type = literal;
		returnOp1.data = n1;
		instr->operands[1] = returnOp1;
		instr->numOperands++;

	} else { 
		printf("%s is identified as a label\n", stri);
		uint32_t* data = getEntryHashTable(symbolTable, stri);
		printf("Hash table returns %x\n", *data);
			
		if (data != NULL) {
			returnOp1.type = literal;
			returnOp1.data = *data - instr->address;
			instr->operands[1] = returnOp1;
			instr->numOperands++;
		}
	}

	printf("error with LS Opcode\n");
}

operand getOperand(char* stri, AssemblyInstruction* instr) {
	operand returnOp;
	returnOp.shift = noShift;
	int num;

	if (strcmp(stri, "xzr") == 0) {
		returnOp.type = Xi;
		returnOp.data = 31;
		return returnOp;
	}
	else if (strcmp(stri, "xzr") == 0) {
		returnOp.type = Wi;
		returnOp.data = 31;
		return returnOp;
	}

	// checks if the operand is a type of shift:
	for (int i = 0; i <= NUM_SHIFTS; i++) {
		if (strcmp(stri, smneumonicPairs[i].name) == 0) {
			returnOp.type = shift;
			returnOp.shift = shiftStringToEnum(stri);
			char* newWord = strtok(NULL, DELIMITERS);
			if (strchr(newWord, 'x') != NULL) {
				sscanf(newWord, "#0x%x", (uint32_t*) &num);
			} else {
				sscanf(newWord, "#%d", &num);
			}
			returnOp.data = num;
			return returnOp;
		}
	}

	// if the opcode is neither a literal, or register, must be a label
	if (!checkRegLit(stri, &num, &returnOp)) { 
		printf("%s is identified as a label\n", stri);
		uint32_t* data = (uint32_t*) getEntryHashTable(symbolTable, stri);
		printf("Hash table returns %x\n", *data);
		
		if (data != NULL) {
			returnOp.type = literal;
			returnOp.data = *data - instr->address;
		} else {
			returnOp.data = 404;
		}
	}
	return returnOp;
}

void trim_leading_spaces(char** stri) {
	while (**stri != '\0' && strchr(DELIMITERS, **stri) != NULL) {
		(*stri)++;
	}
}

uint32_t secondPass(char* assLnStr, uint32_t address) {
	printf("second pass for instruction: %s\n", assLnStr);
	char* assMneu = strtok(assLnStr, DELIMITERS);
	printf("name: {%s}\n", assMneu);
	AssMneumonic name = assStringToEnum(assMneu);
	AssemblyInstruction instr;
	instr.name = name;
	instr.addressMode = normal;
	instr.numOperands = 0;
	instr.address = address;
	char* newWord;

	printf("Finished second pass inits\n");

	if (name != ldr && name != str) {
		for (int i = 0; (newWord = strtok(NULL, DELIMITERS)) != NULL; i++) {
			printf("About to process getOperand(\"%s\");\n", newWord);
			operand newOp = getOperand(newWord, &instr);
			instr.numOperands += 1;
			instr.operands[i] = newOp;
		}
	} else {
		// if a load or store instruction:
		char* newWord = strtok(assMneu + strlen(assMneu) + 1, DELIMITERS);
		operand targetReg = getOperand(newWord, &instr);
		instr.numOperands += 1;
		instr.operands[0] = targetReg;
		// to account for the null pointers
		char* savePoint = assMneu + strlen(assMneu) + strlen(newWord) + 2;
		trim_leading_spaces(&savePoint);
		checkLSOP(savePoint, &instr);
	}

	printAssemblyInstruction(instr);
	printf("\n");

	uint32_t binInstr = assemblyInstructionGenerator(instr);
	return binInstr;
}

/*
	first pass will only add symbols to the symbol tree,
	second pass will convert instructions to numbers, and then write to binary file
*/
int main(int argc, char* argv[]) {
	// checks the number of arguments to the function is correct
	if (argc != 3) {
		fprintf(stderr, "Usage: assemble input_file output_file");
		exit(-1);
	}

	// ensures the file is empty upon first write
	FILE* file = fopen(argv[2], "w");
	fclose(file);

	// creates symbol table
	symbolTable = createHashTable(10);

	// load the file into a buffer, and create the symbol tree
	char** lineBuffer;
	printf("About to enter first pass\n");
	loadFileFirstPass(argv[1], &lineBuffer);
	printf("Completed first pass\n");

	char** currentLine = lineBuffer;
	uint32_t address = 0;

	// for each instruction, decode it into its binary number
	while (*currentLine != NULL) {
		uint32_t binInstr = secondPass(*currentLine, address);
		printf("About to process %s, binInstr = %x\n", *currentLine, binInstr);
		outputBinaryFile(argv[2], binInstr);
		currentLine++;
		address++;
	}
	printf("Leaving main loop\n");
	freeHashTable(symbolTable);

	return EXIT_SUCCESS;
}

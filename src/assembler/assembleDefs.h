#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_SHIFTS 4
#define MAX_NUM_OPS 4

typedef enum {Xi, Wi, shift, literal} operandType;
typedef enum {normal, ZUO, UO, Pre, Post, RO} LdrStrType;
typedef enum {noShift, lsl, lsr, asr, ror} ShiftMneumonic;

typedef enum {
	// unknown instruction enum:
	err=-1,
	// load/Store Mneumonics
	str, ldr, 
	// branching + directive mneumonics
	b, beq, bne, bge, blt, 
	bgt, ble, bal, br, intd,
	// data processing mneumonics
	add, adds, sub, subs, 
	cmp, cmn, neg, negs, 
	and, ands, bic, bics, 
	eor, eon, orr, orn, 
	tst, movk, movn, movz, 
	mov, mvn, madd, msub, 
	mul, mneg
	} AssMneumonic;

typedef struct {
	operandType type;
	ShiftMneumonic shift;
	uint32_t data;
} operand;

typedef struct {
	AssMneumonic name;
	LdrStrType addressMode;
	uint32_t address;
	operand operands[MAX_NUM_OPS];
	int numOperands;
} AssemblyInstruction;

typedef struct {
	char* name;
	AssMneumonic value;
} AssMneumonicMapping;

typedef struct {
	char* name;
	ShiftMneumonic value;
} ShiftMneumonicMapping;

typedef struct {
	operandType value;
	char* name;
} OperandTypeMapping;

typedef struct {
	LdrStrType value;
	char* name;
} LdrStrTypeMapping;

extern AssMneumonicMapping amneumonicPairs[];
extern ShiftMneumonicMapping smneumonicPairs[];
extern OperandTypeMapping omneumonicPairs[];
extern LdrStrTypeMapping lmneumonicPairs[];
extern AssMneumonic assStringToEnum(char* str);
extern ShiftMneumonic shiftStringToEnum(char* str);
extern char* assEnumToString(AssMneumonic amn);
extern char* shiftEnumToString(ShiftMneumonic smn);
extern char* opEnumToString(operandType ot);
extern char* ldrEnumToString(LdrStrType lst);
extern void printAssemblyInstruction(AssemblyInstruction dAssLn);

extern int32_t mask(uint32_t data, int n);

// declare structures for retrieving bit fields
typedef struct {
	uint32_t rd:5;
	uint32_t rn:5;
	uint32_t imm12:12;
	uint32_t sh:1;
	uint32_t opi:3;
	uint32_t :3;
	uint32_t opc:2;
	uint32_t sf:1;
} arithmetic;

typedef struct {
	uint32_t rd:5;
	uint32_t imm16:16;
	uint32_t hw:2;
	uint32_t opi:3;
	uint32_t :3;
	uint32_t opc:2;
	uint32_t sf:1;
} wide_move;

/*
	Decode data processing immediate instruction.
	Precondition: the instruction stored in the cpu's
	currentInstruction is of the data processing immediate group.
	This function determines whether the instruction
	is an arithmetic instruction or a wide move instruction
	and calls the corresponding execution function.
*/
extern void decodeExecuteDPImm(void);

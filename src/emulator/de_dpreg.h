// declare structures for retrieving bit fields
typedef struct {
	uint32_t rd:5;
	uint32_t rn:5;
	uint32_t operand:6;
	uint32_t rm:5;
	uint32_t opr:4;
	uint32_t :3;
	uint32_t m:1;
	uint32_t opc:2;
	uint32_t sf:1;
} data_process_reg;

/*
	Decode data processing instruction register.
	Determine whether the data processing
	register instruction is an arithmetic,
	logical or multiply operation and run the
	corresponding execute function.

	Pre: the instruction is of the instruction group
	data processing register, i.e. op0 = x101
*/
extern void decodeExecuteDPReg(void);

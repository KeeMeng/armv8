// declare structures for retrieving bit fields
typedef struct {
	uint32_t rt:5;
	uint32_t xn:5;
	uint32_t offset:12;
	uint32_t l:1;
	uint32_t :1;
	uint32_t u:1;
	uint32_t :5;
	uint32_t sf:1;
	uint32_t msb:1;
} single_data_transfer;

typedef struct {
	uint32_t rt:5;
	uint32_t simm19:19;
	uint32_t :6;
	uint32_t sf:1;
	uint32_t :1;
} load_literal;

/*
	Decode load store instructions
	Determine whether the instruction is
	a single data transfer or load literal
	instruction and call the corresponding
	function.
*/
extern void decodeExecuteSDT(void);

// declare structures for retrieving bit fields
typedef struct {
	uint32_t simm26:26;
	uint32_t :4;
	uint32_t :2;
} unconditional_branch;

typedef struct {
	uint32_t :5;
	uint32_t xn:5;
	uint32_t :22;
} reg_branch;

typedef struct {
	uint32_t cond:4;
	uint32_t :1;
	uint32_t simm19:19;
	uint32_t :8;
} conditional_branch;

/* 
	Decode branch instruction
	Determine whether the branch is unconditional,
	register or conditional. Then run the corresponding
	execution function.
*/
extern void decodeExecuteBranch(void);

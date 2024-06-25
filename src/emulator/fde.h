// enum for the 4 main instruction groups
typedef enum {
	DPImm,
	DPReg,
	SDT,
	Branch
} InstructionGroup;

// declare all fde functions
extern void fetch(void);
extern void decode(void);

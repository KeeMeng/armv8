#include "common.h"
#include "fde.h"
#include "ioHandler.h"

/*
	Entry point - the main module.

	The main function must read the command
	line arguments and validate them.

	It must then attempt to open a binary file
	and validate the response, returning /
	outputting an error as necessary. This
	can be done via the loadBinaryProgram function
	in the ioHandler module. This will return
	1 upon success and 0 upon failure.

	It must then call the fde loop function
	defined below.

	Upon the termination of the loop, it
	must determine the correct file
	handle for output. This will be
	stdout if no output file was
	given via the command line. It will
	be the output file handle if an output
	file is specified in the command line
	arguments. It should then call the
	output function from the io
*/

Armv8 cpu;
uint32_t currentInstruction = ENTRY_ADDR;

void fdeLoop(void) {
	// initially set the run flag to 1
	cpu.run_flag = true;

	// z flag is initially set
	cpu.pstate.z = true;

	printf("fdeLoop: run_flag: %d\n", cpu.run_flag);
	while (cpu.run_flag) {
		printf("================================\n");
		fetch();
		printf("fdeLoop: currentInstruction = %x\n = ", currentInstruction);
		printBinary(currentInstruction, 0);
		decode();

		// Only increment pc if we haven't jumped
		if (!cpu.jmp_flag) { 
			printf("fdeLoop: incrementing pc\n");
			cpu.pc.d_64 += 4;
		}

		cpu.jmp_flag = false;
	}
}

int main(int argc, char* argv[]) {
	// Check for valid number of arguments
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Invalid number of arguments.");
		return -1;
	}

	// Attempt to read file
	if (!loadBinaryProgram(argv[1])) { return -1; };
	
	// for (int i = 0; i < 16; i++) {
	// 	// print bytes of memory
	// 	printf("%x ", cpu.mem[i]);
	// };
	
	// Run main loop
	fdeLoop();

	// Print output
	if (argc == 3) {
		FILE* output_file = fopen(argv[2], "w+");
		assert(output_file != NULL);
		logOutput(output_file);
	} else {
		logOutput(stdout);
	}
	return EXIT_SUCCESS;	
}

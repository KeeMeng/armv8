#include "common.h"
#include "ioHandler.h"
#include <inttypes.h>

// to ensure the write was successful, otherwise the program exits
#define CHECK_WRITE(str, out) if (fputs(strvar, output) == EOF) { return false; }

// ret is a pointer to 0, and will hold the value held in the memory location j
#define ACCESS_MEM_CELL(j, mem, ret) for (int i = 3; i >= 0; i--) { *ret |= ((uint32_t)mem[j+i]) << (8 * i); };

bool loadBinaryProgram(const char* filename) {
	// assert that filename is not a nullptr
	assert(filename != NULL);

	// attempt to open file
	FILE* inFile = fopen(filename, "rb");

	// check if file unopened, return error
	if (!inFile) {
		fprintf(stderr, "loadBinaryProgram: failed to open file %s.", filename);
		return false;
	}
	
	// get file size
	printf("%s \n", filename);
	fseek(inFile, 0, SEEK_END);
	int fileSize = ftell(inFile);
	rewind(inFile);

	printf("\nloadBinaryProgram: file size = %d\n", fileSize);

	// read file into memory buffer
	fread(cpu.mem, 1, fileSize, inFile);

	// close file
	fclose(inFile);
	return true;
}

int logOutput(FILE* output) {
	// a string buffer to hold the output strings and initialise to the first output line
	char strvar[100] = "Registers:\n";
	CHECK_WRITE(strvar, output);

	// output each register, followed by its contents
	for (int i = 0; i < 31; i++) {
		sprintf(strvar, "X%02d     = %016" PRIx64 "\n", i, cpu.regs[i].d_64);
		CHECK_WRITE(strvar, output);
	}

	// output pc and its contents
	sprintf(strvar, "PC      = %016" PRIx64 "\n", cpu.pc.d_64);
	CHECK_WRITE(strvar, output);

	// output pstate and its contents
	sprintf(strvar, "PSTATE :  %c%c%c%c\n", 
	cpu.pstate.n ? 'N' : '-', 
	cpu.pstate.z ? 'Z' : '-',
	cpu.pstate.c ? 'C' : '-', 
	cpu.pstate.v ? 'V' : '-');
	CHECK_WRITE(strvar, output);

	// output this string
	sprintf(strvar, "Non-zero memory:\n");
	CHECK_WRITE(strvar, output);

	// output all non-zero memory cells
	for (int j = 0; j < (2 << 21); j += 4) {
		uint32_t val = 0;
		ACCESS_MEM_CELL(j, cpu.mem, &val);
		if (val != 0) {
			sprintf(strvar, "0x%06x : %16" PRIx32 "\n", j, val);
			CHECK_WRITE(strvar, output);
		}
	}
	return 1;
}

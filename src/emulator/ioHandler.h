/*
	This function reads a program as a binary file.
 	The file is copied to the emulated cpu's memory
 	at address 0.
*/
extern bool loadBinaryProgram(const char* filename);

// This function outputs the program output to the given output stream.
extern int logOutput(FILE* output);

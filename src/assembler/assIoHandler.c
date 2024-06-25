#include "assembleDefs.h"
#include "assIoHandler.h"
#include "hashTable.h"

int countLines(const char* str) {
	const char* current = str;
	int total = 0;

	while (*current != '\0') {
		if (*current == '\n') {
			total++;
		}
		
		current++;
	}

	return total;
}

bool isLabel(const char* str) {
	// to be a label, the string must be a single contiguous word
	// - i.e. a block of non-space characters surrounded by 0 or more spaces
	// the first char must be alphabetical
	// the last non-space char must be a :
	
	const char* current = str;

	// check length
	// minimum length is 2, e.g. a:
	if (strlen(str) < 2) {
		return false;
	}

	// check that first character is alphabetic, an underscore or a period
	if (!(isalpha(*current) || *current == '_' || *current == '.' )) {
		return false;
	}

	bool lineEnded = false;
	bool endsWithColon = false;

	while (*current != '\n' && *current != '\0') {
		if (isspace(*current)) {
			// if the current character is a space, there should be no more non-space chars on the line
			lineEnded = true;
		} else {
			// check if there is more than one contiguous block
			if (lineEnded) {
				// the line has ended (i.e. space character previously encountered) but we now have another non-space character.
				return false;
			} else {
				// check if :
				if (*current == ':') {
					endsWithColon = true;
				}
			}
		}
		current++;
	}

	return endsWithColon;
}

// extract label
// pre - str is a label
// replace : with \0 to extract only the label name
void extractLabel(char* str) {
	char* current = str;

	while (*current != '\n' && *current != '\0') {
		if (*current == ':') {
			*current = '\0';
		}

		current++;
	}
}

// important - this function allocates an array to the global pointer filebuffer. this is used throughout the lifetime of the program
// deallocate it at the end of the program

// function declarations
void loadFileFirstPass(const char* filePath, char*** lineBuffer) {
	// attempt to open file
	FILE* inFile = fopen(filePath, "r");

	// check that file opened
	if (inFile) {
		// get file size
		fseek(inFile, 0, SEEK_END);
		size_t fileSize = ftell(inFile);
		rewind(inFile);

		// allocate buffer for file characters
		fileBuffer = malloc(fileSize + 1);

		// load all file data into file buffer
		size_t toRead = fileSize;

		while (toRead > 0) {
			toRead -= fread(fileBuffer + (fileSize - toRead), sizeof(char), toRead, inFile);
		}

		// ensure that buffer is null-terminated
		fileBuffer[fileSize] = '\0';

		// count the number of lines
		int numLines = countLines(fileBuffer);

		// allocate line buffer
		*lineBuffer = (char**) malloc((numLines + 1) * sizeof(char*));
		char** currentLine = *lineBuffer;

		// process lines of buffer
		char* startPos = fileBuffer;

		// get pointer to current char
		char* current = fileBuffer;

		// boolean flags
		bool running = true;
		bool firstCharFound = false;

		uint64_t address = 0;

		while (running) {
			// get current character
			char c = *current;
			
			// check if new line has been found or next character is the end of the loop
			if (c == '\n' || c == '\0') { 
				// cases
				if (firstCharFound) {
					// we have found the first character of the line so line is non-empty - check if it is a label
					// i.e. it contains a single continguous word
					
					// check if label was found
					if (isLabel(startPos)) {
						// extract label and add to symbol table
						extractLabel(startPos);
						printf("Found label %s\n", startPos);
						insertHashTable(symbolTable, startPos, (void*) address);
						printf("%p\n", getEntryHashTable(symbolTable, startPos));
					} else {
						// line is instruction, directive or malformed - add to list of lines for further processing
						address += 1;

						// set end of string
						*current = '\0';

						// add line to lines buffer
						*currentLine = startPos;
						currentLine++;
					}
				}

				firstCharFound = false;
			} else if (isspace(c)) {
				if (!firstCharFound) {
					// ignore spaces before first character 
					startPos = current;
				}
			} else {
				// non-space and not end of line - set start pos if not already set
				if (!firstCharFound) {
					startPos = current;
					firstCharFound = true;
				}
			}
			// check if reached end of string
			if (c == '\0') {
				running = false;
			}
			// increment current
			current++;
		}
		// null-terminate lines
		*currentLine = NULL;
	} else {
		// output error
		fprintf(stderr, "Error - Failed to open file %s.\n", filePath);
	}

	// close file
	fclose(inFile);
}

int outputBinaryFile(char* outfile, uint32_t binInstr) {
	// open the file in binary write mode
	FILE* file = fopen(outfile, "ab");
	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	// write the number to the file
	if (fwrite(&binInstr, sizeof(binInstr), 1, file) != 1) {
		perror("Error writing to file");
		fclose(file);
		return 1;
	}

	// close the file
	fclose(file);
	printf("Number written to file successfully.\n\n");

	return 0;
}

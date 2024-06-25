# Part 2 - Assembler

## Modules:
- An assembly file reader
- An instruction parser (string line to instruction)
- Tokenizer (breaking up the string into it label, opcode and operand)
- Instruction assembler 
    - Data processing instructions
    - Load and store instructions
    - Branch instructions
- One or two pass assembly process

## The Job of the Assembler
The assembler converts assembly instructions to a binary executable. It must handle three cases:
    - Instructions, e.g. "add x0, x1, #123"
    - Labels, e.g. "partOne:" (must end with a : and
      start with a letter)
    - Directives: ".int 4" (store their given values in the binary
      file) 

## Labels
Labels simply just represent the address of the next instruction in memory, e.g. suppose we have the following program. The hex numbers on the left represent the addresses of the instructions relative to the start of the program.
    0x0012  add x1, x0, #123
    jumpOne:
    0x0016  sub x1, x1, #1
    0x001a  b jumpOne

When we assemble the program, after the first pass, the label jumpOne will be read and will be mapped to the address 0x0016.
This means that our code changes to:
    0x0012  add x1, x0, #123
    0x0016  sub x1, x1, #1
    0x001a  b 0x0016

We can see that this has the same effect as branching to the label. This is useful as it means that we can use labels to represent points in our program without having to keep track of addresses manually.

## Directives
Directives allow us to define constants and variables. We do this by storing the values as literals stored in the program's binary. When the program is loaded into memory, the literals embedded in its binary also are loaded as part of the program, so we can access them using labels.

The .int directive stores a 32-bit integer.

For example, consider the following code:
    0x0012 add x0, x1, a
        a:
    0x0016 .int 10

After the first pass, we will map a to the address 0x0016 in the symbol table. This allows us to perform operations on the constant stored by the .int.

Another example program is as follows:
    b rest

    my_value:
        .int 0x3f
        .int 0
    
    rest:
        ldr x0, my_value

Here we can see how we can use labels and directives in our programs. 

We assume that the program is loaded into memory at address 0.
So the instruction "b rest" is stored at address 0.

Adding the addesses and substituting the labels gives:
    0x00 b 0x0c
    0x04 0x3f
    0x08 0x00
    0x0c ldr x0, 0x04

Here we can see that we first branch to the ldr instruction at address 0x0c. We load the address 0x04 into the register x0. Note that 0x04 is an address here, not a literal as it is not prefixed with a #. Therefore, this program loads the 8 bytes starting at address 0x04 (given by the my_value label).

## One or Two Pass Assembly Process
We will use a two pass assembly process. In the first pass, we break the program into lines and we will build up a symbol table. The symbol table is a hash map that maps labels to addresses.

In the second pass, we break each line into tokens (e.g. opcode, operands, etc.) and replace all labesl with addresses.


## Overview of the Algorithm:

### Loading from a file and First Pass:
    Note: we haven't fully decided which algorithm to use here. This is just the most efficient one I could come up with - Will

    Other note: we decided to combine the file loading and the first pass, since both involve iterating through the read text we might as well combine the two for efficiency.

    First read the length of the text file. Then load that many characters into a dynamically allocated character buffer.

    Iterate through all characters and count the number of lines, i.e. the \n. Allocate this many character pointers to form an array of character pointers.
    
    Iterate through all characters again and replace all \nwith \0. Add the start of each line to the array of pointers,skipping lines that are empty (i.e. no non-space characterbetween the previous and next \n).

    As we go, we keep track of the address in the binary that the current line represents. For instructions and .int directives, we increment the address by 4 bytes. For labels and empty lines, we do not add anything. In the case where the line currently being read is a label, i.e. it starts with an alphabetical character and ends with a :, then we extract the label name and add it to the symbol table with the corresponding address.

    The result of this is that we get a dynamic 2d array of null-terminated lines. Each line is non-empty, meaning that in a well-formed program every line is either an instruction, a label or an integer directive.

### Second Pass:
    Allocate an array of 32-bit integers with the same number of 32-bit integers as there are lines in line array.
    Iterate through each line and split into into tokens (words) using spaces and commas as delimeters.
    Check the first word:
        - If it is .int then we have a directive - parse the next token as an integer and convert to a 32-bit value. Store this in the list of binary integers.

        - If it is an opcode, then look the opcode up in a pre-filled hashmap. This should map opcodes to function pointers.
        - The function pointed to should take 4 operands and should execute the instruction. We also scan the operands to check if any of them are labels. If so, we replace them with the corresponding address in the symbol table.
            - We will define functions to encode all individual functions
              as 32-bit binary numbers and add them to the list/array.
        
### Output
    We iterate through the list of binary numbers and append them one at a time to the binary file. The result is the executable.
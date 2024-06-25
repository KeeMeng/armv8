CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble:
	cd assembler && make assemble
	cp assembler/assemble .

emulate:
	cd emulator && make emulate
	cp emulator/emulate .

clean:
	$(RM) *.o assemble emulate
	cd assembler && make clean
	cd emulator && make clean

graphs:
	cd assembler && make graphs
	cd emulator && make graphs

cb:
	cd assembler && make cb
	cp assembler/assemble .
	cd emulator && make cb
	cp emulator/emulate .

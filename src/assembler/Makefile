CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble graphs

assemble: assemble.o hashTable.o assembleDefs.o assIoHandler.o tr_dp.o tr_branch.o tr_sdt.o

clean:
	$(RM) *.o assemble emulate
	
graphs: assemble
	rm -rf graphs
	mkdir -p graphs
	cb --allincludes | graphpairs graphs/allincludes
	dot -Tpng graphs/allincludes.dot > graphs/allincludes.png
	cb --directincludes | graphpairs graphs/directincludes
	dot -Tpng graphs/directincludes.dot > graphs/directincludes.png

cb:
	cb --allclean

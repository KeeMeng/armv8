CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate graphs

emulate: emulate.o fde.o ioHandler.o common.o de_dpimm.o de_dpreg.o de_sdt.o de_branch.o

clean:
	$(RM) *.o assemble emulate
	
graphs: emulate
	rm -rf graphs
	mkdir -p graphs
	cb --allincludes | graphpairs graphs/allincludes
	dot -Tpng graphs/allincludes.dot > graphs/allincludes.png
	cb --directincludes | graphpairs graphs/directincludes
	dot -Tpng graphs/directincludes.dot > graphs/directincludes.png

cb:
	cb --allclean

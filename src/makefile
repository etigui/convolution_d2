##########################
# makefile convolution2d #
##########################

#http://randu.org/tutorials/c/make.php

#Source, executable, includes
INCL=main.h kernel.h queue.h ppm.h
SRC=main.c kernel.c queue.c ppm.c
OBJS=$(SRC:.c=.o)
EXE=convolution2d

#Compiler, linker defines, library defines
CFLAGS=-Wall
LIBS=-lrt -lm -lpthread
CC=gcc
RM=rm -rf
DATE=$(shell date +'%d.%m.%Y at %H:%M:%S')

#All target
all : $(EXE)

#Compile and assemble C source files into object files
%.o: %.c
	@echo '//Building file: $<'
	$(CC) $(CFLAGS) -c $*.c
	@echo '//Finished building: $<'
	@echo ' '

# Link all object files with external libraries into binaries
$(EXE): $(OBJS)
	@echo '//Building target: $@'
	$(CC) -o $(EXE) $(OBJS) $(LIBS)
	@echo '//Finished building target: $@ the $(DATE)'

#Objects depend on these libraries
$(OBJS): $(INCL)

#Clean up objects, exectuables
clean:
	$(RM) $(OBJS) $(EXE)
	
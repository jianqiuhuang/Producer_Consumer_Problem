GCC = gcc
FLAGS = -std=c99 -Wall -g
LIB = -lpthread
EXEC = program2

all: source.o Driver.o
	$(GCC) $(FLAGS) $(LIB) -o $(EXEC) source.o Driver.o
Driver.o: Driver.c
	$(GCC) $(FLAGS) $(LIB) -c Driver.c
source.o: source.c
	$(GCC) $(FLAGS) $(LIB) -c source.c
clean:
	rm -f *.o $(EXEC)

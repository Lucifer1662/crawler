CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -I. 
DEPS= vector.h vector.c test.c
EXE = test
$(EXE): vector.h vector.c test.c
	$(CC) $(CFLAGS) $(DEPS) -o $(EXE)

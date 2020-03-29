CC=C:\cygwin64\bin\gcc.exe
CFLAGS=-Wall -Wextra -std=gnu99 -I. 
DEPS= test1.c get_links.c get_links.h ../vector/vector.h ../vector/vector.c ../vector/vector_strings.h ../vector/vector_strings.c
EXE = test
$(EXE): ../vector/vector.h ../vector/vector.c test1.c get_links.c get_links.h ../vector/vector_strings.h ../vector/vector_strings.c
	$(CC) $(CFLAGS) $(DEPS) -o $(EXE)

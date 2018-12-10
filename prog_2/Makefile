CC = gcc -ggdb 
LIBS = -pthread
CFLAGS = -Og -std=gnu11 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function $(LIBS)

all: zad1

zad1: zad1.o semaphore.o
	$(CC) $(CFLAGS) -o $@ $^ -ldl

clean:
	rm -f zad1
	rm -f *.o
	rm -f *~
	rm -f .gdb_history

# vim: ts=8 sw=8 noet

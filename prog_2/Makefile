CC = gcc -ggdb 
CFLAGS = -Og -std=gnu11 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -pthread


all: zad1 zad2 zad8 zad9 zad10 zad11 zad5

zad1: zad1.c semaphore.c
zad2: zad2.c semaphore.c
zad8: zad8.c semaphore.c
zad9: zad9.c
zad10: zad10.c semaphore.c
zad11: zad11.c semaphore.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl
zad5: zad5.c
	$(CC) $(CFLAGS) -o $@ $^ -ldl -lrt

clean:
	rm -f zad1 zad2 zad8 zad9 zad10 zad11 zad5
	rm -f *.o
	rm -f *~
	rm -f .gdb_history

# vim: ts=8 sw=8 noet


CC=gcc
CCOPTS=	-Wall -std=gnu99 

build: main.c mmu.c mmu.h physical_memory_mng.h physical_memory_mng.c
	$(CC) $(CCOPTS)	-o test *.c

clean:
	rm -f test

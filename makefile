
CC=gcc
CCOPTS=	-Wall -std=gnu99 

main.o:	main.c	mmu.h	
	$(CC) $(CCOPTS)	-o	test 	main.c	mmu.c

clean:
	rm -f main

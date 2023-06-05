#include "mmu.h"
#include <time.h>

#include <stdlib.h>

// Testing memory

int main()
{
	MMU *mmu;
	char c;
	int missCount = 0;
	int countRandRead = 100000;
	time_t timeForWrite;
	time_t timeForSequentialRead;
	time_t timeForRandomRead;
	char *buffer = calloc(VIRTUAL_MEMORY_SIZE, sizeof(char));

	mmu = initializeMMU();


	// Access virtual memory
	timeForWrite = time(NULL);
	for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++) {
		c = rand() % 256;
		buffer[i]=c;
		MMU_writeByte(mmu, i, c);
	}
	timeForWrite = time(NULL) - timeForWrite;

	// Read from virtual memory
	timeForSequentialRead = time(NULL);

	for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++) {
		char r = MMU_readByte(mmu, i);
		if (r != buffer[i]) {
			missCount++;
		}
	}
	timeForSequentialRead = time(NULL) - timeForSequentialRead;
	timeForRandomRead = time(NULL);

	for (int i = 0; i < countRandRead; i++) {
		int addr = rand() % VIRTUAL_MEMORY_SIZE;
		char r = MMU_readByte(mmu, addr);
		if (r != buffer[addr]) {
			missCount++;
		}
	}
	timeForRandomRead = time(NULL) - timeForRandomRead;

	printf("-------------------------------------------\n");
	printf("seconds for write data: %ld\n", timeForWrite);
	printf("seconds for read data (%d sequential access): %ld\n", VIRTUAL_MEMORY_SIZE,  timeForSequentialRead);
	printf("seconds for read data (%d random access): %ld\n", countRandRead, timeForRandomRead);
	printf("Errors: %d\n", missCount);
	printf("-------------------------------------------\n");
	destroyMMU(mmu);
	return 0;
}
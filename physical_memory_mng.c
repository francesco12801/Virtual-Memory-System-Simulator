#include <stdlib.h>
#include <stdio.h>
#include "physical_memory_mng.h"

void __init_phisical_memory(int pageSize, int phisicalMemorySize) {
	map = malloc(sizeof(memory_map));
	map->physicalFrameSize = phisicalMemorySize / pageSize; 
	map->frames = calloc(map->physicalFrameSize, sizeof(phisical_frame));
	for (int i = 0; i < map->physicalFrameSize; i++) {
		map->frames[i].isFree = (unsigned char)0;
	}
}

int __reserve_frame() {
	for (int i = 0; i < map->physicalFrameSize; i++) {
		if (FRAME_IS_FREE(map->frames[i])) {
			map->frames[i].isFree = (unsigned char)1;
			return i;
		}
	}
	return -1;
}

int __release_frame(int frameId) {
	if (frameId >= 0 && frameId < map->physicalFrameSize)
		map->frames[frameId].isFree = (unsigned char)0;
	return (frameId >= 0 && frameId < map->physicalFrameSize);
}


int __main() {
	int frameId;
	int hitCount = 0;
	int faultCout = 0;
	__init_phisical_memory((1<<12), (1<<20));

	for (int i=0; i < 10024; i ++) {
		frameId = __reserve_frame();
		if (frameId == i) hitCount++;
		else faultCout++;
	}
	for (int i = 30; i < map->physicalFrameSize/2; i++) {
		if (!__release_frame(i-2)) {
			printf("Error releasgin %d\n", (i-2));
		}
	}
	printf("HIT: %d, fault: %d\n", hitCount, faultCout);
	hitCount = 0;
	faultCout = 0;
	for (int i = 0; i < map->physicalFrameSize; i++) {
		frameId = __reserve_frame();
		if (frameId >= 0) hitCount++;
		else faultCout++;
	}
	printf("HIT: %d, fault: %d\n", hitCount, faultCout);
	return 0;
}
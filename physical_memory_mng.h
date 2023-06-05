

#define FRAME_IS_FREE(f) (f.isFree == 0)

typedef struct {
	unsigned char isFree;
} phisical_frame;

typedef struct {
	phisical_frame *frames;
	int physicalFrameSize;
} memory_map;


memory_map *map;

void __init_phisical_memory(int pageSize, int phisicalMemorySize);
int  __reserve_frame();
int  __release_frame(int frameId);
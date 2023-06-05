
#include <stdio.h>
#include <stdlib.h>

#define VIRTUAL_MEMORY_SIZE (16777216)   // 16 MB
#define PHYSICAL_MEMORY_SIZE (1048576)    // 1 MB
#define PAGE_SIZE (4096)                  // 4 KB
#define PAGE_TABLE_ENTRIES (4096) // Having 4096 pages of virtual mem
#define SWAP_FILE_SIZE (16777216)

// Page table entry flags
#define FLAG_VALID 0x01
#define FLAG_UNSWAPPABLE 0x02
#define FLAG_READ_BIT 0x04
#define FLAG_WRITE_BIT 0x08


#define IS_VALID(flag) (flag & FLAG_VALID)
#define IS_UNSWAPPABLE(flag) (flag & FLAG_UNSWAPPABLE)
#define IS_READ(flag) (flag & FLAG_READ_BIT)
#define IS_WRITE(flag) (flag & FLAG_WRITE_BIT)

typedef struct {
	unsigned int flags;
	int physical_page_index;
} PageTableEntry;

// MMU structure
typedef struct {
    char *physical_memory;
    PageTableEntry *page_table;
    FILE* swap_file;
} MMU;

// MMU function prototypes
MMU* initializeMMU() ;
void destroyMMU(MMU* mmu);
void MMU_writeByte(MMU* mmu, int pos, char c) ;
char MMU_readByte(MMU* mmu, int pos);
void pageFaultExcepHandler(MMU* mmu, int pos);
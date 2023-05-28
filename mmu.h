
#include <stdio.h>
#include <stdlib.h>

#define VIRTUAL_MEMORY_SIZE (1<<24)   // 16 MB
#define PHYSICAL_MEMORY_SIZE (1<<20)    // 1 MB
#define PAGE_SIZE (1<<12)                  // 4 KB
#define PAGE_TABLE_ENTRIES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE) // Having 4096 pages of virtual mem
#define SWAP_FILE_SIZE (1<<24)

// Page table entry flags
#define FLAG_VALID (1 << 0)
#define FLAG_UNSWAPPABLE (1 << 1)
#define FLAG_READ_BIT (1 << 2)
#define FLAG_WRITE_BIT (1 << 3)
#define FLAG_REFERENCE_BIT (1 << 4)

// MMU structure
typedef struct {
    char* physical_memory;
    unsigned char* page_table;
    char* swap_file;
    int current_page;
} MMU;

// MMU function prototypes
void initializeMMU(MMU* mmu) ;
void destroyMMU(MMU* mmu);
void MMU_writeByte(MMU* mmu, int pos, char c) ;
char MMU_readByte(MMU* mmu, int pos);
void pageFaultExcepHandler(MMU* mmu, int pos);
void swapOutPage(MMU* mmu);
int swapInPage(MMU* mmu, int page) ;
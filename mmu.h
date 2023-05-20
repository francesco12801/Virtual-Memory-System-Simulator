
#include <stdio.h>
#include <stdlib.h>

#define VIRTUAL_MEMORY_SIZE 16777216    // 16 MB
#define PHYSICAL_MEMORY_SIZE 1048576    // 1 MB
#define PAGE_SIZE 4096                   // 4 KB
#define PAGE_TABLE_ENTRIES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE)
#define SWAP_FILE_SIZE VIRTUAL_MEMORY_SIZE

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
void MMU_init(MMU* mmu);
void MMU_destroy(MMU* mmu);
void MMU_writeByte(MMU* mmu, int pos, char c);
char MMU_readByte(MMU* mmu, int pos);
void MMU_exception(MMU* mmu, int pos);
void MMU_swapOutPage(MMU* mmu);
int MMU_swapInPage(MMU* mmu, int page);
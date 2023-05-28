
#include "mmu.h"

// MMU initialization
void initializeMMU(MMU* mmu) {
    mmu->physical_memory = (char*)malloc(PHYSICAL_MEMORY_SIZE);
    mmu->page_table = (unsigned char*)calloc(PAGE_TABLE_ENTRIES, sizeof(unsigned char));
    mmu->swap_file = (char*)malloc(SWAP_FILE_SIZE);
    mmu->current_page = 0;
}

// MMU dealloc 
void destroyMMU(MMU* mmu) {
    free(mmu->physical_memory);
    free(mmu->page_table);
    free(mmu->swap_file);
}

// Writing a single byte, accessing to the memory in position pos 

void MMU_writeByte(MMU* mmu, int pos, char c) {
    int page = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;

    if (!(mmu->page_table[page] & FLAG_VALID))
        pageFaultExcepHandler(mmu, pos);

    mmu->page_table[page] |= FLAG_WRITE_BIT;
    mmu->physical_memory[page * PAGE_SIZE + offset] = c;
}

// Read a byte from the virtual memory
char MMU_readByte(MMU* mmu, int pos) {
    int page = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;

    if (!(mmu->page_table[page] & FLAG_VALID))
        pageFaultExcepHandler(mmu, pos);

    mmu->page_table[page] |= FLAG_READ_BIT;
    mmu->page_table[page] |= FLAG_REFERENCE_BIT;
    return mmu->physical_memory[page * PAGE_SIZE + offset];
}

// Handle page fault exception 

void pageFaultExcepHandler(MMU* mmu, int pos) {
    int page = pos / PAGE_SIZE;

    if (mmu->page_table[page] & FLAG_UNSWAPPABLE) {
        printf("Invalid access to unswappable page!\n");
        exit(1);
    }

    if (!(mmu->page_table[page] & FLAG_VALID)) {
        int swapped_out_page = swapInPage(mmu, page);
        mmu->page_table[swapped_out_page] &= ~FLAG_VALID;
    }

    mmu->page_table[page] |= FLAG_VALID;
}


void swapOutPage(MMU* mmu) {
    // setting counter that lemme know if i have founded page
    int found = 0;  
    int initial_page = mmu->current_page; 

    // implementing second chance getting counter as STOP 

    while (!found) {
        // page is the current page saved pointed by MMU 
        int page = mmu->current_page;
        mmu->current_page = (mmu->current_page + 1) % PAGE_TABLE_ENTRIES;



        if (mmu->page_table[page] & FLAG_VALID) {
            // here we need to see if reference bit is set on the same entry (need i to do this control?)
            if (mmu->page_table[page] & FLAG_REFERENCE_BIT) {
                mmu->page_table[page] &= ~FLAG_REFERENCE_BIT; // reset
            } else {
                int swap_offset = page * PAGE_SIZE;
                int memory_offset = swapInPage(mmu, -1) * PAGE_SIZE;

                // Copy the page from physical memory to the swap file
                for (int i = 0; i < PAGE_SIZE; i++) {
                    mmu->swap_file[swap_offset + i] = mmu->physical_memory[memory_offset + i];
                }

                found = 1;
            }
        }
    }
}

// Swap in a page from the swap file
int swapInPage(MMU* mmu, int page) {
    static int last_swapped_in_page = 0;
    int swap_offset = (last_swapped_in_page % PAGE_TABLE_ENTRIES) * PAGE_SIZE;
    int memory_offset = page >= 0 ? page * PAGE_SIZE : last_swapped_in_page * PAGE_SIZE;

    // Copy the page from the swap file to physical memory
    for (int i = 0; i < PAGE_SIZE; i++) {
        mmu->physical_memory[memory_offset + i] = mmu->swap_file[swap_offset + i];
    }

    return last_swapped_in_page++;
}

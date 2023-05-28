#include "mmu.c"

// Testing memory  

int main() {
    MMU mmu;
    initializeMMU(&mmu);

    // Access virtual memory
    for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++) {
        MMU_writeByte(&mmu, i, 'A' + (i % 26));
    }

    // Read from virtual memory
    for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++) {
        char c = MMU_readByte(&mmu, i);
        printf("%c", c);
        printf("\n");
    }

    destroyMMU(&mmu);
    return 0;
}
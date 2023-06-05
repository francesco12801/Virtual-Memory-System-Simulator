
#include "mmu.h"
#include "physical_memory_mng.h"

// MMU initialization
MMU *initializeMMU() {
	MMU *mmu = (MMU *)malloc(sizeof(MMU));
	mmu->physical_memory = (char *)calloc(PHYSICAL_MEMORY_SIZE, sizeof(char));
	__init_phisical_memory(PAGE_SIZE, PHYSICAL_MEMORY_SIZE);

	// Initialize page table
	mmu->page_table = (PageTableEntry *)calloc(PAGE_TABLE_ENTRIES, sizeof(PageTableEntry));
	for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
		mmu->page_table[i].flags = 0;
		mmu->page_table[i].physical_page_index = -1;
	}

	// Open swap file
	mmu->swap_file = fopen("swap_file.bin", "w+b");
	if (mmu->swap_file == NULL) {
		printf("Failed to open swap file.\n");
		exit(1);
	}
	return mmu;
}

// MMU dealloc
void destroyMMU(MMU *mmu) {
	fclose(mmu->swap_file);
	free(mmu->physical_memory);
	free(mmu->page_table);
}

// Writing a single byte, accessing to the memory in position pos
void MMU_writeByte(MMU *mmu, int pos, char c) {
	int page = pos / PAGE_SIZE;
	int offset = pos % PAGE_SIZE;

	if (!(IS_VALID(mmu->page_table[page].flags)))
		pageFaultExcepHandler(mmu, pos);

	mmu->page_table[page].flags |= FLAG_WRITE_BIT;
	mmu->page_table[page].flags |= FLAG_UNSWAPPABLE;
	mmu->physical_memory[mmu->page_table[page].physical_page_index * PAGE_SIZE + offset] = c;
}

// Read a byte from the virtual memory
char MMU_readByte(MMU *mmu, int pos) {
	int page = pos / PAGE_SIZE;
	int offset = pos % PAGE_SIZE;

	if (!(IS_VALID(mmu->page_table[page].flags)))
		pageFaultExcepHandler(mmu, pos);

	mmu->page_table[page].flags |= FLAG_READ_BIT;
	mmu->page_table[page].flags |= FLAG_UNSWAPPABLE;
	return mmu->physical_memory[mmu->page_table[page].physical_page_index * PAGE_SIZE + offset];
}

int electPageToSwapOut(MMU *mmu) {
	for (int i = 0; i < PAGE_TABLE_ENTRIES * 2; i++) {
		if (IS_VALID(mmu->page_table[i % (PAGE_TABLE_ENTRIES)].flags)) {
			if (IS_UNSWAPPABLE(mmu->page_table[i % PAGE_TABLE_ENTRIES].flags)) {
				// Second chance, reset the FLAG_UNSWAPPABLE bits and continue
				mmu->page_table[i].flags &= ~FLAG_UNSWAPPABLE;
			}
			else {
				// Found a victim page to be swapped out. It is the first SWAPPABLE page.
				return i % (PAGE_TABLE_ENTRIES);
			}
		}
	}
	return -1;
}


// Handle page fault exception
void pageFaultExcepHandler(MMU *mmu, int pos) {
	// Calculate the page number
	int page = pos / PAGE_SIZE;
	int page_to_swap = -1;

	int next_frame = __reserve_frame();
	if (next_frame < 0) {
		// it is not available a physical frame for the page.
		// whe should free a frame!
		page_to_swap = electPageToSwapOut(mmu);

		if (page_to_swap >= 0) {
			// printf("Electing page %d to swap out\n", page_to_swap);
			// free the frame linked to the page to swap
			if (IS_WRITE(mmu->page_table[page_to_swap].flags)) {
				// write to swap file only if the page was written
				fseek(mmu->swap_file, page_to_swap * PAGE_SIZE, SEEK_SET);
				fwrite(&mmu->physical_memory[mmu->page_table[page_to_swap].physical_page_index * PAGE_SIZE], sizeof(char), PAGE_SIZE, mmu->swap_file);
			}
			__release_frame(mmu->page_table[page_to_swap].physical_page_index);
			mmu->page_table[page_to_swap].flags = 0;
		}
		else {
			printf("page to swap out not found. Program exit!\n");
			exit(1);
		}
		next_frame = __reserve_frame();
	}

	if (next_frame >= 0) {
		mmu->page_table[page].physical_page_index = next_frame;
		mmu->page_table[page].flags = FLAG_VALID | FLAG_UNSWAPPABLE;
		// Swap in the requested page
		// printf("read page %d from swap\n", page_to_swap);

		fseek(mmu->swap_file, page * PAGE_SIZE, SEEK_SET);
		fread(&mmu->physical_memory[mmu->page_table[page].physical_page_index * PAGE_SIZE], sizeof(char), PAGE_SIZE, mmu->swap_file);
	}
	else {
		printf("Unable to swap aout a page. Program exit!\n");
		exit(1);
	}
}

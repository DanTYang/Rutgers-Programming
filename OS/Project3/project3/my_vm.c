#include "my_vm.h"

char* physicalMemoryBitmap = NULL;

char* virtualMemoryBitmap = NULL;

char* physicalMemory = NULL;

pde_t* pageDirectory = NULL;
pte_t** pageTables = NULL;

int isInitialized = 0;

tlb* TLBhead = NULL;
int currentTLBSize = 0;

double TLBHits = 0;
double TLBMisses = 0;

pthread_mutex_t mutex;

int get_bit_at_index(char* bitmap, int index) {
    char* region = bitmap + (index / 8);

    char bit = (*region >> (index % 8)) & 0x1;

    return (int) bit;
}

void set_bit_at_index(char* bitmap, int index) {
    char* region = bitmap + (index / 8);

    char bit = 1 << (index % 8);

    *region |= bit;

    return;
}

void unset_bit_at_index(char* bitmap, int index) {
    char* region = bitmap + (index / 8);

    char bit = 1 << (index % 8);

    *region &= ~bit;

    return;
}

// Function responsible for allocating and setting your physical memory 
void set_physical_mem() {
    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating
    
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them

    physicalMemory = malloc(MEMSIZE);

    int numPhysicalPages = MEMSIZE / (PGSIZE * 8);
    physicalMemoryBitmap = malloc(numPhysicalPages);

    unsigned long long i;
    for (i = 0; i < numPhysicalPages; i++)
        *(physicalMemoryBitmap + i) = 0;

    unsigned long long numVirtualPages = MAX_MEMSIZE / (PGSIZE * 8);
    virtualMemoryBitmap = malloc(numVirtualPages);

    for (i = 0; i < numVirtualPages; i++)
        *(virtualMemoryBitmap + i) = 0;
}

/* Part 2: Add a virtual to physical page translation to the TLB.
   Feel free to extend the function arguments or return type. */
void add_TLB(void *va, void *pa)
{
    //Part 2 HINT: Add a virtual to physical page translation to the TLB
    tlb* newEntry = malloc(sizeof(tlb));
    newEntry -> virtualMemoryAddress = (unsigned long) va;
    newEntry -> physicalMemoryAddress = (unsigned long) pa;

    if (currentTLBSize == TLB_ENTRIES) {
        tlb* currentEntry = TLBhead;

        int i;
        for (i = 2; i < TLB_ENTRIES; i++) {
            currentEntry = currentEntry -> next;
        }

        tlb* removedEntry = currentEntry -> next;
        currentEntry -> next = NULL;

        free(removedEntry);
    } else {
        currentTLBSize++;
    }

    newEntry -> next = TLBhead;
    TLBhead = newEntry;
}

void remove_TLB(void *va) {
    tlb* currentEntry = TLBhead;
    tlb* previousEntry = NULL;

    int i;
    while (currentEntry != NULL) {
        if (currentEntry -> virtualMemoryAddress == (unsigned long) va) {
            if (previousEntry == NULL) {
                tlb* removedEntry = TLBhead;
                TLBhead = TLBhead -> next;

                free(removedEntry);
            } else {
                previousEntry -> next = currentEntry -> next;

                free(currentEntry);
            }

            currentTLBSize--;
            return;
        }

        previousEntry = currentEntry;
        currentEntry = currentEntry -> next;
    }
}

/* Part 2: Check TLB for a valid translation.
   Returns the physical page address.
   Feel free to extend this function and change the return type. */
pte_t* check_TLB(void *va) {
    //Part 2: TLB lookup code here
    tlb* currentEntry = TLBhead;

    int i;
    for (i = 0; i < currentTLBSize; i++) {
        if (currentEntry -> virtualMemoryAddress == (unsigned long) va) {
            TLBHits++;

            return (pte_t*) currentEntry -> physicalMemoryAddress;
        }
    }

    TLBMisses++;

    return NULL;
}

/* Part 2: Print TLB miss rate.
   Feel free to extend the function arguments or return type. */
void print_TLB_missrate()
{
    double miss_rate = 0;	

    //Part 2 Code here to calculate and print the TLB miss rate.
    miss_rate = (TLBHits + TLBMisses)  / TLBMisses;

    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}

/* The function takes a virtual address and page directories starting address and
   performs translation to return the physical address. */
pte_t *translate(pde_t *pgdir, void *va, int isFreeing) {
    /* Part 1 HINT: Get the Page directory index (1st level) Then get the
       2nd-level-page table index using the virtual address.  Using the page
       directory index and page table index get the physical address.
    
       Part 2 HINT: Check the TLB before performing the translation. If
       translation exists, then you can return physical address from the TLB. */

    //If translation not successfull return null

    pte_t* physicalAddress = check_TLB(va);
    if (physicalAddress != NULL) {
        return physicalAddress;
    }

    int offsetBits = log2(PGSIZE);
    int pageDirectoryBits = (32 - offsetBits) / 2;
    int pageTableBits = 32 - offsetBits - pageDirectoryBits;

    unsigned long virtualAddress = (unsigned long) va;
    unsigned long pageTableAddress = virtualAddress >> (offsetBits + pageTableBits);

    pte_t* pageDirectoryEntry = (pte_t*) *(pgdir + pageTableAddress);
    if (pageDirectoryEntry == NULL) {
        return NULL;
    }

    unsigned long outerBitsMask = (1 << pageTableBits) - 1;
    unsigned long physicalMemoryAddress = (virtualAddress >> offsetBits) & outerBitsMask;

    pte_t* pageTableEntry = (pte_t*) *(pageDirectoryEntry + physicalMemoryAddress);
    if (pageTableEntry == NULL) {
        return NULL;
    }

    if (isFreeing) {
        free(pageDirectoryEntry + physicalMemoryAddress);

        *(pageDirectoryEntry + physicalMemoryAddress) = NULL;
    }

    return pageTableEntry;
}

/* The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added. */
int page_map(pde_t *pgdir, void *va, void *pa)
{
    /* HINT: Similar to translate(), find the page directory (1st level)
       and page table (2nd-level) indices. If no mapping exists, set the
       virtual to physical mapping. */

    int offsetBits = log2(PGSIZE);
    int pageDirectoryBits = (32 - offsetBits) / 2;
    int pageTableBits = 32 - offsetBits - pageDirectoryBits;

    unsigned long virtualAddress = (unsigned long) va;
    unsigned long pageTableAddress = virtualAddress >> (offsetBits + pageTableBits);

    if (*(pgdir + pageTableAddress) == NULL) {
        int totalEntries = pow(2, pageTableBits);
        *(pageTables + pageTableAddress) = malloc(totalEntries * PGSIZE);

        *(pgdir + pageTableAddress) = (pte_t) *(pageTables + pageTableAddress);
    }
    pte_t* pageDirectoryEntry = (pte_t*) *(pgdir + pageTableAddress);

    unsigned long outerBitsMask = (1 << pageTableBits) - 1;
    unsigned long physicalMemoryAddress = (virtualAddress >> offsetBits) & outerBitsMask;

    pte_t pageTableEntry = *(pageDirectoryEntry + physicalMemoryAddress);
    if (pageTableEntry == NULL) {
        pageTableEntry = (pte_t) pa;

        add_TLB(va, pa);

        return 1;
    }

    return 0;
}

//Function that gets the next available page.
void *get_next_avail(int num_pages) {
    //Use virtual address bitmap to find the next free page
    int length = 0;

    unsigned long long numVirtualPages = MAX_MEMSIZE / (PGSIZE * 8);
    unsigned long long i;
    for (i = 0; i < numVirtualPages; i++) {
        int currentBit = get_bit_at_index(virtualMemoryBitmap, i);

        if (currentBit == 0) {
            length++;
            if (length == num_pages)
                return i - num_pages + 1;
        } else {
            length = 0;
        }
    }

    return NULL;
}

void *get_physical_page() {
    int numPhysicalPages = MEMSIZE / (PGSIZE * 8);
    int i;
    for (i = 0; i < numPhysicalPages; i++) {
        int currentBit = get_bit_at_index(physicalMemoryBitmap, i);

        if (currentBit == 0)
            return i;
    }

    return NULL;
}

//Function responsible for allocating pages and used by the benchmark.
void *a_malloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.

    /* HINT: If the page directory is not initialized, then initialize the
      page directory. Next, using get_next_avail(), check if there are free pages. If
      free pages are available, set the bitmaps and map a new page. Note, you will 
      have to mark which physical pages are used. */

    pthread_mutex_lock(&mutex);

    if (!isInitialized) {
        int offsetBits = log2(PGSIZE);
        int pageDirectoryBits = (32 - offsetBits) / 2;
        int pageTableBits = 32 - offsetBits - pageDirectoryBits;

        int directoryEntries = pow(2, pageDirectoryBits);

        pageDirectory = malloc(directoryEntries * PGSIZE);

        set_physical_mem();

        isInitialized = 1;
    }

    int numPages = (num_bytes / PGSIZE) + 1;

    void* virtualAddress = get_next_avail(numPages);
    if (virtualAddress == NULL)
        return NULL;

    int i;
    for (i = 0; i < numPages; i++) {
        //Set virtual bitmap
        set_bit_at_index(virtualMemoryBitmap, virtualAddress + i);

        //Find physical page
        void* physicalAddress = get_physical_page();
        if (physicalAddress == NULL)
            return NULL;

        //Set physical bitmap
        set_bit_at_index(physicalMemoryBitmap, physicalAddress);

        //use pagemap function
        page_map(pageDirectory, ((unsigned long) virtualAddress + i) * PGSIZE, ((unsigned long) physicalAddress) * PGSIZE);
    }

    pthread_mutex_unlock(&mutex);

    return ((unsigned long) virtualAddress) * PGSIZE;
}

//Responsible for releasing one or more memory pages using virtual address (va).
void a_free(void *va, int size) {

    /* Part 1: Free the page table entries starting from this virtual address
       (va). Also mark the pages free in the bitmap. Perform free only if the 
       memory from "va" to va+size is valid.
     
       Part 2: Also, remove the translation from the TLB. */
     
    pthread_mutex_lock(&mutex);

    int numPages = (size / PGSIZE) + 1;

    int i;
    for (i = 0; i < numPages; i++) {
        pte_t* physicalAddress = translate(pageDirectory, (unsigned long) va + (i * PGSIZE), 1);
        if (physicalAddress ==  NULL) {
            perror("Could not free a page that does not exist\n");
            return;
        }

        unset_bit_at_index(virtualMemoryBitmap, (unsigned long) va / PGSIZE);
        unset_bit_at_index(physicalMemoryBitmap, (unsigned long) physicalAddress / PGSIZE);

        remove_TLB(va + (i * PGSIZE));

        //Free actual memory?
    }

    pthread_mutex_unlock(&mutex);

    return;
}


// The function copies data pointed by "val" to physical memory pages using virtual address (va).
void put_value(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger 
       than one page. Therefore, you may have to find multiple pages using translate()
       function. */

    int numPages = (size / PGSIZE) + 1;

    int i;
    for (i = 0; i < numPages; i++) {
        pte_t* physicalAddress =  translate(pageDirectory, va + (i * PGSIZE), 0);

        memcpy(physicalMemory + (unsigned long) physicalAddress, val + (i * PGSIZE), PGSIZE);
    }
}


//Given a virtual address, this function copies the contents of the page to val.
void get_value(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
       "val" address. Assume you can access "val" directly by derefencing them. */

    int numPages = (size / PGSIZE) + 1;

    int i;
    for (i = 0; i < numPages; i++) {
        pte_t* physicalAddress = translate(pageDirectory, va + (i * PGSIZE), 0);

        memcpy(val + (i * PGSIZE), physicalMemory + (unsigned long) physicalAddress, PGSIZE);
    }
}



/* This function receives two matrices mat1 and mat2 as an argument with size
   argument representing the number of rows and columns. After performing matrix
   multiplication, copy the result to answer. */
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
       matrix accessed. Similar to the code in test.c, you will use get_value() to
       load each element and perform multiplication. Take a look at test.c! In addition to 
       getting the values from two matrices, you will perform multiplication and 
       store the result to the "answer array". */
    int a, b, c = 0;
    int addr_1, addr_2, addr_3 = 0;
    int i, j, k; 
    for (i = 0; i < size; i++) 
    { 
        for (j = 0; j < size; j++) 
        { 
            int val = 0;
            for (k = 0; k < size; k++) 
            {
                addr_1 = (unsigned int)mat1 + ((i * size * sizeof(int))) + (k * sizeof(int));
                addr_2 = (unsigned int)mat2 + ((k * size * sizeof(int))) + (j * sizeof(int));
                get_value((void *)addr_1, &a, sizeof(int));
                get_value((void *)addr_2, &b, sizeof(int));
                val += (a*b);
            }
            addr_1 = (unsigned int)answer + ((i * size * sizeof(int))) + (j * sizeof(int));
            put_value((void *)addr_3, &val, sizeof(int));
        } 
    }
}




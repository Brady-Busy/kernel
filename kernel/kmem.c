#include "kmem.h"

uint64_t virtual_base;
uintptr_t next_free = 0;

void invalidate_tlb(uintptr_t virtual_address) {
   __asm__("invlpg (%0)" :: "r" (virtual_address) : "memory");
}

uint64_t ptov(uint64_t physical_addr){
  return physical_addr + virtual_base;
}

// setting up the free list, each page in the list is physical address of the page
void fl_setup(struct stivale2_struct* hdr){
  struct stivale2_mmap_entry* physical_entries;

  struct stivale2_struct_tag_memmap* physical_tag = (struct stivale2_struct_tag_memmap*) find_tag(hdr, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  uint64_t num_entries = physical_tag -> entries;
  physical_entries = physical_tag -> memmap;

  struct stivale2_struct_tag_hhdm* virtual_tag = (struct stivale2_struct_tag_hhdm*) find_tag(hdr, STIVALE2_STRUCT_TAG_HHDM_ID);
  virtual_base = virtual_tag -> addr;

  uintptr_t cursor = 0;

  //term_init();
  //kprintf("%x\n", virtual_base);

  // iterating through physical_entries, we trust physical_tag to give us correct number of entries
  for (int i = 0; i < num_entries; i++) {

    struct stivale2_mmap_entry current = physical_entries[i];

    // if the memory is not usable, skip
    if (current.type != 1) continue;

    uint64_t base = current.base;
    uint64_t end = base + current.length;

    // next_free will be the start of the first usable chunk
    if (!next_free) {
      next_free = base;
    }

    //If the cursor is on the last page in the previous chunk, add a pointer to the first page of this chunk
    if (cursor) {
      // to access cursor, we need to convert it to virtual
      *(uintptr_t*)ptov(cursor) = base;
    }
    // cursor is a physical address
    cursor = base;

    //Now, next_free points to the start of the available memory
    while (cursor < end - PAGE_SIZE) {
      // to access cursor, we need to convert it to virtual
      *(uintptr_t*)ptov(cursor) = cursor + PAGE_SIZE;
      //Move next_free to the next available page
      cursor += PAGE_SIZE;
    }
  }
}


// return and allocate physical address of next free page
uintptr_t pmem_alloc(){
  //Store next_free
  uintptr_t result = next_free;
  
  //Update next_free, converting next_free to virtual to access
  next_free = *(uintptr_t*)ptov(next_free);

  return result;
}

// p is a physical address
void pmem_free(uintptr_t p){
  // converting physical to virtual for access
  *(uintptr_t*)ptov(p) = next_free;
  //kprintf("added 0x%x to the freelist\n",*(uintptr_t*)ptov(p));
  next_free = p;
  //halt();
}

void usable_mem (struct stivale2_struct* hdr) {
  struct stivale2_mmap_entry* physical_entries;

  struct stivale2_struct_tag_memmap* physical_tag = (struct stivale2_struct_tag_memmap*) find_tag(hdr, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  uint64_t num_entries = physical_tag -> entries;
  physical_entries = physical_tag -> memmap;

  kprintf("Usable Memory: \n");

  // iterating through physical_entries, we trust physical_tag to give us correct number of entries
  for (int i = 0; i < num_entries; i++) {

    struct stivale2_mmap_entry current = physical_entries[i];

    // if the memory is not usable, skip
    if (current.type != 1) continue;

    uint64_t base = current.base;
    uint64_t end = base + current.length;

    kprintf("  0x%x-0x%x mapped at 0x%x-0x%x\n", base, end, ptov(base), ptov(end));
  }
}

/**
 *  memory staff
 * 
 */

typedef struct page_table_entry {
  bool present : 1;
  bool writable : 1;
  bool user : 1;
  bool write_through : 1;
  bool cache_disable : 1;
  bool accessed : 1;
  bool dirty : 1;
  bool page_size : 1;
  uint8_t _unused0 : 4;
  uintptr_t address : 40;
  uint16_t _unused1 : 11;
  bool no_execute : 1;
} __attribute__((packed)) page_entry_t;

uintptr_t read_cr3() {
  uintptr_t value;
  __asm__("mov %%cr3, %0" : "=r" (value));
  return value;
}

void write_cr3(uint64_t value) {
  __asm__("mov %0, %%cr3" : : "r" (value));
}

uint64_t get_idx(uintptr_t page_index, int beginning, int mask){
  return (page_index >> beginning) & ((1 << mask) - 1);
}

void translate(void* address) {
  uintptr_t page_index = (uintptr_t) address;
  kprintf("Translating %p\n", address);

  uintptr_t table_address = read_cr3();
  page_entry_t* table_ref = (page_entry_t*) ptov(table_address);

  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    page_entry_t *page_entry = &table_ref[index];
    kprintf ("  Level %d (index %d of 0x%x)\n", 4 - i, index, table_address);
    table_address = page_entry->address << 12;
    table_ref = (page_entry_t*) ptov(table_address);
    if (!page_entry->present){
      kprintf ("    not present");
      return;
    }
    kprintf ("    %s%s%s -> 0x%x\n", page_entry->user ? "user ":"kernel ",
                                     page_entry->writable ? "writable ": "",
                                     page_entry->no_execute ? "" : "executable",
                                     table_address);
  }
  // table address is now pointing to the physical address of the page
  kprintf ("%p maps to 0x%x\n", address, table_address + (page_index & OFFSET));
}

bool vm_map (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable){
  //kprintf("trying to map %x\n", address);
  uintptr_t page_index = address;

  uintptr_t table_address = root;
  page_entry_t* table_ref = (page_entry_t*) ptov(table_address);
  page_entry_t* page_entry;

  //Get us to the page table
  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    //kprintf ("index is %d for level %d\n", index, 4-i);
    page_entry = &table_ref[index];
    if (!page_entry->present && i != 3){
      page_entry->address = pmem_alloc() >> 12;
      memset (ptov(page_entry->address << 12), 0, PAGE_SIZE);
      page_entry->user = page_entry->writable = page_entry->present = true;
      page_entry->no_execute = false;
    }
    table_address = page_entry->address << 12;
    table_ref = (page_entry_t*) ptov(table_address);
  }
  // page_entry is now pointing to the page table entry (level 1)
  //Check to see if this page is taken
  

  if (page_entry->present) {
    return false;
  } else {
    page_entry->address = pmem_alloc() >> 12;
    memset (ptov(page_entry->address << 12), 0, PAGE_SIZE);
    page_entry->user = user;
    page_entry->writable = writable;
    page_entry->present = true;
    page_entry->no_execute = !executable;
    invalidate_tlb(ptov(page_entry->address << 12));
    return true;
  }
}

bool vm_unmap (uintptr_t root, uintptr_t address){
  uintptr_t page_index = address;

  uintptr_t table_address = root;
  page_entry_t* table_ref = (page_entry_t*) ptov(table_address);
  page_entry_t* page_entry;

  //Get us to the page table
  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    page_entry = &table_ref[index];
    if (!page_entry->present){
      return false;
    }
    table_address = page_entry->address << 12;
    table_ref = (page_entry_t*) ptov(table_address);
  }
  // page_entry is now pointing to the page table entry (level 1)
  //Check to see if this page is taken

  if (!page_entry->present) {
    return false;
  } else {
    pmem_free(page_entry->address << 12);
    page_entry->present = false;
    invalidate_tlb(ptov(page_entry->address << 12));
    return true;
  }
}

bool vm_protect (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable){
  uintptr_t page_index = address;

  uintptr_t table_address = root;
  page_entry_t* table_ref = (page_entry_t*) ptov(table_address);
  page_entry_t* page_entry;

  //Get us to the page table
  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    page_entry = &table_ref[index];
    if (!page_entry->present){
      return false;
    }
    table_address = page_entry->address << 12;
    table_ref = (page_entry_t*) ptov(table_address);
  }
  // page_entry is now pointing to the page table entry (level 1)
  // Check to see if this page is taken

  if (!page_entry->present) {
    return false;
  } else {
    page_entry->user = user;
    page_entry->writable = writable;
    page_entry->no_execute = !executable;
    invalidate_tlb(ptov(page_entry->address << 12));
    return true;
  }
}

void unmap_lower_half(uintptr_t root){
  // We can reclaim memory used to hold page tables, but NOT the mapped pages
  page_entry_t* l4_table = ptov(root);

  for (size_t l4_index = 0; l4_index < 256; l4_index++) {
    

    // Does this entry point to a level 3 table?
    if (l4_table[l4_index].present) {

      // Yes. Mark the entry as not present in the level 4 table
      l4_table[l4_index].present = false;

      // Now loop over the level 3 table
      page_entry_t* l3_table = ptov(l4_table[l4_index].address << 12);
      for (size_t l3_index = 0; l3_index < 512; l3_index++) {

        // Does this entry point to a level 2 table?
        if (l3_table[l3_index].present && !l3_table[l3_index].page_size) {

          // Yes. Loop over the level 2 table
          page_entry_t* l2_table = ptov(l3_table[l3_index].address << 12);
          for (size_t l2_index = 0; l2_index < 512; l2_index++) {

            // Does this entry point to a level 1 table?
            if (l2_table[l2_index].present && !l2_table[l2_index].page_size) {

              // Yes. Free the physical page the holds the level 1 table
              pmem_free(l2_table[l2_index].address << 12);
              //kprintf("l2 freed 0x%x\n",l2_table[l2_index].address << 12);
            }
          }
          // Free the physical page that held the level 2 table
          
          pmem_free(l3_table[l3_index].address << 12);
          //kprintf("l3 freed 0x%x\n",l3_table[l3_index].address << 12);
          //halt();
        }
      }
      //halt();
      
      // Free the physical page that held the level 3 table
      pmem_free(l4_table[l4_index].address << 12);
      //kprintf("l4 freed 0x%x\n",l4_table[l4_index].address << 12);
    }
  }
  //halt();
  // Is this freeing what it should
  // Why does it work with line 320 not 324
  // Reload CR3 to flush any cached address translations
  write_cr3(read_cr3());
  // halt();
}
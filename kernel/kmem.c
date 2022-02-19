#include "kmem.h"

uint64_t virtual_base;
uintptr_t next_free = 0;


void fl_setup(struct stivale2_struct* hdr){
  struct stivale2_mmap_entry* physical_entries;

  struct stivale2_struct_tag_memmap* physical_tag = (struct stivale2_struct_tag_memmap*) find_tag(hdr, STIVALE2_STRUCT_TAG_MEMMAP_ID);
  uint64_t num_entries = physical_tag -> entries;
  physical_entries = physical_tag -> memmap;

  struct stivale2_struct_tag_hhdm* virtual_tag = (struct stivale2_struct_tag_hhdm*) find_tag(hdr, STIVALE2_STRUCT_TAG_HHDM_ID);
  virtual_base = virtual_tag -> addr;

  uintptr_t cursor = 0;

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
      *(uintptr_t*)cursor = base;
    }
    // cursor is a virtual address
    cursor = base;

    //Now, next_free points to the start of the available memory
    while (cursor < end - PAGE_SIZE) {
      *(uintptr_t*)cursor = cursor + PAGE_SIZE;
      //Move next_free to the next available page
      cursor += PAGE_SIZE;
    }
  }
}

uintptr_t pmem_alloc(){
  //Store next_free
  uintptr_t result = next_free;
  
  //Update next_free
  next_free = *(uintptr_t*)next_free;

  return result;
}

void pmem_free(uintptr_t p){
  *(uintptr_t*)p = next_free;
  next_free = p;
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

    kprintf("  0x%x-0x%x mapped at 0x%x-0x%x\n", base, end, virtual_base + base, virtual_base + end);
  }
}

/**
 *  memory staff
 * 
 */

typedef struct page_entry {
  uint8_t present : 1;
  uint8_t writable : 1;
  uint8_t kernel : 1;
  uint16_t _unused_0 : 9;
  uint64_t physical_address : 51;
  uint8_t no_execute : 1;
} __attribute__((packed)) page_entry_t;

uintptr_t read_cr3() {
  uintptr_t value;
  __asm__("mov %%cr3, %0" : "=r" (value));
  return value;
}

uint64_t get_idx(uintptr_t page_index, int beginning, int mask){
  uint64_t index = page_index >> beginning;
  uint64_t masker = 1;
  for (int i = 0; i < mask; i++){
    masker *= 2;
  }
  masker--;
  index = index & masker;
  return index;
}

void translate(void* address) {
  uintptr_t page_index = (uintptr_t) address;
  kprintf("Translating %p\n", address);

  uintptr_t table_address = read_cr3();
  page_entry_t* table_ref = (page_entry_t*) (table_address + virtual_base);

  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    page_entry_t page_entry = table_ref[index];
    kprintf ("  Level %d (index %d of 0x%x)\n", 4 - i, index, table_address);
    table_address = page_entry.physical_address << 12;
    table_ref = (page_entry_t*) (table_address + virtual_base);
    if (!page_entry.present){
      kprintf ("    not present");
      return;
    }
    kprintf ("    %s%s%s -> 0x%x\n", page_entry.kernel ? "user ":"kernel ",
                                     page_entry.writable ? "writable ": "",
                                     page_entry.no_execute ? "" : "executable",
                                     table_address);
  }
  // table address is now pointing to the physical address of the page
  kprintf ("%p maps to 0x%x\n", address, table_address + (page_index & OFFSET));
}

bool vm_map (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable){
  uintptr_t page_index = address;

  uintptr_t table_address = root;
  page_entry_t* table_ref = (page_entry_t*) (table_address + virtual_base);
  page_entry_t page_entry;

  //Get us to the page table
  for (int i = 0; i < 4; i++){
    int index = get_idx(page_index, 39 - (i * 9), 9);
    page_entry = table_ref[index];
    if (!page_entry.present && i != 3){
      page_entry.physical_address = pmem_alloc();
      kmemset (page_entry.physical_address, 0, PAGE_SIZE);
      page_entry.kernel = page_entry.writable = page_entry.present = true;
      page_entry.no_execute = false;
    }
    table_address = page_entry.physical_address << 12;
    table_ref = (page_entry_t*) (table_address + virtual_base);
  }
  // page_entry is now pointing to the page table entry (level 1)
  //Check to see if this page is taken

  if (page_entry.present) {
    return false;
  } else {
    page_entry.physical_address = pmem_alloc();
    kmemset (page_entry.physical_address, 0, PAGE_SIZE);
    page_entry.kernel = user;
    page_entry.writable = writable;
    page_entry.present = true;
    page_entry.no_execute = executable;
    return true;
  }
}
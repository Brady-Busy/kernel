#include "kprint.h"
#include "port.h"
#include "pic.h"
#include "handler.h"
// Reserve space for the stack
static uint8_t stack[8192];
uint64_t virtual_base;
#define OFFSET 4095


static struct stivale2_tag unmap_null_hdr_tag = {
  .identifier = STIVALE2_HEADER_TAG_UNMAP_NULL_ID,
  .next = 0
};

// Request a terminal from the bootloader
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
	.tag = {
    .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
    .next = (uintptr_t)&unmap_null_hdr_tag
  },
  .flags = 0
};

// Declare the header for the bootloader
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
  // Use ELF file's default entry point
  .entry_point = 0,

  // Use stack (starting at the top)
  .stack = (uintptr_t)stack + sizeof(stack),

  // Bit 1: request pointers in the higher half
  // Bit 2: enable protected memory ranges (specified in PHDR)
  // Bit 3: virtual kernel mappings (no constraints on physical memory)
  // Bit 4: required
  .flags = 0x1E,
  
  // First tag struct
  .tags = (uintptr_t)&terminal_hdr_tag
};

// Find a tag with a given ID
void* find_tag(struct stivale2_struct* hdr, uint64_t id) {
  // Start at the first tag
	struct stivale2_tag* current = (struct stivale2_tag*)hdr->tags;

  // Loop as long as there are more tags to examine
	while (current != NULL) {
    // Does the current tag match?
		if (current->identifier == id) {
			return current;
		}

    // Move to the next tag
		current = (struct stivale2_tag*)current->next;
	}

  // No matching tag found
	return NULL;
}// Find a tag with a given IDL;

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

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  pic_init();

  // find the beginning of virtual address (NEEDS CLEAN!!!!!!!!!!!!!!!!!!!!!!!)
  struct stivale2_struct_tag_hhdm* virtual_tag = (struct stivale2_struct_tag_hhdm*) find_tag(hdr, STIVALE2_STRUCT_TAG_HHDM_ID);
  virtual_base = virtual_tag -> addr;

  //Time to make a special interrupt handler

  idt_setup();
  pic_unmask_irq(1);

  // Print a greeting
  kprintf("Hello Kernel!\n");
  int a =10;
  kprintf("%% %d %p",12,&a);

  usable_mem(hdr);

  translate(NULL);

  // while (1) {
  //   char test[20];
  //   kgets (test, 20);
  //   kprintf ("%s\n", test);
  // }

  // int* p = (int*)0x1;
  // *p = 123;

	// We're done, just hang...
	halt();
}
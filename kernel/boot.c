#include "kprint.h"
#include "port.h"
#include "pic.h"
#include "handler.h"
// Reserve space for the stack
static uint8_t stack[8192];

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
  uint64_t virtual_base;
  struct stivale2_mmap_entry* physical_entries;

  struct stivale2_struct_tag_hhdm* virtual_tag = (struct stivale2_struct_tag_hhdm*) find_tag(hdr, STIVALE2_STRUCT_TAG_HHDM_ID);
  virtual_base = virtual_tag -> addr;

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


void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  pic_init();
  //Time to make a special interrupt handler

  idt_setup();
  pic_unmask_irq(1);

  // Print a greeting
  kprintf("Hello Kernel!\n");
  //int a =10;
  //kprintf("%% %d %p",12,&a);

  usable_mem(hdr);

  while (1) {
    char test[20];
    kgets (test, 20);
    kprintf ("%s\n", test);
  }

  // int* p = (int*)0x1;
  // *p = 123;

	// We're done, just hang...
	halt();
}
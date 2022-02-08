#include "kprint.h"

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

//interupt handler stuff

typedef struct interrupt_context {
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

__attribute__((interrupt))
void std_handler(interrupt_context_t* ctx) {
  kprintf("Some error occured\n");
  halt();
}

// Every interrupt handler must specify a code selector. We'll use entry 5 (5*8=0x28), which
// is where our bootloader set up a usable code selector for 64-bit mode.
#define IDT_CODE_SELECTOR 0x28

// IDT entry types
#define IDT_TYPE_INTERRUPT 0xE
#define IDT_TYPE_TRAP 0xF

// A struct the matches the layout of an IDT entry
typedef struct idt_entry {
  uint16_t offset_0;
  uint16_t selector;
  uint8_t ist : 3;
  uint8_t _unused_0 : 5;
  uint8_t type : 4;
  uint8_t _unused_1 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t _unused_2;
} __attribute__((packed)) idt_entry_t;

// Make an IDT
idt_entry_t idt[256];



/**
 * Set an interrupt handler for the given interrupt number.
 *
 * \param index The interrupt number to handle
 * \param fn    A pointer to the interrupt handler function
 * \param type  The type of interrupt handler being installed.
 *              Pass IDT_TYPE_INTERRUPT or IDT_TYPE_TRAP from above.
 */
void idt_set_handler(uint8_t index, void* fn, uint8_t type) {
  idt_entry_t* current = &(idt[index]);
  intptr_t handler = (intptr_t) fn; 

  current->offset_0 = handler;
  current->offset_1 = handler >> 16;
  current->offset_2 = handler >> 32;

  current->type = type;

  current->present = 1;
  current->dpl = 0;
  current->ist = 0;
  current->selector = IDT_CODE_SELECTOR;
}

// This struct is used to load an IDT once we've set it up
typedef struct idt_record {
  uint16_t size;
  void* base;
} __attribute__((packed)) idt_record_t;

void kmemset (void* address, int value, size_t n) {
  char* cursor = (char*) address;
  for (int i = 0; i < n; i++) {
    *cursor = value;
    cursor++;
  }
}

/**
 * Initialize an interrupt descriptor table, set handlers for standard exceptions, and install
 * the IDT.
 */
void idt_setup() {
  // Step 1: Zero out the IDT, probably using memset (which you'll have to implement)
  // Write me!
  kmemset (&idt, 0, 256*sizeof(idt_entry_t));

  // Step 2: Use idt_set_handler() to set handlers for the standard exceptions (1--21)
  // Write me!
  for (int i = 0; i < 21; i++) {
    idt_set_handler (i, std_handler, IDT_TYPE_INTERRUPT);
  }

  // Step 3: Install the IDT
  idt_record_t record = {
    .size = sizeof(idt),
    .base = idt
  };
  __asm__("lidt %0" :: "m"(record));
}

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  idt_setup();

  // Print a greeting
  kprintf("Hello Kernel!\n");
  //int a =10;
  //kprintf("%% %d %p",12,&a);

  usable_mem(hdr);

  int* p = (int*)0x1;
  *p = 123;

	// We're done, just hang...
	halt();
}
#include "kprint.h"
#include "port.h"
#include "pic.h"
#include "handler.h"
#include "tools.h"
#include "kmem.h"

#include <stdbool.h>

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

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  pic_init();
  fl_setup(hdr);

  //Time to make a special interrupt handler

  idt_setup();
  pic_unmask_irq(1);

  // Print a greeting
  kprintf("Hello Kernel!\n");
  int a =10;
  kprintf("%% %d %p\n",12,&a);
  uintptr_t test = pmem_alloc();
  uintptr_t test2 = pmem_alloc();
  kprintf("%p %p\n",(void*)test,(void*)test2);
  *(int*)test = 1;
  *(int*)test2 = 3;
  kprintf("%d %d \n",*(int*)test,*(int*)test2);
  pmem_free(test);
  uintptr_t test3 = pmem_alloc();
  kprintf("%p %p\n",(void*)test3,(void*)test2);

  usable_mem(hdr);

  uintptr_t root = read_cr3() & 0xFFFFFFFFFFFFF000;
  int* p = (int*)0x50004000;
  bool result = vm_map(root, (uintptr_t)p, false, true, false);
  if (result) {
    *p = 123;
    kprintf("Stored %d at %p\n", *p, p);
    } else {
      kprintf("vm_map failed with an error\n");
    }


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
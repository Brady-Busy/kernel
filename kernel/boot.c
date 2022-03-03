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

int sys_read (int fd, char const *buf, int size){
  if (fd != 0){
    return -1;
  }
  return kgets (buf, size);
}

int sys_write (int fd, const char *buf, int size){
  if (fd != 1 && fd != 2){
    return -1;
  }
  int counter = 0;
  while (counter < size){
    char current = *buf++;
    if (!current){
      return counter;
    }
    kprint_c(current);
    counter++;
  }
  return size;
}

int syscall_handler(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
  switch (nr) {
    case 0:
      return sys_read(arg0, arg1, arg2);
    case 1:
      return sys_write(arg0, arg1, arg2);
    default:
      return -1;
  }
}

extern int syscall(uint64_t nr, ...);
extern void syscall_entry();

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  pic_init();
  fl_setup(hdr);

  //Time to make a special interrupt handler

  idt_setup();
  pic_unmask_irq(1);
  idt_set_handler(0x80, syscall_entry, IDT_TYPE_TRAP);

  /*
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
    if (!vm_map(root, (uintptr_t)p, false, true, false)) kprintf("success\n");
    if(vm_unmap (root, (uintptr_t)p)) kprintf("successful\n");
    kprintf("Stored %d at %p\n", *p, p);
    *p = 245;
    if (vm_map(root, (uintptr_t)p, false, true, false)) kprintf("success again\n");
    kprintf("Stored %d at %p\n", *p, p);
    } else {
      kprintf("vm_map failed with an error\n");
    }
  */
 while(1){
  char buf[6];
  syscall(SYS_read,0,buf,6);
  kprintf("%d\n", syscall(SYS_write,1,buf,10));}

  //usable_mem(hdr);

  //translate(NULL);

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
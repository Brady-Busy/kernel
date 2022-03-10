#include "kprint.h"
#include "port.h"
#include "pic.h"
#include "handler.h"
#include "tools.h"
#include "kmem.h"
#include "elf.h"

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

// Loading executables *TO DO: Move this to the appropriate location*

// Number of modules
uint64_t module_count;

//Global pointer to array of module descriptors
struct stivale2_module * modules;

void exec_setup(struct stivale2_struct* hdr) {
  // Look for a terminal tag
  struct stivale2_struct_tag_modules* tag = find_tag(hdr, STIVALE2_STRUCT_TAG_MODULES_ID);

  // Make sure we find a module tag
  if (tag == NULL){
    kprintf("No modules found\n");
  }

  //Update module_count & modules pointer
  module_count = tag->module_count;
  modules = tag->modules;
}

typedef void (*elf_exe_t)();

bool exec(struct stivale2_module elf_file) {
  // Cast the beginning of the elf_file to and elf header
  Elf64_Ehdr * elf_hdr = (Elf64_Ehdr *) elf_file.begin;

  // Check to confirm that this is executable
  if (elf_hdr->e_type != 2) {
    kprintf("%s is not executable\n", elf_file.string);
    return false;
  }

  // Get the total number of program header entries + size
  uint64_t pgm_hdr_count = elf_hdr->e_phnum;
  uint64_t pgm_hdr_size = elf_hdr->e_phentsize;

  uint64_t root = read_cr3() & 0xFFFFFFFFFFFFF000;

  for (int i = 0; i < pgm_hdr_count; i++) {
    // Cast the following bits to a program header struct
    Elf64_Phdr * program_hdr = (Elf64_Phdr *) (elf_file.begin + elf_hdr->e_phoff + (i * pgm_hdr_size));

    // From the program header, find the virtual address where it should be loaded
    uintptr_t virtual_add = program_hdr->p_vaddr;

    // Grab the flags to update after vm_map
    uint32_t flags = program_hdr->p_flags;

    if (program_hdr->p_type != 1) continue;

    // Call vm_map to map memory
    if (!vm_map(root, virtual_add, false, true, false)) {
      kprintf("vm_map failed\n");
      return false;
    }

    // Copying the contents of the executable
    pmemcpy(virtual_add, elf_file.begin + program_hdr->p_offset, program_hdr->p_filesz);

    // update permissions to reflect input
    if (!vm_protect(root, virtual_add, false, flags & 2, false & 4)){
      kprintf("vm_protect failed\n");
      return false;
    }
  }

  elf_exe_t current_exe = (elf_exe_t) (elf_hdr->e_entry);
  current_exe();

  return true;
}

void _start(struct stivale2_struct* hdr) {
  // We've booted! Let's start processing tags passed to use from the bootloader
  term_setup(hdr);
  pic_init();
  fl_setup(hdr);
  exec_setup(hdr);

  //Time to make a special interrupt handler

  idt_setup();
  pic_unmask_irq(1);
  idt_set_handler(0x80, syscall_entry, IDT_TYPE_TRAP);


  kprintf("Modules:\n");
  for (int i = 0; i < module_count; i++) {
    //Print name and details
    kprintf("%s 0x%x-0x%x\n", modules[i].string, modules[i].begin, modules[i].end);
    if (!exec(modules[i])){
      kprintf("exec failed\n");
    }
  }


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
//  while(1){
//   char buf[6];
//   syscall(SYS_read,0,buf,6);
//   kprintf("%d\n", syscall(SYS_write,1,buf,10));}

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
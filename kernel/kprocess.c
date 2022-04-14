#include "kprocess.h"

// Number of modules
uint64_t module_count;

//Global pointer to array of module descriptors
struct stivale2_module * modules;
struct stivale2_module * shell_module;



//typedef void (*elf_exe_t)();

bool kexec(struct stivale2_module elf_file) {
  unmap_lower_half(read_cr3());

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
    //kprintf("0x%x\n", virtual_add);

    if (flags == 3 || flags == 7){
      kprintf("File permission denied (RWE or WE)\n");
      return false;
    }

    // Call vm_map to map memory
    if (!vm_map(root, virtual_add, false, true, false)) {
      kprintf("vm_map failed\n");
      return false;
    }

    // Copying the contents of the executable
    memcpy(virtual_add, elf_file.begin + program_hdr->p_offset, program_hdr->p_filesz);

    // update permissions to reflect input
    if (!vm_protect(root, virtual_add, true, (flags & 2) >> 1, flags & 1)){
     kprintf("vm_protect failed\n");
     return false;
    }
  }

  // Pick an arbitrary location and size for the user-mode stack
    uintptr_t user_stack = 0x70000000000;

    size_t user_stack_size = 8 * PAGE_SIZE;

    // Map the user-mode-stack, somehow without <=, it will make page fault
    for(uintptr_t p = user_stack; p <= user_stack + user_stack_size; p += 0x1000) {
      // Map a page that is user-accessible, writable, but not executable
      vm_map(read_cr3() & 0xFFFFFFFFFFFFF000, p, true, true, false);
      //kprintf("%x after vmmap\n", p);
      //translate(p);
    }
    //And now jump to the entry point
    usermode_entry(USER_DATA_SELECTOR | 0x3,            // User data selector with priv=3
                    user_stack + user_stack_size - 8,   // Stack starts at the high address minus 8 bytes
                    USER_CODE_SELECTOR | 0x3,           // User code selector with priv=3
                    elf_hdr->e_entry);                     // Jump to the entry point specified in the ELF file

  // elf_exe_t current_exe = (elf_exe_t) (elf_hdr->e_entry);
  // current_exe();

  
  return true;
}

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

int sys_exec (const char *program, const char *argv[]){
  for (int i = 0; i < module_count; i++) {
    //Print name and details
    if (kstrcmp(modules[i].string, program)){
      kexec(modules[i]);
      return 1;
    }
  }
  kprintf("%s is not found\n", program);
  kexec(*shell_module);
  return 0;
}

int sys_exit (int e_code){
  switch (e_code){
    default:
      break;
  }
  kexec(*shell_module);
  return 0;
}

uint64_t virtual_fl = 0x100000000;

int sys_mmap (uintptr_t address, uintptr_t free_ptr, int prot){
  // if address NULL, peek the front of the freelist, and map its corresponding virtual address and return to user
  if (address == NULL){
    *(uintptr_t*)free_ptr = virtual_fl;
    virtual_fl += PAGE_SIZE;
    if (!vm_map(read_cr3() & (~0xFFF), *(uintptr_t*)free_ptr, true, (prot >> 1) & 1, prot & 1)){
      kprintf("Map failed with %x\n", *(uintptr_t*)free_ptr);
      return 0;
    }
  } else {
    if (!vm_map(read_cr3() & (~0xFFF), address, true, (prot >> 1) & 1, prot & 1)){
      kprintf("Map failed\n");
      return 0;
    }
    *(uintptr_t*)free_ptr = address;
  }
  return 1;
}

int sys_getline (uintptr_t buffer, size_t sz, uintptr_t fd) {
  return kgets(buffer, sz);
}

int syscall_handler(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
  switch (nr) {
    case 0:
      return sys_read(arg0, arg1, arg2);
    case 1:
      return sys_write(arg0, arg1, arg2);
    case 2:
      return sys_exec(arg0, arg1);
    case 3:
      return sys_exit(arg0);
    case 4:
      return sys_mmap(arg0, arg1, arg2);
    case 5:
      return sys_getline(arg0, arg1, arg2);
    default:
      return -1;
  }
}

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

  for (int i = 0; i < module_count; i++) {
    //Print name and details
    if (kstrcmp(modules[i].string, "init")){
      shell_module = &modules[i];
      virtual_fl = 0x100000000;
      kexec(*shell_module);
      return;
    }
  }
  kprintf("Could not find shell\n");
}
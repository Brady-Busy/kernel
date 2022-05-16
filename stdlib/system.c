#include "system.h"

extern int syscall(uint64_t nr, ...);

// Round a value x up to the next multiple of y
#define ROUND_UP(x, y) ((x) % (y) == 0 ? (x) : (x) + ((y) - (x) % (y)))

void* bump = NULL;
size_t space_remaining = 0;

void* mmap (uintptr_t address, int prot){
  uintptr_t next_free = NULL;
  syscall(SYS_mmap, address, &next_free, prot);
  return (void*)next_free;
}

// this malloc doesn't support allocating memory with more than a page
void* malloc(size_t sz) {
  printf("%p\n", &space_remaining);
  //pic_mask_irq(0);

  // Round sz up to a multiple of 16
  sz = ROUND_UP(sz, 16);
  printf("%d\n", sz);

  // Do we have enough space to satisfy this allocation?
  if (space_remaining < sz) {
    printf("here\n");
    void* newmem = mmap(NULL, 2);
    
    // Check for errors
    if (newmem == NULL) {
      //pic_unmask_irq(0);
      return NULL;
    }

    bump = newmem;
    space_remaining = PAGE_SIZE;
  }
  printf("there\n");
  // Grab bytes from the beginning of our bump pointer region
  void* result = bump;
  bump += sz;
  space_remaining -= sz;

  //pic_unmask_irq(0);
  return result;
}

void free(void* p) {
  // Do nothing
}



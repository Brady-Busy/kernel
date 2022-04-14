#include "mem.h"

extern int syscall(uint64_t nr, ...);

// Round a value x up to the next multiple of y
#define ROUND_UP(x, y) ((x) % (y) == 0 ? (x) : (x) + ((y) - (x) % (y)))

void* bump = NULL;
size_t space_remaining = 0;

void* mmap (uintptr_t address, size_t length, int prot){
    if (address == NULL){
        address = 0x700000; //some random address we need to change later
    }
    syscall(4, address, length, prot);
    return (void*)address;
}

void* malloc(size_t sz) {
  // Round sz up to a multiple of pagesize (4kb)
  sz = ROUND_UP(sz, PAGE_SIZE);

  // Do we have enough space to satisfy this allocation?
  if (space_remaining < sz) {
    // No. Get some more space using `mmap`
    size_t rounded_up = ROUND_UP(sz, PAGE_SIZE);
    void* newmem = mmap(NULL, rounded_up, 2);

    // Check for errors
    if (newmem == NULL) {
      return NULL;
    }

    bump = newmem;
    space_remaining = rounded_up;
  }

  // Grab bytes from the beginning of our bump pointer region
  void* result = bump;
  bump += sz;
  space_remaining -= sz;

  return result;
}

void free(void* p) {
  // Do nothing
}
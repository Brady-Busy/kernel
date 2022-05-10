#include "mem.h"

void memcpy(uintptr_t dest, uintptr_t const src, uint64_t size) {
  //Cast dest and src to char ptrs to copy byte by byte
  char * dest_c = (char *) dest;
  char * src_c = (char *) src;

  for (int i = 0; i < size; i++) {
    *dest_c++ = *src_c++;
  }
}

void memset (void* address, int value, size_t n) {
  char* cursor = (char*) address;
  for (int i = 0; i < n; i++) {
    *cursor = value;
    cursor++;
  }
}
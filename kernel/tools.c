#include "tools.h"

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
}

// Round a value x up to the next multiple of y
#define ROUND_UP(x, y) ((x) % (y) == 0 ? (x) : (x) + ((y) - (x) % (y)))

void* kbump = NULL;
size_t kspace_remaining = 0;
uint64_t virtual_starting = 0;

// this malloc doesn't support allocating memory with more than a page
void* kmalloc(size_t sz) {
  // Round sz up to a multiple of 16
  sz = ROUND_UP(sz, 16);

  // Do we have enough space to satisfy this allocation?
  if (kspace_remaining < sz) {
	if (virtual_starting == 0){
		virtual_starting = ptov(0x40000000);
	}
    vm_map(read_cr3() & (~0xFFF), virtual_starting, 0, 1, 1);
	void* newmem = virtual_starting;
	virtual_starting += PAGE_SIZE;
    
    // Check for errors
    if (newmem == NULL) {
      return NULL;
    }

    kbump = newmem;
    kspace_remaining = PAGE_SIZE;
  }

  // Grab bytes from the beginning of our kbump pointer region
  void* result = kbump;
  kbump += sz;
  kspace_remaining -= sz;

  return result;
}
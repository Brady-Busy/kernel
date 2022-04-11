#include "tools.h"

void kmemset (void* address, int value, size_t n) {
  char* cursor = (char*) address;
  for (int i = 0; i < n; i++) {
    *cursor = value;
    cursor++;
  }
}

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

int kstrcmp(const char* str1, const char* str2){
  while (*str1 || *str2){
    if (*(str1++) != *(str2++)) return 0;
  }
  return 1;
}
// Useful functions for use throughout

#pragma once

#include "stivale2.h"
#include <stddef.h>
#include "port.h"
#include "pic.h"

//memset
void kmemset (void* address, int value, size_t n);

void* find_tag(struct stivale2_struct* hdr, uint64_t id);

int kstrcmp(const char* str1, const char* str2);
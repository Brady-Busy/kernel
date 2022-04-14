#pragma once

#include "../kernel/kprocess.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../kernel/handler.h"

// Given an address and the protections
// If address is not NULL, mmap will attempt to map it. If address NULL, mmap will assign a virtual address
// Return a pointer to the mapped virtual address
void* mmap (uintptr_t address, int prot);

// Given a size
// Return a pointer to memory of at least that size
// Note: this malloc does not support allocating more than a page
void* malloc (size_t sz);

// Placeholder
void free(void* p);
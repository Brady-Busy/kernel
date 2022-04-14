#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

// Given a pointer to the destination, the source, and the size
// Copy the memory stored at the source address to the destination address
void memcpy (uintptr_t dest, uintptr_t src, uint64_t size);

// Given an address, desired value and size
// Set the memory at address and the subsequent n spaces to be value
void memset (void* address, int value, size_t n);
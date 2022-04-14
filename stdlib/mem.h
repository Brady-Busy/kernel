#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

void memcpy(uintptr_t dest, uintptr_t src, uint64_t size);

void memset (void* address, int value, size_t n);
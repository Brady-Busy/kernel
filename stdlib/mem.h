#pragma once

#include "../kernel/process.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../kernel/handler.h"

void* mmap (uintptr_t address, size_t length, int prot);

void* malloc (size_t sz);

void free(void* p);
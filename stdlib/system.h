#pragma once

#include "../kernel/kprocess.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../kernel/handler.h"


void* mmap (uintptr_t address, int prot);

void* malloc (size_t sz);

void free(void* p);
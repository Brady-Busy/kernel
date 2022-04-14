#pragma once

#define OFFSET 4095
#define PAGE_SIZE 4096 //assume 4kb pages

#include "kprint.h"
#include <stdbool.h>
#include <mem.h>

void fl_setup(struct stivale2_struct* hdr);

uintptr_t pmem_alloc();

void pmem_free(uintptr_t p);

void usable_mem (struct stivale2_struct* hdr);

uintptr_t read_cr3();

void translate(void* address);

bool vm_map (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);

bool vm_unmap (uintptr_t root, uintptr_t address);

bool vm_protect (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);

void unmap_lower_half(uintptr_t root);

uint64_t ptov(uint64_t physical_addr);
#pragma once

#define OFFSET 4095
#define PAGE_SIZE 4096 //assume 4kb pages

#include "kprint.h"
#include <stdbool.h>
#include <mem.h>

// set up the physical address free list
void fl_setup(struct stivale2_struct* hdr);

// returns the next free physical address in free list
uintptr_t pmem_alloc();

// append given physical address of a page to the front of free list
void pmem_free(uintptr_t p);

// print range of usable memory provided by the boot loader
void usable_mem (struct stivale2_struct* hdr);

// returns the address of lv4 page table
uintptr_t read_cr3();

// prints all four level table information of the given virtual address
void translate(void* address);

// maps a page, lv 4 - 2 will be set to kernel, writable and executable, lv 1 will be set with given permissions
bool vm_map (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);

// unmap a page, it will also free the corresponding physical page
bool vm_unmap (uintptr_t root, uintptr_t address);

// change permission of address based on given param
bool vm_protect (uintptr_t root, uintptr_t address, bool user, bool writable, bool executable);

// unmap all lower half pages
void unmap_lower_half(uintptr_t root);

// returns corresponding virtual address of the physical address, must be called after fl_setup
uint64_t ptov(uint64_t physical_addr);
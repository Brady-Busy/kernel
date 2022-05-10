// Useful functions for use throughout

#pragma once

#include "stivale2.h"
#include <stddef.h>
#include <stdint.h>
#include "kmem.h"
#include "port.h"
#include "pic.h"

// find a boot loader tag with given id
void* find_tag(struct stivale2_struct* hdr, uint64_t id);

// Given a size
// Return a pointer to memory of at least that size
// Note: this malloc does not support allocating more than a page
void* kmalloc (size_t sz);
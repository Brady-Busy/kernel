// Useful functions for use throughout

#pragma once

#include "stivale2.h"
#include <stddef.h>
#include "port.h"
#include "pic.h"

// find a boot loader tag with given id
void* find_tag(struct stivale2_struct* hdr, uint64_t id);
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stivale2.h"
#include "util.h"

#define MAX_LENGTH_HEX 17 //note: 16 plus one for a null terminating character
#define MAX_LENGTH_DEC 21

void term_setup(struct stivale2_struct* hdr);

void kprintf(const char* format, ...);

void kmemset (void* address, int value, size_t n);

int strlen(char * str);
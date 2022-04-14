#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include "stivale2.h"
#include "util.h"
#include "terminal.h"

#define MAX_LENGTH_HEX 17 //note: 16 plus one for a null terminating character
#define MAX_LENGTH_DEC 21

void kprintf(const char* format, ...);

void kprint_c(char c);

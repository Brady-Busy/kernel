#pragma once
#include "kprint.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stivale2.h"
#include "util.h"
#include "port.h"
#include "pic.h"

void idt_setup();

char kgetc();

size_t kgets (char* output, size_t capacity);
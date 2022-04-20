#pragma once

#include <mem.h>
#include "port.h"
#include "kmem.h"

// set up terminal
void term_init();

// display c on terminal
void term_putchar(char c);
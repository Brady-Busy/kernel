#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <mem.h>

#include "kprint.h"
#include "stivale2.h"
#include "util.h"
#include "port.h"
#include "pic.h"
#include "gdt.h"

// Every interrupt handler must specify a code selector. We'll use entry 5 (5*8=0x28), which
// is where our bootloader set up a usable code selector for 64-bit mode.
#define IDT_CODE_SELECTOR 0x28

// IDT entry types
#define IDT_TYPE_INTERRUPT 0xE
#define IDT_TYPE_TRAP 0xF

// Syscall Number
#define SYS_read 0
#define SYS_write 1

// setting up the idt
void idt_setup();

// set up specified handler. 0-21 and 128 is used for standard errors and systemcall
void idt_set_handler(uint8_t index, void* fn, uint8_t type);

// get next char from keyboard input and return
char kgetc();

// get capacity - 1 chars from keyboard input, return number of chars read
size_t kgets (char* output, size_t capacity);

extern int syscall(uint64_t nr, ...);
extern void syscall_entry();
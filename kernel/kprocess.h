#pragma once

#include "terminal.h"
#include "kprint.h"
#include "port.h"
#include "pic.h"
#include "handler.h"
#include "tools.h"
#include "kmem.h"
#include "elf.h"
#include "gdt.h"
#include "usermode_entry.h"
#include <mem.h>
#include <string.h>

#define SYS_read 0
#define SYS_write 1
#define SYS_exec 2
#define SYS_exit 3
#define SYS_mmap 4
#define SYS_getline 5

// Exec_setup will load all the modules from the boot loader, and execute "init" to start a shell
void exec_setup(struct stivale2_struct* hdr);

extern int syscall(uint64_t nr, ...);
extern void syscall_entry();

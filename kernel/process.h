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

void exec_setup(struct stivale2_struct* hdr);

extern int syscall(uint64_t nr, ...);
extern void syscall_entry();

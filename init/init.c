#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../kernel/handler.h"

#define SYS_read 0
#define SYS_write 1
#define SYS_exec 2
#define SYS_exit 3

extern int syscall(uint64_t nr, ...);

void _start() {
  // Issue a write system call
  syscall(SYS_write, 1, "Hello world!\n", 13);
  // char* test_page = (char*)0x400000000;
  // test_page[0] = 'h';
  // test_page[1] = 'e';
  // test_page[2] = 'l';
  // test_page[3] = 'l';
  // test_page[4] = 'o';
  // test_page[5] = '\n';
  // syscall(SYS_write, 1, test_page, 6);
  // // Loop forever
  // for(;;){}

  //char buf[20];
  //size_t cap = kgets(buf, 20);
  //syscall(SYS_write, 1, buf, cap);

  syscall(SYS_exit, 1);
}

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>

void _start() {
  // Issue a write system call
  printf("$");
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
  //syscall(SYS_write, 1, buf, cap)
  char * buf = malloc(80);
  getline(buf, 80, 0);
  exec(strtok(buf, " "), NULL);
  exit(1);
}

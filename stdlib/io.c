#include "io.h"

int read (int fd, char const *buf, int size){
  return syscall(SYS_read, fd, buf, size);
}

int write (int fd, const char *buf, int size){
  return syscall(SYS_write, fd, buf, size);
}

size_t getline (char * buf, size_t sz, char * fd) {
  return syscall(SYS_getline, buf, sz, fd);
}

void perror(const char* str) {
  syscall(SYS_write, 2, str, strlen(str));
}
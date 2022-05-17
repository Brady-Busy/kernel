#include "io.h"


extern void timer_interrupt();

int syscall_counter = 0;

void check_counter(){
  if (++syscall_counter == 5000){
    syscall_counter = 0;
    timer_interrupt();
  }
}

int read (int fd, char const *buf, int size){
  check_counter();
  return syscall(SYS_read, fd, buf, size);
}

int write (int fd, const char *buf, int size){
  check_counter();
  return syscall(SYS_write, fd, buf, size);
}

size_t getline (char * buf, size_t sz, char * fd) {
  return syscall(SYS_getline, buf, sz, fd);
}

void perror(const char* str) {
  check_counter();
  syscall(SYS_write, 2, str, strlen(str));
}

//Printing Functions Suite
void print_c(char c) {
  write(1, &c, 1);
}

void print_s(const char * str) {
  write(1, str, strlen(str));
}

void print_d(uint64_t value) {
  char num[MAX_LENGTH_DEC];
  print_s(itoa(value, num, 10));
}

void print_x(uint64_t value) {
  char hex[MAX_LENGTH_HEX];
  print_s(itoa(value, hex, 16));
}

void print_p(void* ptr) {
  uintptr_t val = (uintptr_t)ptr;
  print_s("0x");
  print_x(val);
}

void printf(const char* format, ...) {
  va_list args;
  va_start (args, format);
  const char* pos = format;

  while (*pos) {
    if (*pos == '%'){
      pos++;
      switch (*pos) {
        case '%':
          print_c('%');
          break;
        case 'd':
          print_d(va_arg(args,uint64_t));
          break;
        case 'x':
          print_x(va_arg(args,uint64_t));
          break;
        case 'c':
          print_c(va_arg(args,int));
          break;
        case 's':
          print_s(va_arg(args,char*));
          break;
        case 'p':
          print_p(va_arg(args,void*));
          break;
        default:
          print_s("Invalid wildcard specified for printing");
      }
      pos++;
    }
    else {
      print_c(*pos);
      pos++;
    }
  }
  va_end (args);
}
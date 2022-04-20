#include "kprint.h"

//Printing Functions Suite
void kprint_c(char c) {
  term_putchar(c);
}

// print string
void kprint_s(const char * str) {
  for (int i = 0 ; i < strlen(str); i++){
    term_putchar(str[i]);
  }
}

// print an integer
void kprint_d(uint64_t value) {
  char num[MAX_LENGTH_DEC];
  kprint_s(itoa(value, num, 10));
}

// print an hexadecimal
void kprint_x(uint64_t value) {
  char hex[MAX_LENGTH_HEX];
  kprint_s(itoa(value, hex, 16));
}

// print a pointer value
void kprint_p(void* ptr) {
  uintptr_t val = (uintptr_t)ptr;
  kprint_s("0x");
  kprint_x(val);
}

// kernel level printf
void kprintf(const char* format, ...){
  va_list args;
  va_start (args, format);

  const char* pos = format;

  while (*pos) {
    if (*pos == '%'){
      pos++;
      switch (*pos) {
        case '%':
          kprint_c('%');
          break;
        case 'd':
          kprint_d(va_arg(args,uint64_t));
          break;
        case 'x':
          kprint_x(va_arg(args,uint64_t));
          break;
        case 'c':
          kprint_c(va_arg(args,int));
          break;
        case 's':
          kprint_s(va_arg(args,char*));
          break;
        case 'p':
          kprint_p(va_arg(args,void*));
          break;
        default:
          kprint_s("???");
      }
      pos++;
    }
    else {
      kprint_c(*pos);
      pos++;
    }
  }

  va_end (args);
}


#include "kprint.h"

typedef void (*term_write_t) (const char*, size_t);
term_write_t term_write;

void term_setup(struct stivale2_struct* hdr) {
  // Look for a terminal tag
  struct stivale2_struct_tag_terminal* tag = find_tag(hdr, STIVALE2_STRUCT_TAG_TERMINAL_ID);

  // Make sure we find a terminal tag
  if (tag == NULL) halt();

  // Save the term_write function pointer
	term_write = (term_write_t) tag->term_write;
}

//Given a string, return the length of the string
int strlen(char * str) {
  //Confirm str is not null
  if (str == NULL) {
    return 0;
  }
  //Count until null terminator
  int count = 0;
  char * tracer = str; //start at the beginning of the string
  while (*tracer != '\0') {
    count++;
    tracer++; // move the pointer
  }
  return count;
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 * Found here: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
 * Adapted and commented by yours truly
 */
char* itoa(uint64_t value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    uint64_t tmp_value;

    do {
        tmp_value = value; //use tmp_value to store the original value
        value /= base; // update value, which allows us to update the char for pointer below
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz" [tmp_value - value * base]; //referencing a specific point in the array, this is effectively doing the mod operation
    } while ( value ); //continue until value is zero

    *ptr-- = '\0'; //null terminator
    //Based on what we have so far, this is backwards. The following while loop reverses this
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}


//Printing Functions Suite
void kprint_c(char c) {
  term_write(&c, 1);
}

void kprint_s(const char * str) {
  term_write(str, strlen(str));
}

void kprint_d(uint64_t value) {
  char num[MAX_LENGTH_DEC];
  kprint_s(itoa(value, num, 10));
}

void kprint_x(uint64_t value) {
  char hex[MAX_LENGTH_HEX];
  kprint_s(itoa(value, hex, 16));
}

void kprint_p(void* ptr) {
  uintptr_t val = (uintptr_t)ptr;
  kprint_s("0x");
  kprint_x(val);
}

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


#pragma once
#include <stdint.h>

// Inspiration taken from https://jhu-cs318.github.io/pintos-doxygen/html/annotated.html

#define READY 0
#define WAITING 1
#define RUNNING 2
#define DYING 3
#define PAGE_SIZE 4096


typedef struct  context_switch {
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rbp;
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) context_switch_t;



typedef struct {
    uintptr_t func; //ptr to function to run
    uint32_t thread_id;
    uint8_t state; // running, waiting, ready, dying
    char name[8]; 
    uint8_t *stack; 
    unsigned buffer;
    void * args; // arguments for the function
    context_switch_t* contextSaved;
} thread_t;

int thread_create(thread_t * memory, const char * name, uintptr_t func, void * args); 


/** use this to create the context of the function it is in and pass it the thread it is switching to
 * fn is the thread to jump to
*/
extern int context(thread_t fn);
extern void context_entry();
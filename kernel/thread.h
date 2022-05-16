#pragma once
#include <stdint.h>
#include "pic.h"
#include <string.h>
#include "kmem.h"

// Inspiration taken from https://jhu-cs318.github.io/pintos-doxygen/html/annotated.html

#define READY 0
#define WAITING 1
#define RUNNING 2
#define DYING 3
#define PAGE_SIZE 4096

/*
 * context_switch structure
 * This struct stores the registers as well as the instruction pointer, code selector, flags etc
 * In other words, it contains the necessary information to store the context of a thread
*/
typedef struct context_switch {
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

/*
 * thread structure
 * This struct stores the function pointer, state, thread id, name, stack pointer, buffer, function arguments and context of a thread
 * Note: this was modeled roughly after the pthread_t struct
*/
typedef struct thread {
    uintptr_t func; //ptr to function to run
    uint32_t thread_id;
    uint8_t state; // running, waiting, ready, dying
    char name[8]; 
    uint8_t *stack; 
    unsigned buffer; // future implementations can create a buffer as a safeguard against overwriting
    void * args; // arguments for the function
    context_switch_t contextSaved;
} thread_t;

/*
 * thread_list structure
 * This struct contains the necessary information about a list of threads
 * This includes to number of threads, the pointer to where all thread stacks are located, the current running thread and an array of all threads. 
*/
typedef struct thread_list {
  uint32_t thread_num;
  uint64_t stack_ptr;
  uint32_t current_running;
  thread_t* lst[512];
} thread_list_t;

/*
 * thread_init function
 * This function takes no arguments and initializes our global thread_list struct so that we are ready for users to create threads
*/
void thread_init();

/*
 * thread_create function
 * This function creates a thread internally
 * Arguments: a pointer to a space in memory for the thread struct, the name for the thread, a pointer to the function the new thread should run and a pointer to the arguments for that function
 * Returns: a uint64_t pointer to the stack of that thread
*/
uint64_t thread_create(thread_t * memory, const char * name, uintptr_t func, void * args);

/*
 * create_thread function
 * This function creates a thread specified by the user
 * Arguments: a pointer to a space in memory for the thread struct, the name for the thread, a pointer to the function the new thread should run and a pointer to the arguments for that function
 * Returns: thread_id
*/
extern int create_thread(thread_t * memory, const char * name, uintptr_t func, void * args);

/*
 * context_entry function
 * This function is an interrupt entry that invokes a system call, updates the registers and invokes the context_handler
 * Arguments: None
 * Returns: None
*/
extern void context_entry();

/*
 * end_current function
 * This function marks a thread as "done"
 * Arguments: none (updates the current thread that is stored in the global struct)
 * Returns: thread_id
*/
void end_current();
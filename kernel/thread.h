#pragma once
#include <stdlib.h>
// Inspiration taken from https://jhu-cs318.github.io/pintos-doxygen/html/annotated.html

#define READY 0
#define WAITING 1
#define RUNNING 2
#define DYING 3
#define PAGE_SIZE 4096
typedef struct {
    uintptr_t func; //ptr to function to run
    uint32_t thread_id;
    uint8_t state; // running, waiting, ready, dying
    char name[8]; 
    uint8_t *stack; 
    unsigned buffer;
    void * args; // arguments for the function
} thread_t;

int thread_create(thread_t * memory, const char * name, uintptr_t func, void * args); 
#include "thread.h"

uintptr_t func; //ptr to function to run
    uint32_t thread_id;
    uint8_t state; // running, waiting, ready, dying
    char name[8]; 
    uint8_t *stack; 
    unsigned buffer;
    void * args; // arguments for the function

int thread_create(thread_t * memory, const char * name, uintptr_t func, void * args) {
    memory->func = func;
    memory->thread_id = 1; // TODO update this so this is different for each thread
    memory->state = 0; // set to ready
    strcpy(memory->name, name);
    memory->stack = PAGE_SIZE;
    memory->buffer = 0; // TODO ask Charlie
    memory->args = args;
    // TODO add to task queue
    return memory->thread_id; // TODO change to where stored and returned locally
}
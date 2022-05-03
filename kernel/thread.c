#include "thread.h"

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


/** 
 * context is the context of the function we are currently in
 * we create a new thread not to jump to but save into global struct
 * save its context same as its parent except for ip, argument, and sp
*/
void context_handler(context_switch_t* context){
    //check code selector
    //change function to justr save the thread
    thread_t* m = (thread_t*)context->rdi;
    const char* n = (const char*)context->rsi;
    uintptr_t func = (uintptr_t) context->rdx;
    void* args = (void*)context->rcx;
    
    thread_create(m, n, func, args);
    
    //save context to current running thread here
    memcpy(m->contextSaved, context, sizeof(context_switch_t));//they've the same process
    m->contextSaved.ip = func;
    m->contextSaved.rdi = args;
    m->contextSaved.sp = m->stack;
    // int id = save_thread();
    // m->thread_id = id;
    // context->rax = id;
    //save m into the global struct
   
    // kprintf("hello from created thread with id: %d\n", id);
}




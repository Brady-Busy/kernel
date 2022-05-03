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
 * context is the context of the function we are leaving
 * somehow I will save it in the current running thread i.e the one we are leaving
 * this will be used if th euser causes an interupt and creates a new thread.
*/
void context_handler(context_switch_t* context){

    thread_t* m = (thread_t*)context->rdi;
    const char* n = (const char*)context->rsi;
    uintptr_t func = (uintptr_t) context->rdx;
    void* args = (void*)context->rcx;
    

    //save context to current running thread here
    thread_create(m, n, func, args);//??
    context->ip = func;
    context->rdi = args;
    context->sp = m->stack;
    
    m->contextSaved = context;
    kprintf("hello thread\n");
}


// void switch_thread(thread_t* from, thread_t* to){
//     //save c
// }

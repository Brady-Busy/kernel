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
 * context has a thread as its first parameter in register rdi
 * this thread holds the information of the function i want to jumo to
*/
void context_handler(context_switch_t* context){

    //save context to current running thread here
    thread_t* jumpTo = (thread_t*)context->rdi;
    uintptr_t function = jumpTo->func;
    //do i need to check the thread state
    //if it is running it could mean that it had a context saved so i can copy from it
    // context = jumpTo->contextSaved;
    context->sp = jumpTo->stack;
    context->rdi = jumpTo->args;
    context->ip = function;
    kprintf("%p\n", context->ip);
    kprintf("%p\n", context->rdi);
    //if new thread then save the current context created
    // jumpTo->contextSaved = context;
    kprintf("hello thread\n");
}

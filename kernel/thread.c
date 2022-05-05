#include "thread.h"

thread_list_t global_thread;

void thread_init(){
    global_thread.thread_num = 0;
    global_thread.current_running = 0;
    global_thread.stack_ptr = 0x200000000;
}

int thread_create(thread_t * memory, const char * name, uintptr_t func, void * args) {
    memory->func = func;
    memory->thread_id = global_thread.thread_num; // TODO update this so this is different for each thread
    memory->state = 0; // set to ready
    strcpy(memory->name, name);
    // map one page stack
    vm_map(read_cr3() & (~(0xFFF)), global_thread.stack_ptr, 1, 1, 0);
    global_thread.stack_ptr += PAGE_SIZE;
    // stack starts at higher address of the mapped stack - 8
    memory->stack = global_thread.stack_ptr - 8;
    memory->buffer = 0; // TODO ask Charlie
    memory->args = args;
    // TODO add to task queue
    global_thread.lst[global_thread.thread_num++] = memory;
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

thread_t * next_thread(){
    while(!global_thread.lst[(++global_thread.current_running) % global_thread.thread_num] -> state){
        global_thread.current_running = global_thread.current_running % global_thread.thread_num;
        return global_thread.lst[global_thread.current_running];
    }
}

void scheduler_handler(context_switch_t* context) {
    context_switch_t * saves = global_thread.lst[global_thread.current_running] -> contextSaved;
    saves->ip = context->ip;
    saves->sp = context->sp;

    context_switch_t * next = next_thread()->contextSaved;

    context->ip  = next->ip;
    context->sp = next->sp;
    context->rdi = next->rcx;
    return;
}


// void switch_thread(thread_t* from, thread_t* to){
//     //save c
// }

#include "thread.h"

thread_list_t global_thread;

void thread_init(){
    global_thread.thread_num = 0;
    global_thread.current_running = 0;
    global_thread.stack_ptr = 0x200000000;
    // Unmask irq0 which is the timer interrupt
    pic_unmask_irq(0);
}

int thread_create(thread_t * memory, const char * name, uintptr_t func, void * args) {
    memory->func = func;
    memory->thread_id = global_thread.thread_num; // TODO update this so this is different for each thread
    memory->state = 0; // set to ready
    strcpy(&memory->name, name);
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
    kprintf("anything wrong?\n");
    
    thread_create(m, n, func, args);
    
    //save context to current running thread here
    memcpy(&m->contextSaved, context, sizeof(context_switch_t));//they've the same process
    m->contextSaved.ip = func;
    m->contextSaved.rdi = args;
    m->contextSaved.sp = m->stack;
    // int id = save_thread();
    // m->thread_id = id;
    // context->rax = id;
    //save m into the global struct
   
    // kprintf("hello from created thread with id: %d\n", id);
    kprintf("after context handler\n");
}

thread_t * next_thread(){
    while(!global_thread.lst[(++global_thread.current_running) % global_thread.thread_num] -> state){
        global_thread.current_running = global_thread.current_running % global_thread.thread_num;
    }
    return global_thread.lst[global_thread.current_running];
}

void scheduler_handler(context_switch_t* context) {
    kprintf("in handler\n");
    // turn off timer interrupt
    pic_mask_irq(0);
    if (global_thread.thread_num <= 1){
        return;
    }
    context_switch_t * saves = &(global_thread.lst[global_thread.current_running] -> contextSaved);
    // save the instruction pointer and the stack
    saves->ip = context->ip;
    saves->sp = context->sp;

    context_switch_t * next = &(next_thread()->contextSaved);
    // swich to next context
    context->ip  = next->ip;
    context->sp = next->sp;

    // turn on timer interrupt again
    pic_unmask_irq(0);

    return;
}




#include "thread.h"

thread_list_t global_thread;

void thread_init(){
    global_thread.thread_num = 0;
    global_thread.current_running = 0;
    global_thread.stack_ptr = 0x200000000;
}

uint64_t thread_create(thread_t * memory, const char * name, uintptr_t func, void * args) {
    pic_mask_irq(0);
    memory->func = func;
    memory->thread_id = global_thread.thread_num;
    memory->state = 0; // set to ready
    strcpy(&memory->name, name);
    // map 8 page stack
    for(int i = 0; i < 4; i++) {
        // Map a page that is user-accessible, writable, but not executable
        vm_map(read_cr3() & 0xFFFFFFFFFFFFF000, global_thread.stack_ptr, 1, 1, 0);
        global_thread.stack_ptr += PAGE_SIZE;
    }
    // stack starts at higher address of the mapped stack - 8
    memory->stack = global_thread.stack_ptr - 8;
    memory->buffer = 0; // Set as a default. Future implementations can build upon this
    memory->args = args;
    global_thread.lst[global_thread.thread_num++] = memory;
    // Unmask irq0 which is the timer interrupt
    pic_unmask_irq(0);
    return memory->stack;
}


/** 
 * context is the context of the function we are currently in
 * we create a new thread and save it into the global struct
 * save its context same as its parent except for ip, argument, and sp
*/
void context_handler(context_switch_t* context){
    pic_mask_irq(0);

    //change function to just save the thread
    thread_t* m = (thread_t*)context->rdi;
    const char* n = (const char*)context->rsi;
    uintptr_t func = (uintptr_t) context->rdx;
    void* args = (void*)context->rcx;
    thread_create(m, n, func, args);
    int id = m->thread_id;

    //save context to current running thread here
    memcpy(&(m->contextSaved), context, sizeof(context_switch_t));//they are in the same process
    m->contextSaved.ip = func;
    m->contextSaved.rdi = args;
    m->contextSaved.sp = m->stack;
    context->rax = id;
    outb(PIC1_COMMAND, PIC_EOI);
    pic_unmask_irq(0);
}

/** 
 * next_thread returns a pointer to the next thread to be run
*/
thread_t * next_thread(){
    int cursor = global_thread.current_running;

    // Check for next available thread
    while(global_thread.lst[(++cursor) % global_thread.thread_num] -> state);

    // If the next available thread is the current thread (there are no other ready threads), return NULL
    if (global_thread.current_running == cursor % global_thread.thread_num){
        return NULL;
    }

    //Update current thread in the global struct and return
    global_thread.current_running = cursor % global_thread.thread_num;
    return global_thread.lst[global_thread.current_running];
}

int total_switch = 0;

void scheduler_handler(context_switch_t* context) {
    // turn off timer interrupt
    pic_mask_irq(0);
    uint64_t hold = context->ip;
    if(hold >> 63){
        outb(PIC1_COMMAND, PIC_EOI);
        pic_unmask_irq(0);
        return;
    }

    if (global_thread.thread_num <= 1){
        // kprintf("only child\n");
        outb(PIC1_COMMAND, PIC_EOI);
        pic_unmask_irq(0);
        return;
    }

    // save the instruction pointer and the stack
    memcpy(&(global_thread.lst[global_thread.current_running] -> contextSaved), context, sizeof(context_switch_t));
    context_switch_t * next = &(next_thread()->contextSaved);

    if (next == NULL){
        outb(PIC1_COMMAND, PIC_EOI);
        pic_unmask_irq(0);
        return;
    }

    memcpy(context, next, sizeof(context_switch_t));
    total_switch++;
    // turn on timer interrupt again
    pic_unmask_irq(0);
    outb(PIC1_COMMAND, PIC_EOI);
    return;
}

void end_current(){
    global_thread.lst[global_thread.current_running]->state = 1;
}

void clean_threads(){
    thread_init();
    for(int i = 0; i < 512; i++){
        global_thread.lst[i] = NULL;
    }
}

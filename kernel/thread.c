#include "thread.h"

thread_list_t global_thread;

void thread_init(){
    global_thread.thread_num = 0;
    global_thread.current_running = 0;
    global_thread.stack_ptr = 0x200000000;
}

uint64_t thread_create(thread_t * memory, const char * name, uintptr_t func, void * args) {
    pic_mask_irq(0);
    kprintf("thread_create called\n");
    memory->func = func;
    memory->thread_id = global_thread.thread_num; // TODO update this so this is different for each thread
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
    memory->buffer = 0; // TODO ask Charlie
    memory->args = args;
    // TODO add to task queue
    global_thread.lst[global_thread.thread_num++] = memory;
    // Unmask irq0 which is the timer interrupt
    pic_unmask_irq(0);
    return memory->stack; // TODO change to where stored and returned locally
}


/** 
 * context is the context of the function we are currently in
 * we create a new thread not to jump to but save into global struct
 * save its context same as its parent except for ip, argument, and sp
*/
void context_handler(context_switch_t* context){
    pic_mask_irq(0);

    //check code selector
    //change function to justr save the thread
    thread_t* m = (thread_t*)context->rdi;
    const char* n = (const char*)context->rsi;
    uintptr_t func = (uintptr_t) context->rdx;
    void* args = (void*)context->rcx;
    kprintf("anything wrong?\n");
    
    thread_create(m, n, func, args);
    int id = m->thread_id;

    //save context to current running thread here
    memcpy(&(m->contextSaved), context, sizeof(context_switch_t));//they've the same process
    m->contextSaved.ip = func;
    m->contextSaved.rdi = args;
    m->contextSaved.sp = m->stack;
    
    context->rax = id;
    //save m into the global struct
   
    // kprintf("hello from created thread with id: %d\n", id);
    kprintf("after context handler\n");
    pic_unmask_irq(0);
}

thread_t * next_thread(){
    int cursor = global_thread.current_running;
    // kprintf("%d %d\n", global_thread.current_running, global_thread.thread_num);
    while(global_thread.lst[(++cursor) % global_thread.thread_num] -> state);
    if (global_thread.current_running == cursor % global_thread.thread_num){
        return NULL;
    }
    global_thread.current_running = cursor % global_thread.thread_num;
    // if (global_thread.current_running == 0) {kprintf("first thread\n");}
    return global_thread.lst[global_thread.current_running];
}

int total_switch = 0;

void scheduler_handler(context_switch_t* context) {
    // if(total_switch == 3){
    //     kprintf("three times");
    //     outb(PIC1_COMMAND, PIC_EOI);
    //     return;
    // }
    //kprintf("in handler\n");
    // turn off timer interrupt
    pic_mask_irq(0);
    uint64_t hold = context->ip;
    //kprintf("here %x\n", context->ip);
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

    

    // kprintf("Z\n");
    // context_switch_t * saves = &(global_thread.lst[global_thread.current_running] -> contextSaved);
    // save the instruction pointer and the stack
    memcpy(&(global_thread.lst[global_thread.current_running] -> contextSaved), context, sizeof(context_switch_t));
    // (global_thread.lst[global_thread.current_running] -> contextSaved).sp = global_thread.lst[global_thread.current_running]->stack;
    // kprintf("A\n");
    context_switch_t * next = &(next_thread()->contextSaved);
    if (next == NULL){
        outb(PIC1_COMMAND, PIC_EOI);
        pic_unmask_irq(0);
        return;
    }
    //kprintf("got next as %p\n", next->ip);
    //kprintf("leaving contxt at %p\n", context->ip);
    //kprintf("accessing stack at %p\n", context->sp);
    //kprintf("switched %d times\n", total_switch++);
    memcpy(context, next, sizeof(context_switch_t));
    // kprintf("B\n");
    total_switch++;
    // turn on timer interrupt again
    pic_unmask_irq(0);
    outb(PIC1_COMMAND, PIC_EOI);
    return;
}

void end_current(){
    global_thread.lst[global_thread.current_running]->state = 1;
}


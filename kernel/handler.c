#include "handler.h"
#include "thread.h"
// interupt handler stuff

// Create and initialize keyboard code array
char lower[241] = {'\0'};
char upper[241] = {'\0'};

// List all printable keyboard chars
char * dict = "1234567890"
              "-=\b\0qwerty"
              "uiop[]\n\0as"
              "dfghjkl;'`"
              "\0\\zxcvbnm,"
              "./\0*\0 \0\0\0\0"
              "\0\0\0\0\0\0\0\0\0";
char * shift_dict = "!@#$%^&*()"
                    "_+\b\0QWERTY"
                    "UIOP{}\n\0AS"
                    "DFGHJKL:\"~"
                    "\0|ZXCVBNM<"
                    ">?\0*\0 \0\0\0\0"
                    "\0\0\0\0\0\0\0\0\0";

// setting up cyclic buffer for getc and getline
char key_buffer[128];
int reader = 0;
int writer = 0;
volatile size_t length = 0;

// global variable keep track of if shift is pressed
int shift_pressed = 0;

// Initialize all of the scan codes
void init_scan_dict() {
  // only set for printable elements
  for (int i = 2; i < (5 * 16) + 6; i++) {
      lower[i] = dict[i-2];
      upper[i] = shift_dict[i-2];
  }
}

// interupt context struct used for handlers
typedef struct interrupt_context {
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

// setting up all the standard interupt handles based on ms mannual.
__attribute__((interrupt))
void divide_error_handler(interrupt_context_t* ctx) {
  kprintf("Error: Divide Error. See DIV and IDIV instructions\n");
  halt();
}

__attribute__((interrupt))
void debug_handler(interrupt_context_t* ctx) {
  kprintf("Error: Debug. Check all code and data references\n");
  halt();
}

__attribute__((interrupt))
void nmi_interrupt_handler (interrupt_context_t* ctx) {
  kprintf("Error: NMI Interrupt (Non-maskable external interrupt\n");
  halt();
}

__attribute__((interrupt))
void breakpoint_handler (interrupt_context_t* ctx) {
  kprintf("Error: Breakpoint. See INT3 Instruction.\n");
  halt();
}

__attribute__((interrupt))
void overflow_handler (interrupt_context_t* ctx) {
  kprintf("Error: Overflow. See INTO instruction.\n");
  halt();
}

__attribute__((interrupt))
void bound_range_handler (interrupt_context_t* ctx) {
  kprintf("Error: BOUND Range Exceeded. See BOUND instruction\n");
  halt();
}

__attribute__((interrupt))
void invalid_opcode_handler (interrupt_context_t* ctx) {
  kprintf("Error: Invalid Opcode (Undefined Opcode). See UD Instruction or reserved opcode\n");
  halt();
}

__attribute__((interrupt))
void device_unavailable_handler (interrupt_context_t* ctx) {
  kprintf("Error: Device Not Available (No Math Coprocessor). See floating point or WAIT/FWAIT instructions\n");
  halt();
}

__attribute__((interrupt))
void double_fault_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Double Fault. Code: %d Check any instruction that can generate an exception, an NMI, or an INTR\n", code);
  halt();
}

__attribute__((interrupt))
void segment_overrun_handler (interrupt_context_t* ctx) {
  kprintf("Error: CoProcessor Segment Overrun (reserved). See floating-point instruction\n");
  halt();
}

__attribute__((interrupt))
void invalid_tss_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Invalid TSS. Code: %d. Check task switches and TSS accesses\n", code);
  halt();
}

__attribute__((interrupt))
void segment_not_present_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Segment Not Present. Code: %d. Check loading segment registers or accessing system segments\n", code);
  halt();
}

__attribute__((interrupt))
void stack_segfault_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Stack Segment Fault. Code: %d. Check stack operations and SS register loads\n", code);
  halt();
}

// read_cr2() is used to get address of the page access that caused page fault
uintptr_t read_cr2() {
  uintptr_t value;
  __asm__("mov %%cr2, %0" : "=r" (value));
  return value;
}
__attribute__((interrupt))
void page_fault_handler(interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Page fault. Code %d. Access failed with %p.\n", code, read_cr2());
  halt();
}

__attribute__((interrupt))
void general_protection_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: General Protection. Code: %d. Check any memory references or other protection checks\n", code);
  halt();
}

__attribute__((interrupt))
void floating_point_handler (interrupt_context_t* ctx) {
  kprintf("Error: Floating-Point Error (Math Fault). Check floating-point or WAIT/FWAIT instructions\n");
  halt();
}

__attribute__((interrupt))
void alignment_check_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Alighment Check. Code: %d. Any data reference in memory\n", code);
  halt();
}

__attribute__((interrupt))
void machine_check_handler (interrupt_context_t* ctx) {
  kprintf("Error: Machine check. Remember, codes are model dependent\n");
  halt();
}

__attribute__((interrupt))
void simd_fp_handler (interrupt_context_t* ctx) {
  kprintf("Error: SIMD floating-point exception\n");
  halt();
}

__attribute__((interrupt))
void virtualization_exception_handler (interrupt_context_t* ctx) {
  kprintf("Error: Virtualization exception (EPT violation)\n");
  halt();
}

__attribute__((interrupt))
void control_protection_handler (interrupt_context_t* ctx, unsigned long code) {
  kprintf("Error: Control protection exception. Code: %d. See documentation for details\n", code);
  halt();
}

__attribute__((interrupt))
void irq1_handler(interrupt_context_t* ctx) {

  // Get the scan code
  int code = inb(0x60);

  // Check if it is shift
  if (code == 42 || code == 54) {
    shift_pressed++;
    outb(PIC1_COMMAND, PIC_EOI);
    return;
  } else if (code == 170 || code == 182) {
    shift_pressed--;
    outb(PIC1_COMMAND, PIC_EOI);
    return;
  }

  if (shift_pressed) {
    if (upper[code] && length < 128) {
      key_buffer[length++, writer++] = upper[code];
      writer %= 128;
      term_putchar(upper[code]);
    } else if (!upper[code]) {
      // reserved for implementing functional keys
    } else {
      // buffer full, \a didn't work, so just do nothing
    }
  } else {
    if (lower[code] && length < 128) {
      key_buffer[length++, writer++] = lower[code];
      writer %= 128;
      term_putchar(lower[code]);
    } else if (!lower[code]) {
      // reserved for implementing functional keys
    } else {
      // buffer full, \a didn't work, so just do nothing
    }
  }

  // allow another keyboard interrupt
  outb(PIC1_COMMAND, PIC_EOI);
}


__attribute__((interrupt))
void irq0_handler(interrupt_context_t* ctx) {

  kprintf("tick\n");

  // allow another keyboard interrupt
  outb(PIC1_COMMAND, PIC_EOI);
}

// return next char in buffer
char kgetc () {
  reader %= 128;
  // if length is 0, wait
  while (!length);
  // decrement length, then return what is at reader
  return length--, key_buffer[reader++];
}

// get capacity - 1 chars from buffer
size_t kgets (char* output, size_t capacity) {
  size_t chars_read = 0;
  while (chars_read < capacity - 1) {
    char current = kgetc();
    // if we reach new line, return what we have
    if (current == '\n') {
      output[chars_read] = '\0';
      return chars_read;
    } else if (current == '\b'){
      // if backspace and there aren't any characters, scream
      if (!chars_read){
        kprintf("\a");
      } else{
        chars_read--;
      }
    } else {
      output[chars_read++] = current; 
    }
  }
  output[chars_read] = '\0';
  return chars_read;
}


// A struct the matches the layout of an IDT entry
typedef struct idt_entry {
  uint16_t offset_0;
  uint16_t selector;
  uint8_t ist : 3;
  uint8_t _unused_0 : 5;
  uint8_t type : 4;
  uint8_t _unused_1 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t _unused_2;
} __attribute__((packed)) idt_entry_t;

// Make an IDT
idt_entry_t idt[256];


/**
 * Set an interrupt handler for the given interrupt number.
 *
 * \param index The interrupt number to handle
 * \param fn    A pointer to the interrupt handler function
 * \param type  The type of interrupt handler being installed.
 *              Pass IDT_TYPE_INTERRUPT or IDT_TYPE_TRAP from above.
 */
void idt_set_handler(uint8_t index, void* fn, uint8_t type) {
  idt_entry_t* current = &(idt[index]);
  intptr_t handler = (intptr_t) fn; 

  current->offset_0 = handler;
  current->offset_1 = handler >> 16;
  current->offset_2 = handler >> 32;

  current->type = type;

  current->present = 1;
  current->dpl = 3; // Allow users to evoke interrupts
  current->ist = 0;
  current->selector = KERNEL_CODE_SELECTOR;
}

// This struct is used to load an IDT once we've set it up
typedef struct idt_record {
  uint16_t size;
  void* base;
} __attribute__((packed)) idt_record_t;

/**
 * Initialize an interrupt descriptor table, set handlers for standard exceptions, and install
 * the IDT.
 */
void idt_setup() {
  init_scan_dict();
  // Step 1: Zero out the IDT, probably using memset (which you'll have to implement)
  // Write me!
  memset (&idt, 0, 256*sizeof(idt_entry_t));

  // Step 2: Use idt_set_handler() to set handlers for the standard exceptions (1--21)
  // Write me!
  idt_set_handler(0, divide_error_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(1, debug_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(2, nmi_interrupt_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(3, breakpoint_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(4, overflow_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(5, bound_range_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(6, invalid_opcode_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(7, device_unavailable_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(8, double_fault_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(9, segment_overrun_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(10, invalid_tss_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(11, segment_not_present_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(12, stack_segfault_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(13, general_protection_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(14, page_fault_handler, IDT_TYPE_INTERRUPT);
  //Note, 15 is reserved and thus not initialized here
  idt_set_handler(16, floating_point_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(17, alignment_check_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(18, machine_check_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(19, simd_fp_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(20, virtualization_exception_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(21, control_protection_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(IRQ0_INTERRUPT, irq0_handler, IDT_TYPE_INTERRUPT);
  idt_set_handler(IRQ1_INTERRUPT, irq1_handler, IDT_TYPE_INTERRUPT);
  // setting up system call
  idt_set_handler(0x80, syscall_entry, IDT_TYPE_TRAP);
   idt_set_handler(0x90, context_entry, IDT_TYPE_TRAP);

  // Step 3: Install the IDT
  idt_record_t record = {
    .size = sizeof(idt),
    .base = idt
  };
  __asm__("lidt %0" :: "m"(record));
}

#include "handler.h"

// interupt handler stuff

// Create a non-shift dictionary
char lower[216];

char * dict = "1234567890-=\0\0qwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 \0
\0\0\0\0\0\0\0\0\0\0\0\0";
// Initialize all of the press
// Scan code as access
void init_scan_dict() {
  // Initialize lower dictionary
  lower[1] = '\0';
  
  // Set base elements

  for (int i = 2; i < (3 * 16) + 7; i++) {
      lower[i] = dict[i-2];
  }

}





typedef struct interrupt_context {
  uintptr_t ip;
  uint64_t cs;
  uint64_t flags;
  uintptr_t sp;
  uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

__attribute__((interrupt))
void std_handler(interrupt_context_t* ctx) {
  kprintf("Some error occured\n");
  halt();
}

__attribute__((interrupt))
void irq1_handler(interrupt_context_t* ctx) {
  kprintf("%c", lower[inb(0x60)]);
  outb(PIC1_COMMAND, PIC_EOI);
}

// Every interrupt handler must specify a code selector. We'll use entry 5 (5*8=0x28), which
// is where our bootloader set up a usable code selector for 64-bit mode.
#define IDT_CODE_SELECTOR 0x28

// IDT entry types
#define IDT_TYPE_INTERRUPT 0xE
#define IDT_TYPE_TRAP 0xF

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
  current->dpl = 0;
  current->ist = 0;
  current->selector = IDT_CODE_SELECTOR;
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
  kmemset (&idt, 0, 256*sizeof(idt_entry_t));

  // Step 2: Use idt_set_handler() to set handlers for the standard exceptions (1--21)
  // Write me!
  for (int i = 0; i < 21; i++) {
    idt_set_handler (i, std_handler, IDT_TYPE_INTERRUPT);
  }

  // Set up to handle PIC_1 (keyboard interrupt)
  idt_set_handler(IRQ1_INTERRUPT, irq1_handler, IDT_TYPE_INTERRUPT);

  // Step 3: Install the IDT
  idt_record_t record = {
    .size = sizeof(idt),
    .base = idt
  };
  __asm__("lidt %0" :: "m"(record));
}

#include "multiboot.h"

/****************************************/
/*				STATIC					*/
/****************************************/

static idt_entry_t idt[256];
static idtr_t idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];
extern void* isr_stub_table[];
extern void isr_stub_keyboard(void);

// inits a single idt
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
	// code segment, the space to compute code in the kernel
    descriptor->kernel_cs      = 0x10;
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void idt_keyboard(){
	idt_set_descriptor(0x21, isr_stub_keyboard, 0x8E);
	vectors[0x21] = true;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }
	idt_keyboard();

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}

__attribute__((noreturn)) void exception_handler(){
	while (1){
		__asm__ volatile ("cli;hlt");
	}
}

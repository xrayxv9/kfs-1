#include "multiboot.h"

static idt_entry_t idt[256];
static idtr_t idtr;
static bool vectors[IDT_MAX_DESCRIPTORS];
extern void* isr_stub_table[];

// inits a single idt
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08;
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}

__attribute__((noreturn)) void exception_handler(){
	while (1){
		__asm__ volatile ("cli;hlt");
	}
}

uint8_t	*init_font(){
	// beginning of the font
	char *font_ptr = _binary_Lat15_Terminus16_psf_start;

	// header of the fonts, followed by the glyphs
	PSF1_Header *header = (PSF1_Header *) font_ptr;

	if (header->magic != PSF_FONT_MAGIC){
		// PSF invalid
		// add a print kernel
		return 0x0;
	}

	// Getting the Glyphs
	uint8_t *glyphs = (uint8_t *)(font_ptr + sizeof(PSF1_Header));
	return glyphs;
}

int kernel_main() {
	uint8_t		*glyphs;

	glyphs = init_font();
	if (glyphs == 0x0)
		return 1;
	return 0;
}

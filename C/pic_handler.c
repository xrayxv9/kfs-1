#include "multiboot.h"

/****************************************/
/*				UTILS					*/
/****************************************/

// inline is made to avoir the function call, instead the compiler remplace the function call with the pure code
void outb(uint16_t port, uint8_t val){
	// sends value to PIC
	__asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

// Reads the value written in the asked port of the PIC
uint8_t inb(uint16_t port) {
	uint8_t ret;
	// gives the port and the ret value to read and put in ret the value written
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

// Little break (similar to a await())
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/****************************************/
/*				DEBUG FCT				*/
/****************************************/

static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}


/****************************************/
/*				INIT UTILS				*/
/****************************************/

void PIC_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}


// The io_wait is needed between every steps because it takes time to handle their bytes 
void PIC_remap(int offset1, int offset2){
	// Init The PICS and tells them that ICW4 will get configured later on
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); /* Initialise PIC1 */
	io_wait();

	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	// Gives both of the offset needed to send infos to the right part of the IDT
	outb(PIC1_DATA, offset1);
	io_wait();
	
	outb(PIC2_DATA, offset2);
	io_wait();

	// Maps the cascade betwwen Mater / Slave
	outb(PIC1_DATA, 1 << CASCADE_IRQ); /* we put it on the third IRQ line */
	io_wait();

	outb(PIC2_DATA, CASCADE_IRQ);
	io_wait();

	// Configure the ICW4 to use the 8086 system 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	PIC_disable();
	// Unmask both PICs. (unlocks all the IRQs)
	// outb(PIC1_DATA, 0);
	// outb(PIC2_DATA, 0);
}


// Disable a line in The IRQ (c.f: disable keyboard, mouse..)
void IRQ_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

// Activates a line in the IRQ (c.f: keyboard, mouse..)
void IRQ_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}


void PIC_sendEOI(uint8_t irq){
	if (irq >= 8){
		outb(PIC2_COMMAND, PIC_EOI);
	}
	outb(PIC1_COMMAND, PIC_EOI);
	
}

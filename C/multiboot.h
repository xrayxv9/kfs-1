#ifndef BOOT_H
#define BOOT_H

/****************************************/
/*				EXTERNAL LIBS			*/
/****************************************/


#include <stdbool.h>

// Obtained By looking at the objdump -t of the font.o
extern char _binary_Lat15_Terminus16_psf_start[];
extern char _binary_Lat15_Terminus16_psf_end[];
extern char _binary_Lat15_Terminus16_psf_size[];

/****************************************/
/*				MACROS					*/
/****************************************/

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/****************************************/
/*				DEFINE					*/
/****************************************/

#define PSF_FONT_MAGIC				0x0436
#define IDT_MAX_DESCRIPTORS			256
#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */
#define LSHIFT_SCANCODE_PRESSED		0x2A
#define LSHIFT_SCANCODE_RELEASED	0xAA
#define RSHIFT_SCANCODE_PRESSED		0x36
#define RSHIFT_SCANCODE_RELEASED	0xB6
#define CAPSLOCK_SCANCODE			0x3A
#define WHITE						0x00FFFFFF
#define BLACK						0x00000000

// PIC = Programmable Interrupt Controller
#define PIC1			0x20		/* Master's address */
#define PIC2			0xA0		/* Slave's address */
#define PIC1_COMMAND	PIC1		/* Master's command 'port' */
#define PIC1_DATA		(PIC1+1)	/* Master's data 'port' */
#define PIC2_COMMAND	PIC2		/* Slave's command 'port' */
#define PIC2_DATA		(PIC2+1)	/* Slave's data 'port' */
#define PIC_EOI			0x20		/* Data to send once the interrupt has been taken care of */

// ICW = Initialization command Words (a code to init PIC)
// ICW2: vector's offset, to not say that you keyboard interrupt is a system interupt 
// ICW3: Tells how the Master / Slave are wired
// ICW4: Additional information system related

#define ICW1_ICW4		0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE		0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL		0x08		/* Level triggered (edge) mode */
#define ICW1_INIT		0x10		/* Initialization - required! */

#define ICW4_8086		0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM		0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

/****************************************/
/*				TYPEDEF					*/
/****************************************/

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
typedef unsigned int		uintptr_t;

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long long	int64_t;
typedef signed int			intptr_t;

/****************************************/
/*				STRUCT					*/
/****************************************/

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    
    uint32_t syms[4];				/* Union pour la table des symboles (aout ou elf) : prend 16 octets */

    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    
    /* VBE info (VESA info) */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    /* Framebuffer configuration (aigned at buffer offset) */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    
    /* end of struct / color mask buffer */
    union {
        struct {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
        struct {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
} __attribute__((packed));

typedef struct multiboot_info multiboot_info_t;

typedef struct position{
	uint32_t x_offset;
	uint32_t y_offset;
} t_position;

typedef struct {
    uint16_t magic;							/* Magic bytes for identification. */
    uint8_t fontMode;						/* PSF font mode. */
    uint8_t characterSize;					/* PSF character size. */
} __attribute__((packed)) PSF1_Header;

// Interrupt description Table
typedef struct {
	uint16_t    isr_low;					/* The lower 16 bits of the ISR's address */
	uint16_t    kernel_cs;					/* The GDT segment selector that the CPU will load into CS before calling the ISR */
	uint8_t     reserved;					/* Set to zero */
	uint8_t     attributes;					/* Type and attributes; see the IDT page */
	uint16_t    isr_high;					/* The higher 16 bits of the ISR's address */
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint32_t	*fb;
    uint32_t	pitch;
    uint32_t	width;
    uint32_t	height;
    uint8_t		*glyphs;
	uint32_t	char_size;
    uint32_t	cursor_x;
    uint32_t	cursor_y;
} display_ctx_t;

extern display_ctx_t g_display;

/****************************************/
/*				FUNCTION				*/
/****************************************/

void		idt_init();
void		PIC_remap(int offset1, int offset2);
void		IRQ_set_mask(uint8_t IRQline);
void		IRQ_clear_mask(uint8_t IRQline);
uint8_t		*init_font(uint32_t *out_charsize);
void		outb(uint16_t port, uint8_t val);
uint8_t		inb(uint16_t port);
void		PIC_sendEOI(uint8_t irq);
void		ft_putchar(uint8_t c, uint32_t color);


#endif

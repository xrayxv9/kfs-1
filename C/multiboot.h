#ifndef BOOT_H
#define BOOT_H

// EXTERN

#include <stdbool.h>

// Obtained By looking at the objdump -t of the font.o
extern char _binary_Lat15_Terminus16_psf_start[];
extern char _binary_Lat15_Terminus16_psf_end[];
extern char _binary_Lat15_Terminus16_psf_size[];

// MACRO

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

// DEFINE

#define PSF_FONT_MAGIC 0x0436
#define IDT_MAX_DESCRIPTORS 256

// TYPEDEF

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

// STRUCTS

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t pad[22];
    uint32_t framebuffer_addr_low;
    uint32_t framebuffer_addr_high;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed));

typedef struct position{
	uint32_t x_offset;
	uint32_t y_offset;
} t_position;

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} __attribute__((packed)) PSF1_Header;

// Interrupt description Table
typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;
#endif

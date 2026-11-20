#include "multiboot.h"

display_ctx_t g_display;

void kernel_main(uint32_t magic, struct multiboot_info *mbi) {
    uint8_t *glyphs;

    if (magic != 0x2BADB002) return;            /* not start with multiboot launcher */
    if (!CHECK_FLAG(mbi->flags, 12)) return;    /* no framebuffer */
    if (mbi->framebuffer_bpp != 32) return;     /* this code write only 32bits pixels */

    glyphs = init_font(&(g_display.char_size));
    if (glyphs == 0x0){
		return;
	}

	g_display.fb = (uint32_t *)(uintptr_t)mbi->framebuffer_addr;
    g_display.pitch = mbi->framebuffer_pitch;
    g_display.width = mbi->framebuffer_width;
    g_display.height = mbi->framebuffer_height;
    g_display.glyphs = glyphs;
    g_display.cursor_x = 0;
    g_display.cursor_y = 0;

	PIC_remap(0x20, 0x28);
	IRQ_clear_mask(1);
	idt_init();
	ft_putchar('4', WHITE);
	ft_putchar('2', WHITE);
}

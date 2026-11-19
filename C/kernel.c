#include "multiboot.h"

display_ctx_t g_display;

void kernel_main(uint32_t magic, struct multiboot_info *mbi) {
    uint8_t *glyphs;

    if (magic != 0x2BADB002) return;            /* not start with multiboot launcher */
    if (!CHECK_FLAG(mbi->flags, 12)) return;    /* no framebuffer */
    if (mbi->framebuffer_bpp != 32) return;     /* this code write only 32bits pixels */

    uint32_t *fb = (uint32_t *)(uintptr_t)mbi->framebuffer_addr;
    uint32_t pitch = mbi->framebuffer_pitch;


    /* remplit l'écran en bleu (si bpp == 32) */
    for (uint32_t y = 0; y < mbi->framebuffer_height; y++) {
        for (uint32_t x = 0; x < mbi->framebuffer_width; x++) {
			if ((y <= mbi->framebuffer_height / 2 && x <= mbi->framebuffer_width / 2 )||
					(y >= mbi->framebuffer_height / 2 && x >= mbi->framebuffer_width / 2 ))
            fb[y * (pitch / 4) + x] = 0x000000FF;
        }
    }

    glyphs = init_font();
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
}

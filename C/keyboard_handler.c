#include "multiboot.h"

void keyboard_handler(){
	(void)inb(0x60);
	for (uint32_t y = 0; y < g_display.height; y++) {
        for (uint32_t x = 0; x < g_display.width; x++) {
			if ((y <= g_display.height / 2 && x <= g_display.width / 2 )||
					(y >= g_display.height / 2 && x >= g_display.width / 2 ))
            g_display.fb[y * (g_display.pitch / 4) + x] = 0x00FF0000;
        }
    }
	PIC_sendEOI(1);

}

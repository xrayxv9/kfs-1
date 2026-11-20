#include "multiboot.h"

static const char keymap_normal[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0
};

static const char keymap_shift[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0
};

static bool lshift = false;
static bool rshift = false;
static bool capslock = false;
static bool extended = false;

static char scancode_to_char(uint8_t scancode) {
    if (scancode >= 128) {
        return 0;
    }

    bool shift = lshift || rshift;
    char c = shift ? keymap_shift[scancode] : keymap_normal[scancode];

    // Gestion simple de CapsLock sur les lettres
    if (capslock && c >= 'a' && c <= 'z') {
        c = (char)(c - 'a' + 'A');
    } else if (capslock && shift && c >= 'A' && c <= 'Z') {
        c = (char)(c - 'A' + 'a');
    }

    return c;
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
	uint32_t color = 0x00000000;

    if (scancode == 0xE0) {
        extended = true;
        PIC_sendEOI(1);
        return;
    }

    if (extended) {
        extended = false;
        // Ici scancode & 0x7F vaut par ex. 0x48 (Haut), 0x50 (Bas), 0x53 (Suppr)
        PIC_sendEOI(1);
        return;
    }

    if (scancode == 0x2A) {
        lshift = true;
    } else if (scancode == 0xAA) {
        lshift = false;
    } else if (scancode == 0x36) {
        rshift = true;
    } else if (scancode == 0xB6) {
        rshift = false;
    } else if (scancode == 0x3A) {
        capslock = !capslock;
    } else if (!(scancode & 0x80)) {
        char c = scancode_to_char(scancode);
        if (c != 0) {
			for (uint32_t y = 0; y < g_display.height; y++) {
				for (uint32_t x = 0; x < g_display.width; x++) {
					if ((y <= g_display.height / 2 && x <= g_display.width / 2 )||
							(y >= g_display.height / 2 && x >= g_display.width / 2 ))
					g_display.fb[y * (g_display.pitch / 4) + x] = (color | c) << 4;
					else{
						g_display.fb[y * (g_display.pitch / 4) + x] = (color | c) << 20;
					}

				}
			}
        }
    }
	

    PIC_sendEOI(1);
}

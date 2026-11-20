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

    if (capslock && c >= 'a' && c <= 'z') {
        c = (char)(c + ' ');
    } else if (capslock && shift && c >= 'A' && c <= 'Z') {
        c = (char)(c - ' ');
    }

    return c;
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    if (scancode == 0xE0) {
        extended = true;
        PIC_sendEOI(1);
        return;
    }

    if (extended) {
        extended = false;
        PIC_sendEOI(1);
        return;
    }

    if (scancode == LSHIFT_SCANCODE_PRESSED) {
        lshift = true;
    } else if (scancode == LSHIFT_SCANCODE_RELEASED) {
        lshift = false;
    } else if (scancode == RSHIFT_SCANCODE_PRESSED) {
        rshift = true;
    } else if (scancode == RSHIFT_SCANCODE_RELEASED) {
        rshift = false;
    } else if (scancode == CAPSLOCK_SCANCODE) {
        capslock = !capslock;
    } else if (!(scancode & 0x80)) {
        char c = scancode_to_char(scancode);
        if (c != 0) {
			ft_putchar(c, WHITE);
        }
    }
	

    PIC_sendEOI(1);
}

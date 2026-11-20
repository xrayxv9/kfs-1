#include "multiboot.h"

uint8_t	*init_font(uint32_t *out_charsize){
	// beginning of the font
	char *font_ptr = _binary_Lat15_Terminus16_psf_start;

	// header of the fonts, followed by the glyphs
	PSF1_Header *header = (PSF1_Header *) font_ptr;

	if (header->magic != PSF_FONT_MAGIC){
		// PSF invalid
		// add a print kernel
		return 0x0;
	}

	if (out_charsize){
		*out_charsize = header->characterSize;
	}

	// Getting the Glyphs
	uint8_t *glyphs = (uint8_t *)(font_ptr + sizeof(PSF1_Header));
	return glyphs;
}

void cls(uint32_t coo_x_begin, uint32_t coo_x_end, uint32_t coo_y_begin, uint32_t coo_y_end){
	for (uint32_t y = coo_y_begin; y < coo_y_end; y++){
		for (uint32_t x = coo_x_begin; x < coo_x_end; x++){
			g_display.fb[y * (g_display.pitch / 4) + x] = BLACK;
		}
	}
}

void ft_putchar(uint8_t c, uint32_t color){
	const uint8_t *glyph = g_display.glyphs + (uint32_t)c * g_display.char_size;
	uint32_t pitch_in_pixels = g_display.pitch / 4;

	if (c == '\n') {
        g_display.cursor_x = 0;
        g_display.cursor_y += g_display.char_size;
        return;
    }

    if (c == '\b') {
        if (g_display.cursor_x >= 8) {
            g_display.cursor_x -= 8;
			cls(g_display.cursor_x, g_display.cursor_x + 8, g_display.cursor_y, g_display.cursor_y + g_display.char_size);
        } else if (g_display.cursor_x == 0 && g_display.cursor_y >= g_display.char_size){
			g_display.cursor_y -= g_display.char_size;
		}
		return ;
    }
	if (c == '\t'){
		ft_putchar(' ', WHITE);
		ft_putchar(' ', WHITE);
		ft_putchar(' ', WHITE);
		ft_putchar(' ', WHITE);
		return ;
	}

	cls(g_display.cursor_x, g_display.cursor_x + 8, g_display.cursor_y, g_display.cursor_y + g_display.char_size);
	for (uint32_t row = 0; row < g_display.char_size; row++) {
        uint8_t line = glyph[row];
        uint32_t py = g_display.cursor_y + row;
        if (py >= g_display.height) break;

        for (uint32_t col = 0; col < 8; col++) {
            uint32_t px = g_display.cursor_x + col;
            if (px >= g_display.width) break;

            if (line & (1u << (7 - col))) {
                g_display.fb[py * pitch_in_pixels + px] = color;
            }
        }
    }

	g_display.cursor_x += 8;
	if (g_display.cursor_x + 8 > g_display.width) {
		g_display.cursor_x = 0;
		g_display.cursor_y += g_display.char_size;
	}
}

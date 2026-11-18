#include "boot.h"

uint8_t	*init_font(){
	// beginning of the font
	char *font_ptr = _binary_Lat15_Terminus16_psf_start;

	PSF1_Header *header = (PSF1_Header *) font_ptr;

	if (header->magic != PSF_FONT_MAGIC){
		// PSF invalid
		// add a print kernel
		return 0x0;
	}

	uint8_t *glyphs = (uint8_t *)(font_ptr + sizeof(PSF1_Header));
	return glyphs;
}

int main() {
	uint8_t		*glyphs;

	glyphs = init_font();
	if (glyphs == 0x0)
		return 1;
	return 0;
}

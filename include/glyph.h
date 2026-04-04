#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>


typedef struct {
	uint8_t c;
	uint8_t fg;
	uint8_t bg;
} Glyph;


#define glyph(_c, _fg, _bg) ((Glyph){ .c = (_c), .fg = (_fg), .bg = (_bg) })


#define Glyph_FMT_FG "\x1b[38;5;%um"
#define Glyph_FMT_BG "\x1b[48;5;%um"
#define Glyph_C_FMT "%c"


#endif // GLYPH_H

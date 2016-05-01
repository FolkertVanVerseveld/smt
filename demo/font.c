#include "font.h"
/*
4116
4  6
4226
5  7
5337
*/
void draw7seg(unsigned mask, GLfloat x, GLfloat y)
{
	if (mask & 1) {
		glVertex2f(x - 1, y - 1);
		glVertex2f(x + 1, y - 1);
	}
	if (mask & 2) {
		glVertex2f(x - 1, y);
		glVertex2f(x + 1, y);
	}
	if (mask & 4) {
		glVertex2f(x - 1, y + 1);
		glVertex2f(x + 1, y + 1);
	}
	if (mask & 8) {
		glVertex2f(x - 1, y - 1);
		glVertex2f(x - 1, y);
	}
	if (mask & 16) {
		glVertex2f(x - 1, y);
		glVertex2f(x - 1, y + 1);
	}
	if (mask & 32) {
		glVertex2f(x + 1, y - 1);
		glVertex2f(x + 1, y);
	}
	if (mask & 64) {
		glVertex2f(x + 1, y);
		glVertex2f(x + 1, y + 1);
	}
}

unsigned mask_dig[16] = {
	0x7d, 0x60, 0x37, 0x67, 0x6a, 0x4f, 0x5f, 0x61,
	0x7f, 0x6f, 0x7b, 0x5e, 0x1d, 0x76, 0x1f, 0x1b
};

unsigned mask_alpha[26] = {
	// a     b     c     d     e     f
	0x7b, 0x5e, 0x1d, 0x76, 0x1f, 0x1b,
	// g     h     i     j     k
	0x6f, 0x7a, 0x10, 0x44, 0x5a,
	// l     m     n     o     p
	0x1c, 0x79, 0x52, 0x56, 0x3b,
	// q     r     s     t     u
	0x6b, 0x7b, 0x4f, 0x61, 0x7c,
	// v     w     x     y     z
	0x54, 0x7c, 0x7a, 0x6a, 0x37
};

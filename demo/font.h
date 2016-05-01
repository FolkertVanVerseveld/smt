#ifndef SMT_DEMO_FONT_H
#define SMT_DEMO_FONT_H

#include <GL/gl.h>

void draw7seg(unsigned mask, GLfloat x, GLfloat y);

extern unsigned mask_dig[16];
extern unsigned mask_alpha[26];

#endif

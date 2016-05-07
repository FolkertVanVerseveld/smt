#include <smt/smt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RES_INVALID ((unsigned)-1)

unsigned win = RES_INVALID;
unsigned gl = RES_INVALID;

#define PALSZ 256
GLfloat palette[PALSZ][3];
GLuint tex_pal;

static void cleanup(void)
{
	if (gl != RES_INVALID) {
		smtFreegl(gl);
		gl = RES_INVALID;
	}
	if (win != RES_INVALID) {
		smtFreewin(win);
		win = RES_INVALID;
	}
}

static void init(void)
{
	unsigned i;
	for (i = 0; i < PALSZ; ++i) {
		float x = i, w = PALSZ, hw = w / 2;
		palette[i][0] = sin(M_PI * x / w);
		palette[i][1] = (hw + hw * sin(M_PI * x / hw)) / (hw * 3);
		palette[i][2] = 1.0f - sin(M_PI * x / w);
	}
	glGenTextures(1, &tex_pal);
	glBindTexture(GL_TEXTURE_1D, tex_pal);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, PALSZ, 0, GL_RGB, GL_FLOAT, palette);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

static void display(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_1D, tex_pal);
	glEnable(GL_TEXTURE_1D);
	glBegin(GL_QUADS);
	static GLfloat t_off = 0;
	unsigned i;
	const unsigned n = 6;
	for (i = 0; i < n; ++i) {
		GLfloat y0, y1, t0, t1;
		y0 = i * 1.0f / n;
		y1 = (i + 1) * 1.0f / n;
		t0 = t_off + i * 1.0f / (n - 1);
		t1 = t0 + 1;
		glTexCoord1f(t0); glVertex2f(0, y0);
		glTexCoord1f(t0); glVertex2f(1, y0);
		glTexCoord1f(t1); glVertex2f(1, y1);
		glTexCoord1f(t1); glVertex2f(0, y1);
	}
	t_off += 1.0f / 64;
	glEnd();
}

int main(int argc, char **argv)
{
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret) goto fail;
	atexit(cleanup);
	ret = smtCreatewin(&win, 800, 600, "pseudo plasma bars", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	if (ret) goto fail;
	ret = smtCreategl(&gl, win);
	if (ret) goto fail;
	init();
	while (1) {
		unsigned ev;
		GLenum glerr;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		display();
		if ((glerr = glGetError()) != GL_NO_ERROR)
			fprintf(stderr, "gl err: %u\n", glerr);
		smtSwapgl(win);
	}
end:
fail:
	return ret;
}

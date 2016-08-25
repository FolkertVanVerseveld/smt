#include <stdio.h>
#include <stdlib.h>
#include <smt/smt.h>

#define WIDTH 200
#define HEIGHT 300
#define TITLE "bare bones openal"

unsigned win, gl;

GLenum gl_err;
ALenum al_err;

static inline int glchk(void) {
	if ((gl_err = glGetError()) != GL_NO_ERROR)
		fprintf(stderr, "OpenGL error: %u\n", gl_err);
	return gl_err != GL_NO_ERROR;
}
static inline int alchk(void) {
	if ((al_err = alGetError()) != AL_NO_ERROR)
		fprintf(stderr, "OpenAL error: %u\n", al_err);
	return al_err != AL_NO_ERROR;
}

static void cleanup(void)
{
	if (gl != SMT_RES_INVALID) {
		smtFreegl(gl);
		gl = SMT_RES_INVALID;
	}
	if (win != SMT_RES_INVALID) {
		smtFreewin(win);
		win = SMT_RES_INVALID;
	}
}

int main(int argc, char **argv)
{
	int ret = 1;
	smtInit(&argc, argv);
	atexit(cleanup);
	if (smtCreatewin(&win, WIDTH, HEIGHT, TITLE, SMT_WIN_VISIBLE | SMT_WIN_BORDER))
		goto fail;
	if (smtCreategl(&gl, win))
		goto fail;
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		glchk();
		alchk();
		smtSwapgl(win);
	}
end:
	ret = 0;
fail:
	cleanup();
	return ret;
}

#include <smt/smt.h>

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	unsigned win, gl, spr;
	ret = smtCreatewin(&win, 400, 300, "logo", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	if (ret != 0) goto fail;
	ret = smtCreategl(&gl, win);
	if (ret != 0) goto fail;
	GLuint tex;
	glGenTextures(1, &tex);
	ret = smtCreatespr(&spr, 0, 0, "/home/methos/c/gut/demo/vim.png", tex, 0);
	if (ret != 0) goto fail;
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 1, 1, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(1, 0); glVertex2f(1, 0);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(0, 1); glVertex2f(0, 1);
		glEnd();
		smtSwapgl(win);
	}
end:
	glDeleteTextures(1, &tex);
	ret = smtFreespr(spr);
	if (ret != 0) goto fail;
	ret = smtFreegl(gl);
	if (ret != 0) goto fail;
	ret = smtFreewin(win);
fail:
	return ret;
}

/* print OpenGL attributes */
#include <smt/smt.h>
#include <stdio.h>

GLuint gl_major, gl_minor;

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	unsigned win, gl;
	ret = smtCreatewin(&win, 200, 200, "hello gl", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	if (ret != 0) goto fail;
	ret = smtCreategl(&gl, win);
	if (ret != 0) goto fail;
	smtGlAttrup(GL_MAJOR_VERSION, &gl_major);
	smtGlAttrup(GL_MINOR_VERSION, &gl_minor);
	printf("GL version: %u.%u\n", gl_major, gl_minor);
	while (1) {
		unsigned ev;
		GLuint gl_err;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		glClearColor(0, 1, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		if ((gl_err = glGetError()) != GL_NO_ERROR)
			printf("gl error: %u\n", gl_err);
		smtSwapgl(win);
	}
end:
	ret = smtFreegl(gl);
	if (ret != 0) goto fail;
	ret = smtFreewin(win);
fail:
	return ret;
}

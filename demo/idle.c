#include <smt/smt.h>

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	unsigned win, gl;
	ret = smtCreatewin(&win, 400, 300, "empty", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	if (ret != 0) goto fail;
	ret = smtCreategl(&gl, win);
	if (ret != 0) goto fail;
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		smtSwapgl(win);
	}
end:
	ret = smtFreegl(gl);
	if (ret != 0) goto fail;
	ret = smtFreewin(win);
fail:
	return ret;
}

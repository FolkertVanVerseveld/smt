/* flash like crazy and toggle between display modes */
#include <smt/smt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	unsigned win, gl, mode = 0, dis = (unsigned)-1;
	srand(time(NULL));
	smtInit(&argc, argv);
	smtCreatewin(&win, 100, 100, "small", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	while (1) {
		unsigned ev, disp;
		int x, y;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			case SMT_EV_KEY_UP:
				smtMode(win, mode ^= SMT_WIN_FULL_FAKE);
				smtGetPoswin(win, &x, &y);
				break;
			}
		}
		smtDisplaywin(win, &disp);
		if (dis != disp) {
			printf("enter display %u\n", disp);
			dis = disp;
		}
		if (dis & 1)
			glClearColor((rand() & 0xff) / 255.0f, 1, 0, 1);
		else
			glClearColor(1, (rand() & 0xff) / 255.0f, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		smtSwapgl(win);
	}
end:
	return 0;
}

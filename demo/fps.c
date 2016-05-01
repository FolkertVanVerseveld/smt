#include <smt/smt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font.h"

#define WIDTH 256
#define HEIGHT (WIDTH/2)

#define FPS_RAW_INTERVAL 100

smtTimer_t t_sec;
GLfloat fps_raw;
unsigned fps_dt = 0, fps_n = 0;

unsigned mask = 0;

void idle(void)
{
	unsigned n, dt;
	dt = smtTickt(&t_sec);
	n = smtSlicet(&t_sec, 1000);
	mask = (mask + n) & 0xf;
	if (dt < 1) dt = 1;
	fps_dt += dt;
	if (fps_dt > FPS_RAW_INTERVAL) {
		fps_raw = 1000.0f/dt;
		fps_n = 0;
		fps_dt %= FPS_RAW_INTERVAL;
	}
}

void drawfps(void)
{
	char buf[10], *ptr;
	size_t n;
	snprintf(buf, sizeof buf, "%.2f", fps_raw);
	n = strlen(buf);
	GLfloat x = -n;
	glColor3f(1, 1, 1);
	glScalef(16, 32, 1);
	glBegin(GL_LINES);
	for (ptr = buf; *ptr; ++ptr, x += 2.5) {
		int ch = *ptr;
		if (ch >= '0' && ch <= '9')
			draw7seg(mask_dig[ch - '0'], x, 0);
	}
	glEnd();
}

void display(void)
{
	++fps_n;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WIDTH / 2, WIDTH / 2, HEIGHT / 2, -HEIGHT / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	drawfps();
}

int main(int argc, char **argv)
{
	unsigned win, gl;
	smtInit(&argc, argv);
	smtCreatewin(&win, WIDTH, HEIGHT, "frame rate", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	smtResett(&t_sec);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		idle();
		display();
		smtSwapgl(win);
	}
end:
	return 0;
}

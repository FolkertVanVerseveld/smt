#include <stdio.h>
#include <math.h>
#include <smt/smt.h>

#define WIDTH 400
#define HEIGHT 300
#define TITLE "cursor tracker pointer"

float angle = 0.0f;
unsigned btn = SMT_MOUSE_MIDDLE;

static void motion(int mx, int my)
{
	double x, y;
	x = mx - WIDTH / 2.0;
	y = my - HEIGHT / 2.0;
	angle = atan2(-y, x) * 180.0 / M_PI;
}

static inline void display(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WIDTH / 2.0, WIDTH / 2.0, -HEIGHT / 2.0, HEIGHT / 2.0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(angle, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	switch (btn) {
	case SMT_MOUSE_LEFT:
		glColor3f(1, 0, 0);
		break;
	case SMT_MOUSE_MIDDLE:
		glColor3f(0, 1, 0);
		break;
	case SMT_MOUSE_RIGHT:
		glColor3f(0, 0, 1);
		break;
	case SMT_MOUSE_EXTRA0:
		glColor3f(1, 0, 1);
		break;
	case SMT_MOUSE_EXTRA1:
		glColor3f(.5f, .5f, 0);
		break;
	}
	glVertex2f(-40, 20);
	glVertex2f(40, 0);
	glVertex2f(-40, -20);
	glEnd();
}

int main(int argc, char **argv)
{
	unsigned win, gl;
	smtInit(&argc, argv);
	smtCreatewin(&win, WIDTH, HEIGHT, TITLE, SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			case SMT_EV_MOUSE_MOTION:
				motion(smt.mouse.x, smt.mouse.y);
				break;
			case SMT_EV_MOUSE_DOWN:
				btn = smt.mouse.down;
				break;
			}
		}
		display();
		smtSwapgl(win);
	}
end:
	return 0;
}

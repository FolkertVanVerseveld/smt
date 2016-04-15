#ifndef SMT_SMT_H
#define SMT_SMT_H

#include <stddef.h>
#include <GL/gl.h>

#define SMT_WIN_FULL_SLOW 1
#define SMT_WIN_FULL_FAST 2
#define SMT_WIN_VISIBLE 4
#define SMT_WIN_BORDER 8
#define SMT_WIN_RESIZE 16
#define SMT_WIN_MIN 32
#define SMT_WIN_MAX 64
#define SMT_WIN_GRAB 128
#define SMT_WIN_HDPI 256

#define SMT_ERR_OVERFLOW 1
#define SMT_ERR_STATE 2

#define SMT_EV_DONE 0
#define SMT_EV_QUIT 1
#define SMT_EV_KEY_DOWN 2
#define SMT_EV_KEY_UP 3
#define SMT_EV_MOUSE_MOTION 4
#define SMT_EV_DROP_FILE 5
#define SMT_EV_UNKNOWN 255

#define SMT_SPR_RESIZE 1
#define SMT_SPR_STRICT 2

#define SMT_MIN_TEXTURE_SIZE 2
#define SMT_MAX_TEXTURE_SIZE 2048

#define SMT_CURS_DEFAULT 0
#define SMT_CURS_ARROW 1
#define SMT_CURS_BEAM 2
#define SMT_CURS_WAIT 3
#define SMT_CURS_CROSSHAIR 4
#define SMT_CURS_WAITARROW 5
#define SMT_CURS_SIZENWSE 6
#define SMT_CURS_SIZENESW 7
#define SMT_CURS_SIZEWE 8
#define SMT_CURS_SIZENS 9
#define SMT_CURS_SIZEALL 10
#define SMT_CURS_NO 11
#define SMT_CURS_HAND 12
#define SMT_CURS_MAX 13

#define SMT_CURS_SHOW 0
#define SMT_CURS_HIDE 1
#define SMT_CURS_TOGGLE 2

#define SMT_KEY_RIGHT 0x1000
#define SMT_KEY_UP 0x1001
#define SMT_KEY_LEFT 0x1002
#define SMT_KEY_DOWN 0x1003

#define SMT_MOD_LSHIFT 1
#define SMT_MOD_RSHIFT 2
#define SMT_MOD_SHIFT (SMT_MOD_LSHIFT|SMT_MOD_RSHIFT)
#define SMT_MOD_LALT 4
#define SMT_MOD_RALT 8
#define SMT_MOD_ALT (SMT_MOD_LALT|SMT_MOD_RALT)
#define SMT_MOD_LCTRL 16
#define SMT_MOD_RCTRL 32
#define SMT_MOD_CTRL (SMT_MOD_LCTRL|SMT_MOD_RCTRL)

#define SMT_KEY_F1 0x1004
#define SMT_KEY_F(x) (SMT_KEY_F1+(x)-1)

#define SMT_IMG_PNG 1
#define SMT_IMG_JPG 2
#define SMT_IMG_ALL 3

struct smtconf {
	struct {
		unsigned mod;
		unsigned scan, virt;
	} kbp;
	struct {
		unsigned x, y;
		unsigned state;
	} mouse;
	char *drop;
};

extern struct smtconf smt;

unsigned smtOptimg(unsigned options);
int smtInit(int *argc, char **argv);
/** create gl context and make current */
int smtCreategl(unsigned *gl, unsigned win);
/** create window with specified arguments */
int smtCreatewin(unsigned *win, unsigned w, unsigned h, const char *title, unsigned flags);
int smtCreatespr(unsigned *spr, unsigned w, unsigned h, const char *name, GLuint tex, unsigned flags);
/** attach to window and make current */
int smtSetgl(unsigned win, unsigned gl);
/** swap buffers with window */
int smtSwapgl(unsigned win);
int smtFreegl(unsigned index);
int smtFreewin(unsigned index);
int smtFreespr(unsigned spr);
/* getters */
int smtGetsizespr(unsigned spr, unsigned *pw, unsigned *ph, unsigned *vw, unsigned *vh);
int smtGetsizewin(unsigned win, unsigned *w, unsigned *h);
int smtGetminwin(unsigned win, unsigned *w, unsigned *h);
int smtGetmaxwin(unsigned win, unsigned *w, unsigned *h);
char *smtGetclip(void);
/* setters */
int smtSetsizewin(unsigned win, unsigned w, unsigned h);
int smtSetminwin(unsigned win, unsigned w, unsigned h);
int smtSetmaxwin(unsigned win, unsigned w, unsigned h);
int smtSetcurs(unsigned cur, unsigned state);
int smtSetclip(const char *str);

unsigned smtPollev(void);
unsigned smtQwerty(void);
void smtExit(int status) __attribute__ ((noreturn));

#endif

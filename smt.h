#ifndef SMT_SMT_H
#define SMT_SMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <AL/al.h>
#include <GL/gl.h>

#define SMT_RES_INVALID ((unsigned)-1)

#define SMT_WIN_DESKTOP 0
#define SMT_WIN_FULL_SLOW 1
#define SMT_WIN_FULL_FAST 2
#define SMT_WIN_FULL_FAKE 4
#define SMT_WIN_FULL_MASK 7
#define SMT_WIN_VISIBLE 8
#define SMT_WIN_BORDER 16
#define SMT_WIN_RESIZE 32
#define SMT_WIN_MIN 64
#define SMT_WIN_MAX 128
#define SMT_WIN_GRAB 256
#define SMT_WIN_HDPI 512

#define SMT_ERR_OVERFLOW 1
#define SMT_ERR_STATE 2
#define SMT_ERR_DOMAIN 3
#define SMT_ERR_NOMEM 4

#define SMT_EV_DONE 0
#define SMT_EV_QUIT 1
#define SMT_EV_KEY_DOWN 2
#define SMT_EV_KEY_UP 3
#define SMT_EV_MOUSE_MOTION 4
#define SMT_EV_MOUSE_DOWN 5
#define SMT_EV_MOUSE_UP 6
#define SMT_EV_MOUSE_SCROLL 7
#define SMT_EV_DROP_FILE 8
#define SMT_EV_UNKNOWN 255

#define SMT_SPR_RESIZE 1
#define SMT_SPR_STRICT 2

#define SMT_MIN_TEXTURE_SIZE 2
#define SMT_MAX_TEXTURE_SIZE 2048

#define SMT_KEY_RIGHT 0x1000
#define SMT_KEY_UP 0x1001
#define SMT_KEY_LEFT 0x1002
#define SMT_KEY_DOWN 0x1003

#define SMT_MOUSE_LEFT 1
#define SMT_MOUSE_MIDDLE 2
#define SMT_MOUSE_RIGHT 3
#define SMT_MOUSE_EXTRA0 8
#define SMT_MOUSE_EXTRA1 9

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

#define SMT_SFX_ALUT 1
#define SMT_SFX_AL_CTX 2
#define SMT_SFX_ALL 3

struct smtconf {
	struct {
		unsigned mod;
		unsigned scan, virt;
	} kbp;
	struct {
		int x, y;
		unsigned state, win;
		unsigned down, up;
		int sx, sy;
	} mouse;
	char *drop;
};

extern struct smtconf smt;

typedef struct smtTimer {
	unsigned elapsed, last, next, delay;
} smtTimer_t;

/* timing */
void smtResett(struct smtTimer *t);
unsigned smtTickt(struct smtTimer *t);
unsigned smtSlicet(struct smtTimer *t, unsigned delay);
unsigned smtSleep(unsigned ms);
unsigned smtTicks(void);

#define SMT_CTX_GL_OLD 1
#define SMT_CTX_GL_NEW 2

/* startup/shutdown */
int smtInit(int *argc, char **argv);
void smtExit(int status) __attribute__ ((noreturn));

#define SMT_SCREEN_SAVE_OFF 0
#define SMT_SCREEN_SAVE_ON 1
#define SMT_SCREEN_SAVE_TOGGLE 2
#define SMT_SCREEN_SAVE_QUERY 3

/** apply screen save state and return new state */
unsigned smtScreensave(unsigned state);

/* OpenGL */
int smtGlAttru(GLuint opt, GLuint arg);
int smtGlAttrup(GLuint opt, GLuint *val);
int smtGlSoftd(GLint val);
int smtGlSoftdp(GLint *val);
/* smt configuration routines */
unsigned smtOptimg(unsigned options);
unsigned smtOptsfx(unsigned options);
/** create gl context and make current */
int smtCreategl(unsigned *gl, unsigned win);
/** create window with specified arguments */
int smtCreatewin(unsigned *win, unsigned w, unsigned h, const char *title, unsigned flags);
/**
\brief Load sprite from specified path

At least one window and one gl context must be created before you can load images.
*/
int smtCreatespr(unsigned *spr, unsigned w, unsigned h, const char *name, GLuint tex, unsigned flags);
/** attach to window and make current */
int smtSetgl(unsigned win, unsigned gl);
/** swap buffers with window */
int smtSwapgl(unsigned win);
int smtFreegl(unsigned index);
int smtFreewin(unsigned index);
int smtFreespr(unsigned spr);
/* getters */
int smtGetSizespr(unsigned spr, unsigned *pw, unsigned *ph, unsigned *vw, unsigned *vh);
int smtGetSizewin(unsigned win, unsigned *w, unsigned *h);
int smtGetMinwin(unsigned win, unsigned *w, unsigned *h);
int smtGetMaxwin(unsigned win, unsigned *w, unsigned *h);
int smtGetPoswin(unsigned win, int *x, int *y);
char *smtClip(void);

/* window boundary routines */
int smtSizewin(unsigned win, unsigned w, unsigned h);
int smtMinwin(unsigned win, unsigned w, unsigned h);
int smtMaxwin(unsigned win, unsigned w, unsigned h);

/* cursor type */
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
/* cursor state */
#define SMT_CURS_SHOW 0
#define SMT_CURS_HIDE 1
#define SMT_CURS_TOGGLE 2

int smtCursor(unsigned type, unsigned state);
int smtClips(const char *str);
/** switch between windowed(=desktop) and fullscreen modes */
int smtMode(unsigned win, unsigned mode);
int smtTitle(unsigned win, const char *title);
int smtFocus(unsigned win);
int smtVisible(unsigned win, int show);
int smtBorder(unsigned win, int show);
int smtGrab(unsigned win, int show);
int smtState(unsigned win, unsigned flags);
int smtGamma(unsigned win, float bright);

/* display routines */

unsigned smtDisplayCount(void);
int smtDisplayBounds(unsigned index, int *x, int *y, unsigned *w, unsigned *h);
/** determine on which display the window is located */
int smtDisplaywin(unsigned win, unsigned *display);

/* messages */
#define SMT_MSG_ERR 0
#define SMT_MSG_WARN 1
#define SMT_MSG_INFO 2
int smtMsg(int type, unsigned win, const char *title, const char *message);

/* event handlers/wrappers */
unsigned smtPollev(void);
unsigned smtQwerty(void);

/* audio routines */
int smtOggfv(const char *name, char **buf, size_t *count, ALenum *format, ALsizei *freq);

#ifdef __cplusplus
}
#endif

#endif

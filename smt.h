#ifndef SMT_SMT_H
#define SMT_SMT_H

#include <stddef.h>
#include <stdbool.h>
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
#define SMT_EV_UNKNOWN 255

#define SMT_SPR_RESIZE 1
#define SMT_SPR_STRICT 2

#define SMT_MIN_TEXTURE_SIZE 2
#define SMT_MAX_TEXTURE_SIZE 2048

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
unsigned smtPollev(void);
void smtExit(int status) __attribute__ ((noreturn));

#endif

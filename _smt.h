#ifndef SMT__SMT_H
#define SMT__SMT_H

#include <SDL2/SDL.h>
#include "smt.h"

#define ERR_SMT 1
#define ERR_SDL 2
#define ERR_IMG 4

#define INIT_SMT 1
#define INIT_SDL 2
#define INIT_VFX 4
#define INIT_IMG 8
#define INIT_IMG_PNG 16

#define SMT_GL_INIT 1
#define SMT_GL_DOUBLE 2

/* window states */
#define SMT_WIN_INIT 1

#define SMT_SPR_INIT 1

#define ERRSZ 16
#define WINSZ 32
#define GLSZ 8
#define SPRSZ 128

struct _smtconf {
	struct {
		unsigned type[ERRSZ], code[ERRSZ];
		unsigned index, count, rcount;
		unsigned genf;
	} err;
	struct {
		unsigned sdl;
		unsigned gl;
		unsigned libs;
	} init;
	struct {
		SDL_GLContext data[GLSZ];
		unsigned state[GLSZ];
		unsigned rpop[GLSZ];
		unsigned n, ri, cur;
	} gl;
	struct {
		unsigned flags[WINSZ];
		unsigned w[WINSZ], h[WINSZ];
		unsigned state[WINSZ];
		SDL_Window *scr[WINSZ];
		unsigned rpop[WINSZ];
		unsigned n, ri;
	} win;
	struct {
		GLuint tex[SPRSZ];
		unsigned w[SPRSZ], h[SPRSZ];
		unsigned tw[SPRSZ], th[SPRSZ];
		unsigned state[SPRSZ];
		unsigned rpop[SPRSZ];
		unsigned n, ri;
	} spr;
	SDL_Event ev;
};

#define error(mask) do{\
	_smt.err.genf|=mask;\
	++_smt.err.count;\
	_smt_pge();\
	goto fail;\
	}while(0)

#define sdl_error() error(ERR_SDL)
#define img_error() error(ERR_IMG)

extern struct _smtconf _smt;

/** push back error on stack. not thread-safe */
void _smt_err(unsigned type, unsigned code);
unsigned _smt_perr(void);
void _smt_pge(void);
void _smt_error(unsigned mask);

#endif

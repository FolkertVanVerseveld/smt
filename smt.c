#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "_smt.h"
#include "log.h"

struct _smtconf _smt;

inline void _smt_error(unsigned mask)
{
	_smt.err.genf |= mask;
	++_smt.err.count;
	_smt_pge();
}

static void _smt_stop(void)
{
	/* make sure all acquired resources are freed */
	unsigned i;
	smtDbgf(
		"gl: %u, win: %u, spr: %u\n",
		_smt.gl.n - _smt.gl.ri,
		_smt.win.n - _smt.win.ri,
		_smt.spr.n - _smt.spr.ri
	);
	for (i = 0; i < _smt.spr.n; ++i) {
		if (_smt.spr.state[i] & SMT_SPR_INIT)
			smtFreespr(i);
	}
	for (i = 0; i < _smt.gl.n; ++i) {
		if (_smt.gl.state[i] & SMT_GL_INIT)
			smtFreegl(i);
	}
	for (i = 0; i < _smt.win.n; ++i) {
		if (_smt.win.state[i] & SMT_GL_INIT)
			smtFreewin(i);
	}
	if (_smt.init.libs & INIT_IMG)
		IMG_Quit();
	if (_smt.init.libs & INIT_SDL)
		SDL_Quit();
}

/** push back error on stack. not thread-safe */
void _smt_err(unsigned type, unsigned code)
{
	unsigned index = _smt.err.index;
	_smt.err.type[index] = type;
	_smt.err.code[index] = code;
	_smt.err.index = (_smt.err.index + 1) % ERRSZ;
	++_smt.err.count;
}

unsigned _smt_perr(void)
{
	unsigned type, code, index;
	index = (_smt.err.index + ERRSZ - 1) % ERRSZ;
	type = _smt.err.type[index];
	code = _smt.err.code[index];
	fprintf(stderr, "smt: type=%u,code=%u\n", type, code);
	_smt.err.index = index;
	return _smt.err.count - ++_smt.err.rcount;
}

void _smt_pge(void)
{
	unsigned genf;
	genf = _smt.err.genf;
	if (genf & ERR_SDL) {
		++_smt.err.rcount;
		fprintf(stderr, "smt: sdl: %s\n", SDL_GetError());
	}
	if (genf & ERR_IMG) {
		++_smt.err.rcount;
		fprintf(stderr, "smt: img: %s\n", IMG_GetError());
	}
}

int smtSetgl(unsigned win, unsigned gl)
{
	if (win >= WINSZ || gl >= GLSZ)
		return SMT_ERR_OVERFLOW;
	if (!(_smt.win.state[win] & SMT_WIN_INIT) ||
		!(_smt.gl.state[gl] & SMT_GL_INIT))
	{
		return SMT_ERR_STATE;
	}
	if (SDL_GL_MakeCurrent(_smt.win.scr[win], _smt.gl.data[gl]) != 0)
		return SMT_ERR_STATE;
	_smt.gl.cur = gl;
	return 0;
}

int smtCreategl(unsigned *gl, unsigned win)
{
	if (_smt.gl.n >= GLSZ || win >= WINSZ)
		return SMT_ERR_OVERFLOW;
	if (!(_smt.win.state[win] & SMT_WIN_INIT))
		return SMT_ERR_STATE;
	SDL_GLContext ctx = SDL_GL_CreateContext(_smt.win.scr[win]);
	if (!gl) sdl_error();
	unsigned i;
	if (_smt.gl.ri)
		i = _smt.gl.rpop[--_smt.gl.ri];
	else
		i = _smt.gl.n++;
	_smt.gl.data[i] = ctx;
	_smt.gl.state[i] = SMT_GL_INIT;
	_smt.gl.cur = i;
	*gl = i;
	return 0;
fail:
	return SMT_ERR_STATE;
}

unsigned _smt_flags2sdl(unsigned flags)
{
	unsigned sdl = SDL_WINDOW_OPENGL;
	if (flags & SMT_WIN_FULL_SLOW)
		sdl |= SDL_WINDOW_FULLSCREEN;
	if (flags & SMT_WIN_FULL_FAST)
		sdl |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if (!(flags & SMT_WIN_VISIBLE))
		sdl |= SDL_WINDOW_HIDDEN;
	if (!(flags & SMT_WIN_BORDER))
		sdl |= SDL_WINDOW_BORDERLESS;
	if (flags & SMT_WIN_RESIZE)
		sdl |= SDL_WINDOW_RESIZABLE;
	if (flags & SMT_WIN_MIN)
		sdl |= SDL_WINDOW_MINIMIZED;
	if (flags & SMT_WIN_MAX)
		sdl |= SDL_WINDOW_MAXIMIZED;
	if (flags & SMT_WIN_GRAB)
		sdl |= SDL_WINDOW_INPUT_GRABBED;
	if (flags & SMT_WIN_HDPI)
		sdl |= SDL_WINDOW_ALLOW_HIGHDPI;
	return sdl;
}

int smtCreatewin(unsigned *win, unsigned w, unsigned h, const char *title, unsigned flags)
{
	if (_smt.win.n >= WINSZ)
		return SMT_ERR_OVERFLOW;
	unsigned sdl = _smt_flags2sdl(flags);
	SDL_Window *scr = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		w, h, sdl
	);
	if (!scr) sdl_error();
	unsigned i;
	if (_smt.win.ri)
		i = _smt.win.rpop[--_smt.win.ri];
	else
		i = _smt.win.n++;
	_smt.win.scr[i] = scr;
	_smt.win.w[i] = w;
	_smt.win.h[i] = h;
	_smt.win.flags[i] = flags;
	_smt.win.state[i] = SMT_WIN_INIT;
	*win = i;
	return 0;
fail:
	return 1;
}

#define glchk(i) do{\
	if (i >= GLSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.gl.state[i] & SMT_GL_INIT))\
		return SMT_ERR_STATE;\
	}while(0)
#define winchk(i) do{\
	if (i >= WINSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.win.state[i] & SMT_WIN_INIT))\
		return SMT_ERR_STATE;\
	}while(0)
#define sprchk(i) do{\
	if (i >= SPRSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.spr.state[i] & SMT_SPR_INIT))\
		return SMT_ERR_STATE;\
	}while(0)

int smtFreegl(unsigned i)
{
	glchk(i);
	_smt.gl.rpop[_smt.gl.ri++] = i;
	_smt.gl.state[i] = 0;
	if (i == _smt.gl.cur)
		_smt.gl.cur = GLSZ;
	SDL_GL_DeleteContext(_smt.gl.data[i]);
	_smt.gl.data[i] = NULL;
	return 0;
}

int smtFreespr(unsigned i)
{
	sprchk(i);
	_smt.spr.rpop[_smt.spr.ri++] = i;
	_smt.spr.state[i] = 0;
	return 0;
}

int smtSwapgl(unsigned win)
{
	winchk(win);
	SDL_GL_SwapWindow(_smt.win.scr[win]);
	return 0;
}

unsigned smtPollev(void)
{
	if (SDL_PollEvent(&_smt.ev) == 0)
		return SMT_EV_DONE;
	switch (_smt.ev.type) {
	case SDL_QUIT: return SMT_EV_QUIT;
	default: return SMT_EV_UNKNOWN;
	}
}

int smtFreewin(unsigned i)
{
	if (i >= WINSZ)
		return SMT_ERR_OVERFLOW;
	if (!(_smt.win.state[i] & SMT_WIN_INIT))
		return SMT_ERR_STATE;
	_smt.win.rpop[_smt.win.ri++] = i;
	_smt.win.state[i] = 0;
	SDL_DestroyWindow(_smt.win.scr[i]);
	_smt.win.scr[i] = NULL;
	return 0;
}

int smtInit(int *argc, char **argv)
{
	atexit(_smt_stop);
	_smt.init.libs |= INIT_SMT;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		sdl_error();
	_smt.init.libs |= INIT_SDL;
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)
		sdl_error();
	_smt.init.gl |= SMT_GL_DOUBLE;
	int imgmask = IMG_INIT_PNG | IMG_INIT_JPG;
	if ((IMG_Init(imgmask) & imgmask) != imgmask)
		img_error();
	_smt.init.libs |= INIT_IMG;
	return 0;
fail:
	fputs("smt: failed to init\n", stderr);
	return 1;
}

void smtExit(int status)
{
	if (_smt.err.count != _smt.err.rcount)
		fprintf(stderr, "smt: pending errors: %d\n", (int)_smt.err.count - _smt.err.rcount);
	exit(status);
}

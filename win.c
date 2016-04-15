#include "_smt.h"

#define winchk(i) do{\
	if (i >= WINSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.win.state[i] & SMT_WIN_INIT))\
		return SMT_ERR_STATE;\
	}while(0)

int smtSwapgl(unsigned win)
{
	winchk(win);
	SDL_GL_SwapWindow(_smt.win.scr[win]);
	return 0;
}

int smtGetsizewin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	int w, h;
	SDL_GetWindowSize(_smt.win.scr[win], &w, &h);
	if (w < 0 || h < 0)
		return SMT_ERR_STATE;
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	if (width) *width = w;
	if (height) *height = h;
	return 0;
}

int smtSetsizewin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowSize(_smt.win.scr[win], w, h);
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	return 0;
}

int smtGetminwin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	int w, h;
	SDL_GetWindowMinimumSize(_smt.win.scr[win], &w, &h);
	if (w < 0 || h < 0)
		return SMT_ERR_STATE;
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	if (width) *width = w;
	if (height) *height = h;
	return 0;
}

int smtSetminwin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowMinimumSize(_smt.win.scr[win], w, h);
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	return 0;
}

int smtGetmaxwin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	int w, h;
	SDL_GetWindowMaximumSize(_smt.win.scr[win], &w, &h);
	if (w < 0 || h < 0)
		return SMT_ERR_STATE;
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	if (width) *width = w;
	if (height) *height = h;
	return 0;
}

int smtSetmaxwin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowMaximumSize(_smt.win.scr[win], w, h);
	_smt.win.w[win] = w;
	_smt.win.h[win] = h;
	return 0;
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

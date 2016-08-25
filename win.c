#include "_smt.h"
#include <SDL2/SDL_image.h>

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

unsigned _smt_diswin(unsigned win)
{
	int i = SDL_GetWindowDisplayIndex(_smt.win.scr[win]);
	return (unsigned)(i < 0 ? 0 : i);
}

int smtDisplaywin(unsigned win, unsigned *display)
{
	winchk(win);
	unsigned i, n;
	int x, y;
	unsigned w, h;
	SDL_Rect bounds;
	if (smtGetPoswin(win, &x, &y) || smtGetSizewin(win, &w, &h))
		return SMT_ERR_STATE;
	x += w / 2;
	for (i = 0, n = smtDisplayCount(); i < n; ++i) {
		SDL_GetDisplayBounds(i, &bounds);
		if (x >= bounds.x && y >= bounds.y && x < bounds.x + bounds.w && y < bounds.y + bounds.h) {
			_smt.win.index[win] = i;
			if (display) *display = i;
			return 0;
		}
	}
	return SMT_ERR_STATE;
}

int smtIconf(unsigned win, const char *path)
{
	winchk(win);
	SDL_Surface *icon = IMG_Load(path);
	if (!icon) return SMT_ERR_STATE;
	SDL_SetWindowIcon(_smt.win.scr[win], icon);
	SDL_FreeSurface(icon);
	return 0;
}

static inline int _smt_getbnds(unsigned win, SDL_Rect *bounds)
{
	unsigned display;
	if (smtDisplaywin(win, &display) || SDL_GetDisplayBounds(display, bounds))
		return SMT_ERR_STATE;
	return 0;
}

static inline int _smt_win_boundsp(unsigned win, int *x, int *y, unsigned *w, unsigned *h)
{
	int wx, wy, xp, yp;
	int ww, wh, wp, hp;
	SDL_Window *scr = _smt.win.scr[win];
	if (w || h) {
		ww = wp = _smt.win.physic[win].w;
		wh = hp = _smt.win.physic[win].h;
		SDL_GetWindowSize(scr, &wp, &hp);
		if (wp < 0 || hp < 0)
			return SMT_ERR_OVERFLOW;
		if (w) *w = wp;
		if (h) *h = hp;
		if (ww != wp || wh != hp) {
			_smt.win.physic[win].w = wp;
			_smt.win.physic[win].h = hp;
		}
	}
	if (x || y) {
		wx = _smt.win.physic[win].x;
		wy = _smt.win.physic[win].y;
		SDL_GetWindowPosition(scr, &xp, &yp);
		if (x) *x = xp;
		if (y) *y = yp;
		if (wx != xp || wy != yp) {
			_smt.win.physic[win].x = xp;
			_smt.win.physic[win].y = yp;
		}
	}
	return 0;
}

int smtGetPoswin(unsigned win, int *x, int *y)
{
	winchk(win);
	return _smt_win_boundsp(win, x, y, NULL, NULL);
}

int smtGetSizewin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	return _smt_win_boundsp(win, NULL, NULL, width, height);
}

int smtSizewin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowSize(_smt.win.scr[win], w, h);
	_smt.win.physic[win].w = w;
	_smt.win.physic[win].h = h;
	return 0;
}

int smtGetMinwin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	int w, h;
	SDL_GetWindowMinimumSize(_smt.win.scr[win], &w, &h);
	if (w < 0 || h < 0)
		return SMT_ERR_STATE;
	if (width) *width = w;
	if (height) *height = h;
	return 0;
}

int smtMinwin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowMinimumSize(_smt.win.scr[win], w, h);
	unsigned ww, hh;
	ww = _smt.win.physic[win].w;
	hh = _smt.win.physic[win].h;
	if (ww < w) _smt.win.physic[win].w = w;
	if (hh < h) _smt.win.physic[win].h = h;
	return 0;
}

int smtGetMaxwin(unsigned win, unsigned *width, unsigned *height)
{
	winchk(win);
	int w, h;
	SDL_GetWindowMaximumSize(_smt.win.scr[win], &w, &h);
	if (w < 0 || h < 0)
		return SMT_ERR_STATE;
	if (width) *width = w;
	if (height) *height = h;
	return 0;
}

int smtMaxwin(unsigned win, unsigned w, unsigned h)
{
	winchk(win);
	if (w < 1 || h < 1)
		return SMT_ERR_STATE;
	SDL_SetWindowMaximumSize(_smt.win.scr[win], w, h);
	unsigned ww, hh;
	ww = _smt.win.physic[win].w;
	hh = _smt.win.physic[win].h;
	if (ww > w) _smt.win.physic[win].w = w;
	if (hh > h) _smt.win.physic[win].h = h;
	return 0;
}

int smtTitle(unsigned win, const char *title)
{
	winchk(win);
	SDL_SetWindowTitle(_smt.win.scr[win], title);
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

int smtMode(unsigned win, unsigned mode)
{
	winchk(win);
	int x, y;
	unsigned flags, modeo, w, h;
	SDL_Window *scro;
	scro = _smt.win.scr[win];
	flags = _smt.win.flags[win];
	modeo = flags & SMT_WIN_FULL_MASK;
	flags &= ~SMT_WIN_FULL_MASK;
	if (mode == modeo) return 0;
	w = _smt.win.physic[win].w;
	h = _smt.win.physic[win].h;
	switch (mode) {
	case SMT_WIN_DESKTOP:
		smtDbgf("win: change %u to windowed\n", win);
		if (modeo == SMT_WIN_FULL_FAKE) {
			x = _smt.win.desk[win].x;
			y = _smt.win.desk[win].y;
			w = _smt.win.desk[win].w;
			h = _smt.win.desk[win].h;
			if (flags & SMT_WIN_BORDER)
				SDL_SetWindowBordered(scro, SDL_TRUE);
			SDL_SetWindowSize(scro, w, h);
			SDL_SetWindowPosition(scro, x, y);
			_smt.win.physic[win] = _smt.win.desk[win];
		}
		break;
	case SMT_WIN_FULL_FAKE: {
		SDL_Rect bounds;
		if (modeo == SMT_WIN_DESKTOP) {
			SDL_GetWindowPosition(scro, &x, &y);
			_smt.win.physic[win].x = x;
			_smt.win.physic[win].y = y;
			_smt.win.desk[win] = _smt.win.physic[win];
		}
		smtDbgf("win: change %u to fake\n", win);
		if (_smt_getbnds(win, &bounds))
			return SMT_ERR_STATE;
		SDL_SetWindowBordered(scro, SDL_FALSE);
		SDL_SetWindowPosition(scro, bounds.x, bounds.y);
		SDL_SetWindowSize(scro, bounds.w, bounds.h);
		_smt.win.physic[win] = bounds;
		break;
	default:
		fprintf(stderr, "smt: unimplemented mode %u\n", mode);
		goto fail;
	}
	}
	_smt.win.flags[win] = flags | mode;
	return 0;
fail:
	return SMT_ERR_STATE;
}

int smtFocus(unsigned win)
{
	winchk(win);
	SDL_RaiseWindow(_smt.win.scr[win]);
	return 0;
}

int smtVisible(unsigned win, int show)
{
	winchk(win);
	SDL_Window *scr = _smt.win.scr[win];
	if (show)
		SDL_ShowWindow(scr);
	else
		SDL_HideWindow(scr);
	return 0;
}

int smtBorder(unsigned win, int show)
{
	winchk(win);
	SDL_bool val = show ? SDL_TRUE : SDL_FALSE;
	SDL_SetWindowBordered(_smt.win.scr[win], val);
	return 0;
}

int smtGrab(unsigned win, int show)
{
	winchk(win);
	SDL_bool val = show ? SDL_TRUE : SDL_FALSE;
	SDL_SetWindowGrab(_smt.win.scr[win], val);
	return 0;
}

int smtGamma(unsigned win, float bright)
{
	winchk(win);
	if (SDL_SetWindowBrightness(_smt.win.scr[win], bright))
		sdl_error();
	return 0;
fail:
	return SMT_ERR_STATE;
}

int smtState(unsigned win, unsigned flags)
{
	winchk(win);
	SDL_bool val;
	SDL_Window *scr = _smt.win.scr[win];
	val = flags & SMT_WIN_GRAB ? SDL_TRUE : SDL_FALSE;
	SDL_SetWindowGrab(scr, val);
	val = flags & SMT_WIN_BORDER ? SDL_TRUE : SDL_FALSE;
	SDL_SetWindowBordered(scr, val);
	if (flags & SMT_WIN_VISIBLE)
		SDL_ShowWindow(scr);
	else
		SDL_HideWindow(scr);
	return 0;
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
	_smt.win.physic[i].w = w;
	_smt.win.physic[i].h = h;
	if (!(flags & SMT_WIN_FULL_MASK)) {
		_smt.win.desk[i].w = w;
		_smt.win.desk[i].h = h;
	}
	_smt.win.flags[i] = flags;
	_smt.win.state[i] = SMT_WIN_INIT;
	smtDisplaywin(i, NULL);
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

unsigned smtDisplayCount(void)
{
	int n = SDL_GetNumVideoDisplays();
	return (unsigned)(n < 0 ? 0 : n);
}

unsigned smtModeCount(unsigned i)
{
	if (i >= smtDisplayCount())
		return 0;
	int n = SDL_GetNumDisplayModes(i);
	return (unsigned)(n < 0 ? 0 : n);
}

int smtModeBounds(unsigned scr, unsigned i, struct smtMode *mode)
{
	if (i >= smtModeCount(scr))
		return SMT_ERR_OVERFLOW;
	// make sure invalid/unmodified values are spotted
	SDL_DisplayMode dmode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
	if (SDL_GetDisplayMode(scr, i, &dmode))
		return SMT_ERR_STATE;
	mode->format = dmode.format;
	mode->bitdepth = SDL_BITSPERPIXEL(dmode.format);
	mode->width = dmode.w;
	mode->height = dmode.h;
	mode->frequency = dmode.refresh_rate;
	return 0;
}

int smtDisplayBounds(unsigned i, int *x, int *y, unsigned *w, unsigned *h)
{
	unsigned n = smtDisplayCount();
	if (i >= n) return SMT_ERR_OVERFLOW;
	SDL_Rect bounds;
	if (SDL_GetDisplayBounds(i, &bounds))
		return SMT_ERR_STATE;
	if (x) *x = bounds.x;
	if (y) *y = bounds.y;
	if (w) *w = bounds.w;
	if (h) *h = bounds.h;
	return 0;
}

int smtMsg(int type, unsigned win, const char *title, const char *message)
{
	Uint32 flags;
	switch (type) {
	case SMT_MSG_WARN: flags = SDL_MESSAGEBOX_WARNING; break;
	case SMT_MSG_INFO: flags = SDL_MESSAGEBOX_INFORMATION; break;
	default: flags = SDL_MESSAGEBOX_ERROR;
	}
	SDL_Window *scr = NULL;
	if (win < WINSZ && (_smt.win.state[win] & SMT_WIN_INIT))
		scr = _smt.win.scr[win];
	if (SDL_ShowSimpleMessageBox(flags, title, message, scr))
		return SMT_ERR_STATE;
	return 0;
}

unsigned smtScreensave(unsigned state)
{
	switch (state) {
	case SMT_SCREEN_SAVE_OFF:
		SDL_DisableScreenSaver();
		break;
	case SMT_SCREEN_SAVE_ON:
		SDL_EnableScreenSaver();
		break;
	case SMT_SCREEN_SAVE_QUERY:
		break;
	default:
		if (SDL_IsScreenSaverEnabled())
			SDL_DisableScreenSaver();
		else
			SDL_EnableScreenSaver();
		break;
	}
	return SDL_IsScreenSaverEnabled() ? SMT_SCREEN_SAVE_ON : SMT_SCREEN_SAVE_OFF;
}

int smtPos(unsigned win, int x, int y)
{
	winchk(win);
	SDL_SetWindowPosition(_smt.win.scr[win], x, y);
	return 0;
}

static inline void _smt_win_bounds(SDL_Window *scr, int x, int y, unsigned w, unsigned h)
{
	SDL_SetWindowPosition(scr, x, y);
	SDL_SetWindowSize(scr, w, h);
}

int smtBounds(unsigned win, int x, int y, unsigned w, unsigned h)
{
	winchk(win);
	_smt_win_bounds(_smt.win.scr[win], x, y, w, h);
	return 0;
}

int smtRelBounds(unsigned win, unsigned display, int x, int y, unsigned w, unsigned h)
{
	int dx, dy, ret;
	winchk(win);
	ret = smtDisplayBounds(display, &dx, &dy, NULL, NULL);
	if (ret) return ret;
	_smt_win_bounds(_smt.win.scr[win], dx + x, dy + y, w, h);
	return 0;
}

int smtBoundsp(unsigned win, int *x, int *y, unsigned *w, unsigned *h)
{
	winchk(win);
	return _smt_win_boundsp(win, x, y, w, h);
}

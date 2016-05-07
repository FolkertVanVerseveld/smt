#include <ctype.h>
#include <stdlib.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include "_smt.h"
#include "log.h"

struct _smtconf _smt = {
	.opt = {
		.img = SMT_IMG_PNG | SMT_IMG_JPG,
		.sfx = SMT_SFX_ALUT | SMT_SFX_AL_CTX
	}
};

struct smtconf smt;

inline void _smt_error(unsigned mask)
{
	_smt.err.genf |= mask;
	++_smt.err.count;
	_smt_pge();
}

void _smt_freecurs();

static void _smt_stop(void)
{
	/* make sure all acquired resources are freed */
	unsigned i;
	_smt_freesfx();
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
	_smt_freecurs();
	if (_smt.init.libs & INIT_IMG)
		IMG_Quit();
	if (_smt.init.libs & INIT_SDL)
		SDL_Quit();
	if (_smt.drop) SDL_free(_smt.drop);
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

int _smt_nogl(void)
{
	unsigned win = _smt.gl.curw;
	if (!(_smt.win.state[win] & SMT_WIN_INIT))
		return SMT_ERR_STATE;
	return !!SDL_GL_MakeCurrent(_smt.win.scr[win], NULL);
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
	_smt.gl.curw = win;
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

#define glchk(i) do{\
	if (i >= GLSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.gl.state[i] & SMT_GL_INIT))\
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

static inline void _smt_kbpcpy(void)
{
	register unsigned smod, mod, scan, virt;
	smod = _smt.ev.key.keysym.mod;
	scan = _smt.ev.key.keysym.scancode;
	virt = _smt.ev.key.keysym.sym;
	mod = 0;
	if (smod & KMOD_LSHIFT)
		mod |= SMT_MOD_LSHIFT;
	if (smod & KMOD_RSHIFT)
		mod |= SMT_MOD_RSHIFT;
	if (smod & KMOD_LALT)
		mod |= SMT_MOD_LALT;
	if (smod & KMOD_RALT)
		mod |= SMT_MOD_RALT;
	if (smod & KMOD_LCTRL)
		mod |= SMT_MOD_LCTRL;
	if (smod & KMOD_RCTRL)
		mod |= SMT_MOD_RCTRL;
	#define key2scan(x) ((x) & ~(SDLK_SCANCODE_MASK))
	if (virt >= SDLK_F1 && virt <= SDLK_F12)
		virt = key2scan(virt) - SDL_SCANCODE_F1 + SMT_KEY_F1;
	else if (virt >= SDLK_F13 && virt <= SDLK_F24)
		virt = key2scan(virt) - SDL_SCANCODE_F13 + SMT_KEY_F(13);
	else {
		switch (virt) {
		case SDLK_RIGHT: virt = SMT_KEY_RIGHT; break;
		case SDLK_UP: virt = SMT_KEY_UP; break;
		case SDLK_LEFT: virt = SMT_KEY_LEFT; break;
		case SDLK_DOWN: virt = SMT_KEY_DOWN; break;
		}
	}
	smt.kbp.mod = mod;
	smt.kbp.scan = scan;
	smt.kbp.virt = virt;
}

unsigned smtQwerty(void)
{
	register unsigned mod, virt, key;
	const unsigned char numup[] = {")!@#$%^&*("};
	mod = _smt.ev.key.keysym.mod;
	virt = _smt.ev.key.keysym.sym;
	key = virt & 0xff;
	if (isprint(key) && (mod & KMOD_SHIFT)) {
		/* map ranged keys */
		if (key >= 'a' && key <= 'z')
			return key - 'a' + 'A';
		if (key >= '0' && key <= '9')
			return numup[key - '0'];
		/* map special keys */
		switch (key) {
		case '`': return '~';
		case '-': return '_';
		case '=': return '+';
		case '[': return '{';
		case ']': return '}';
		case '\\': return '|';
		case ';': return ':';
		case '\'': return '"';
		case ',': return '<';
		case '.': return '>';
		case '/': return '?';
		}
	}
	return key;
}

static void _smt_drop(void)
{
	if (_smt.drop) SDL_free(_smt.drop);
	_smt.drop = strdup(_smt.ev.drop.file);
	char *p, *q;
	unsigned char fst, snd;
	for (p = q = _smt.drop; *p;) {
		if (*p == '%' && p[1] && p[1] != '%') {
			fst = tolower(p[1]);
			snd = tolower(p[2]);
			if (fst >= '0' && fst <= '9')
				fst -= '0';
			else
				fst -= 'a' - 10;
			if (snd >= '0' && snd <= '9')
				snd -= '0';
			else
				snd -= 'a' - 10;
			*q++ = fst << 4 | snd;
			p += 3;
		} else
			*q++ = *p++;
	}
	*q = '\0';
	smt.drop = _smt.drop;
}

unsigned smtPollev(void)
{
	if (SDL_PollEvent(&_smt.ev) == 0)
		return SMT_EV_DONE;
	switch (_smt.ev.type) {
	case SDL_QUIT: return SMT_EV_QUIT;
	case SDL_KEYDOWN: _smt_kbpcpy(); return SMT_EV_KEY_DOWN;
	case SDL_KEYUP: _smt_kbpcpy(); return SMT_EV_KEY_UP;
	case SDL_MOUSEMOTION:
		smt.mouse.x = _smt.ev.motion.x;
		smt.mouse.y = _smt.ev.motion.y;
		smt.mouse.state = _smt.ev.motion.state;
		return SMT_EV_MOUSE_MOTION;
	case SDL_DROPFILE:
		_smt_drop();
		return SMT_EV_DROP_FILE;
	default: return SMT_EV_UNKNOWN;
	}
}

int smtInit(int *argc, char **argv)
{
	(void)argc;
	(void)argv;
	atexit(_smt_stop);
	_smt.init.libs |= INIT_SMT;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		sdl_error();
	_smt.init.libs |= INIT_SDL;
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)
		sdl_error();
	_smt.init.gl |= SMT_GL_DOUBLE;
	unsigned img = _smt.opt.img;
	int imgmask = 0;
	if (img & SMT_IMG_PNG) imgmask |= IMG_INIT_PNG;
	if (img & SMT_IMG_JPG) imgmask |= IMG_INIT_JPG;
	if (imgmask) {
		if ((IMG_Init(imgmask) & imgmask) != imgmask)
			img_error();
		_smt.init.libs |= INIT_IMG;
	}
	if (_smt_initsfx(argc, argv) != 0) goto fail;
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

char *smtClip(void)
{
	return SDL_GetClipboardText();
}

int smtClips(const char *str)
{
	return SDL_SetClipboardText(str);
}

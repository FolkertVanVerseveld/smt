#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include "_smt.h"

unsigned smtOptimg(unsigned opt)
{
	return _smt.opt.img == opt || opt > SMT_IMG_ALL ? opt : (_smt.opt.img = opt);
}

#define sprchk(i) do{\
	if (i >= SPRSZ)\
		return SMT_ERR_OVERFLOW;\
	if (!(_smt.spr.state[i] & SMT_SPR_INIT))\
		return SMT_ERR_STATE;\
	}while(0)

#define PALSZ 256
static void _smt_setpal(SDL_Surface *surf)
{
	GLfloat r[PALSZ], g[PALSZ], b[PALSZ];
	int i, max = PALSZ;
	if (surf->format->palette->ncolors < max)
		max = surf->format->palette->ncolors;
	for (i = 0; i < max; ++i) {
		r[i] = (GLfloat)surf->format->palette->colors[i].r/PALSZ;
		g[i] = (GLfloat)surf->format->palette->colors[i].g/PALSZ;
		b[i] = (GLfloat)surf->format->palette->colors[i].b/PALSZ;
	}
	/* nvidia needs this, dunno if this is nvidia specific */
	glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_R, max, r);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_G, max, g);
	glPixelMapfv(GL_PIXEL_MAP_I_TO_B, max, b);
}

static inline int _smt_ispow2(unsigned x)
{
	return x && !(x & (x - 1));
}

static void _smt_maptex(SDL_Surface *surf, GLuint tex)
{
	GLint internal;
	GLenum format;
	glBindTexture(GL_TEXTURE_2D, tex);
	if (surf->format->palette) {
		_smt_setpal(surf);
		internal = GL_RGBA;
		format = GL_COLOR_INDEX;
	} else {
		/*
		jpeg *needs* GL_RGB or else we get a segfault
		png with GL_RGBA looks fine
		*/
		internal = (!surf->format->Amask || surf->format->BitsPerPixel == 24) ? GL_RGB : GL_RGBA;
		format = internal;
	}
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		internal, surf->w, surf->h,
		0, format, GL_UNSIGNED_BYTE, surf->pixels
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surf);
}

static SDL_Surface *_smt_gettex(const char *name)
{
	SDL_Surface *surf = IMG_Load(name);
	if (surf) return surf;
	_smt_error(ERR_IMG);
	return NULL;
}

static inline unsigned _smt_nextpow2(unsigned x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	++x;
	return x;
}

static int _smt_resizesurf(SDL_Surface **surf, unsigned size);

static int _smt_dirtymap(SDL_Surface **surf, unsigned *w, unsigned *h, unsigned flags)
{
	int ret = 0;
	if ((*surf)->w != (*surf)->h || !_smt_ispow2((unsigned)(*surf)->w)) {
		unsigned max = (int)((*surf)->w > (*surf)->h ? (*surf)->w : (*surf)->h);
		if (max < SMT_MIN_TEXTURE_SIZE)
			max = SMT_MIN_TEXTURE_SIZE;
		max = _smt_nextpow2(max);
		if (max > SMT_MAX_TEXTURE_SIZE) {
			ret = SMT_ERR_OVERFLOW;
			goto fail;
		}
		if ((flags & SMT_SPR_RESIZE) && !_smt_resizesurf(surf, max)) {
			ret = SMT_ERR_STATE;
			goto fail;
		}
	}
fail:
	if (w) *w = (*surf)->w;
	if (h) *h = (*surf)->h;
	if (ret) SDL_FreeSurface(*surf);
	return ret;
}

int smtCreatespr(unsigned *spr, unsigned w, unsigned h, const char *name, GLuint tex, unsigned flags)
{
	if (_smt.spr.n >= SPRSZ)
		return SMT_ERR_OVERFLOW;
	SDL_Surface *surf = _smt_gettex(name);
	if (!surf) goto fail;
	unsigned pw, ph, tw, th;
	pw = surf->w;
	ph = surf->h;
	if ((flags & SMT_SPR_STRICT) && (pw != w || ph != h)) {
		SDL_FreeSurface(surf);
		return SMT_ERR_STATE;
	}
	int ret = _smt_dirtymap(&surf, &tw, &th, flags);
	if (ret != 0) return ret;
	_smt_maptex(surf, tex);
	unsigned i;
	if (_smt.spr.ri)
		i = _smt.spr.rpop[--_smt.spr.ri];
	else
		i = _smt.spr.n++;
	_smt.spr.tex[i] = tex;
	_smt.spr.w[i] = pw;
	_smt.spr.h[i] = ph;
	_smt.spr.tw[i] = tw;
	_smt.spr.th[i] = th;
	_smt.spr.state[i] = SMT_SPR_INIT;
	*spr = i;
	return 0;
fail:
	return SMT_ERR_STATE;
}

int smtGetsizespr(unsigned spr, unsigned *pw, unsigned *ph, unsigned *vw, unsigned *vh)
{
	sprchk(spr);
	if (pw) *pw = _smt.spr.w[spr];
	if (ph) *ph = _smt.spr.h[spr];
	if (vw) *vw = _smt.spr.tw[spr];
	if (vh) *vh = _smt.spr.th[spr];
	return 0;
}

static int _smt_resizesurf(SDL_Surface **surf, unsigned size)
{
	SDL_Surface *orig = *surf;
	SDL_Surface *new = SDL_CreateRGBSurface(
		orig->flags, size, size, 32,
		orig->format->Rmask, orig->format->Gmask,
		orig->format->Bmask, orig->format->Amask
	);
	if (!new) return SMT_ERR_STATE;
	size_t nrow, orow, nlen, olen;
	orow = (unsigned)orig->pitch;
	olen = (unsigned)orig->w * (unsigned)orig->h * orig->format->BytesPerPixel;
	nrow = (unsigned)new->pitch;
	nlen = (unsigned)new->w * (unsigned)new->h * new->format->BytesPerPixel;
	SDL_LockSurface(orig);
	SDL_LockSurface(new);
	memset(new->pixels, 0, nlen);
	for (size_t opos = 0, npos = 0; opos < olen; opos += orow, npos += nrow)
		memcpy(&((Uint8*)new->pixels)[npos], &((Uint8*)orig->pixels)[opos], orow);
	SDL_UnlockSurface(new);
	SDL_UnlockSurface(orig);
	*surf = new;
	SDL_FreeSurface(orig);
	return 0;
}

int smtFreespr(unsigned i)
{
	sprchk(i);
	_smt.spr.rpop[_smt.spr.ri++] = i;
	_smt.spr.state[i] = 0;
	return 0;
}

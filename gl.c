#include "_smt.h"

#define bits(x) case x##_BITS:attr=SDL_##x##_SIZE;break
#define sdl(x) case x:attr=SDL_##x;break
#define ctx(x) case GL_##x:attr=SDL_GL_CONTEXT_##x;break

static inline int _smt_mapgla(unsigned opt, SDL_GLattr *a)
{
	SDL_GLattr attr;
	switch (opt) {
	bits(GL_RED);
	bits(GL_GREEN);
	bits(GL_BLUE);
	bits(GL_ALPHA);
	/* FIXME this is not the right option, but i don't know what is */
	case GL_SUBPIXEL_BITS:
		attr = SDL_GL_BUFFER_SIZE;
		break;
	sdl(GL_DOUBLEBUFFER);
	bits(GL_DEPTH);
	bits(GL_STENCIL);
	bits(GL_ACCUM_RED);
	bits(GL_ACCUM_GREEN);
	bits(GL_ACCUM_BLUE);
	sdl(GL_STEREO);
	case GL_SAMPLE_BUFFERS:
		attr = SDL_GL_MULTISAMPLEBUFFERS;
		break;
	case GL_SAMPLES:
		attr = SDL_GL_MULTISAMPLESAMPLES;
		break;
	ctx(MAJOR_VERSION);
	ctx(MINOR_VERSION);
	sdl(GL_CONTEXT_FLAGS);
	sdl(GL_CONTEXT_PROFILE_MASK);
	case GL_FRAMEBUFFER_SRGB:
		attr = SDL_GL_FRAMEBUFFER_SRGB_CAPABLE;
		break;
	default:
		return SMT_ERR_OVERFLOW;
	}
	*a = attr;
	return 0;
}

int smtGlAttru(GLuint opt, GLuint arg)
{
	SDL_GLattr attr;
	int ret, val;
	if ((ret = _smt_mapgla(opt, &attr)))
		return ret;
	val = arg;
	/* just to guarantee that GL_TRUE and GL_FALSE are properly interpreted */
	if (arg == GL_TRUE) val = 1;
	else if (arg == GL_FALSE) val = 0;
	if (opt == GL_CONTEXT_PROFILE_MASK) {
		if (arg == SMT_CTX_GL_OLD) {
			smtDbgs("gl: ctx old");
			if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY))
				sdl_error();
			return 0;
		} else if (arg == SMT_CTX_GL_NEW) {
			smtDbgs("gl: ctx new");
			if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
				sdl_error();
			return 0;
		}
	}
	if (SDL_GL_SetAttribute(attr, val))
		sdl_error();
	return 0;
fail:
	return SMT_ERR_STATE;
}

int smtGlAttrup(GLuint opt, GLuint *val)
{
	SDL_GLattr attr;
	int ret;
	if ((ret = _smt_mapgla(opt, &attr)))
		return ret;
	if (SDL_GL_GetAttribute(attr, &ret))
		sdl_error();
	if (val) *val = ret;
	return 0;
fail:
	return SMT_ERR_STATE;
}

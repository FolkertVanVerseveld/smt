#include "_smt.h"
#include "log.h"

unsigned smtOptsfx(unsigned opt)
{
	return _smt.opt.sfx == opt || opt > SMT_SFX_ALL ? opt : (_smt.opt.sfx = opt);
}

void _smt_freesfx(void)
{
	smtDbgs("freesfx");
	if (!(_smt.init.sfx)) return;
	if (_smt.init.sfx & SMT_SFX_ALUT) {
		smtDbgs("alutExit");
		alutExit();
	}
	/* alut does not close context and device,
	so we have to do that ourselves */
	if (!_smt.al.ctx)
		_smt.al.ctx = alcGetCurrentContext();
	if (_smt.al.ctx && !_smt.al.dev)
		_smt.al.dev = alcGetContextsDevice(_smt.al.ctx);
	alcMakeContextCurrent(NULL);
	if (_smt.al.ctx) {
		alcDestroyContext(_smt.al.ctx);
		_smt.al.ctx = NULL;
	}
#ifndef ALC_DEV_NOCLOSE
	if (_smt.al.dev) {
		alcCloseDevice(_smt.al.dev);
		_smt.al.dev = NULL;
	}
#endif
	_smt.init.sfx = 0;
}

static inline int _smt_inital(int *argc, char **argv)
{
	ALCcontext *ctx = NULL;
	ALCdevice *dev = NULL;
	const char *dname = NULL;
	int ret = 1;
	ret = 0;
	unsigned sfx = _smt.opt.sfx;
	smtDbgs("inital");
	if (!sfx) return 0;
	if (sfx & SMT_SFX_ALUT) {
		smtDbgs("alutInit");
		if (!alutInit(argc, argv)) goto fail;
		_smt.init.sfx |= SMT_SFX_ALUT;
		_smt.al.ctx = alcGetCurrentContext();
		if (_smt.al.ctx) {
			_smt.init.sfx |= SMT_SFX_AL_CTX;
			_smt.al.dev = alcGetContextsDevice(_smt.al.ctx);
		}
		return 0;
	}
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
		dname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	dev = alcOpenDevice(dname);
	if (!dev) goto fail;
	if (sfx & SMT_SFX_AL_CTX) {
		smtDbgs("alutInit ctx");
		ctx = alcCreateContext(dev, NULL);
		if (!ctx) goto fail;
		_smt.init.sfx |= SMT_SFX_AL_CTX;
		alcMakeContextCurrent(ctx);
	}
	ret = 0;
fail:
	_smt.al.ctx = ctx;
	_smt.al.dev = dev;
	if (ret) {
		if (ctx) alcDestroyContext(ctx);
		if (dev) alcCloseDevice(dev);
	}
	return ret;
}

int _smt_initsfx(int *argc, char **argv)
{
	int ret = 1;
	smtDbgs("initsfx");
	if (_smt.init.sfx) return 1;
	_smt.init.sfx |= SMT_SFX_INIT;
	if (_smt_inital(argc, argv) != 0) goto fail;
	ret = 0;
fail:
	if (ret) fputs("smt: no audio available\n", stderr);
	return ret;
}

int smtOggfv(const char *name, char **buf, size_t *count, ALenum *format, ALsizei *freq)
{
	int ret = 1, big = 0, open = 0;
	char *ptr = NULL;
	size_t sz = 32768;
	/* NOTE do *not* issue fclose(f) as ov_open already does this! */
	FILE *f;
	vorbis_info *info = NULL;
	OggVorbis_File ogg;
	if (!(ptr = malloc(sz))) {
		ret = SMT_ERR_NOMEM;
		goto fail;
	}
	f = fopen(name, "rb");
	if (!f) {
		ret = SMT_ERR_STATE;
		goto fail;
	}
	if (ov_open(f, &ogg, NULL, 0) != 0) {
		ret = SMT_ERR_DOMAIN;
		/* NOTE except when it fails */
		fclose(f);
		goto fail;
	}
	open = 1;
	info = ov_info(&ogg, -1);
	*format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	*freq = info->rate;
	long n;
	size_t tot = 0;
	char *pos = ptr;
	int p = 0;
	do {
		n = ov_read(&ogg, pos, sz - tot, big, 2, 1, &p);
		if (n < 0) {
			ret = SMT_ERR_DOMAIN;
			goto fail;
		}
		tot += n;
		pos += n;
		if (tot >= sz * 3 / 4) {
			char *new = realloc(ptr, sz << 1);
			if (!new) {
				ret = SMT_ERR_NOMEM;
				goto fail;
			}
			ptr = new;
			pos = &ptr[tot];
			sz <<= 1;
		}
	} while (n > 0);
	ret = 0;
fail:
	if (open) ov_clear(&ogg);
	if (ret) {
		if (ptr) free(ptr);
		ptr = NULL;
	} else
		*count = tot;
	*buf = ptr;
	return ret;
}

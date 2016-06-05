/* play sine wave at 440Hz */
#include <stdio.h>
#include <stdlib.h>
#include <smt/smt.h>
#include <AL/alut.h>

unsigned win, gl;

#ifdef AL_INVALID
#undef AL_INVALID
#endif
#define AL_INVALID ((ALuint)-1)

ALuint src = AL_INVALID;
ALuint buf = AL_INVALID;

static inline void cleanup(void)
{
	static unsigned freed = 0;
	if (!freed) freed = 1;
	if (buf != AL_INVALID)
		alDeleteBuffers(1, &buf);
	if (src != AL_INVALID)
		alDeleteSources(1, &src);
	smtFreegl(gl);
	smtFreewin(win);
}

static inline void init(void)
{
	alGenSources(1, &src);
	ALfloat freq = 440;
	ALfloat phase = 0;
	ALfloat sec = 0.1f;
	buf = alutCreateBufferWaveform(ALUT_WAVEFORM_SINE, freq, phase, sec);
	atexit(cleanup);
}

static inline void display(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{
	smtInit(&argc, argv);
	smtCreatewin(&win, 300, 100, "wave forms", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	init();
	alSourcei(src, AL_BUFFER, buf);
	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSourcef(src, AL_GAIN, 1);
	alSourcePlay(src);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		display();
		smtSwapgl(win);
	}
end:
	cleanup();
	return 0;
}

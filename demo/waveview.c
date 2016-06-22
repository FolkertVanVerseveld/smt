/*
compute and draw waveform in real time
redefine wave_phase to the wave form routine.
provided examples are:

name         | formula
-------------+--------------------------------------
half sinusoid| amplitude = abs(sin(phi))
sinusoid     | amplitude = sin(2 * phi)
sawtooth     | amplitude = 2 * phi - 1
square       | amplitude is 1 if phi >= 0.5, else -1
noise        | amplitude is pseudo random

where phi lies in range: [0, pi]
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <smt/smt.h>
#include <AL/alut.h>

#define WIDTH 384
#define HEIGHT 128
#define TITLE "Waveform Viewer"

unsigned win, gl;

GLenum gl_err;
ALenum al_err;

#define AL_RES_INVALID ((ALuint)-1)

ALuint wave_source = AL_RES_INVALID;
ALuint wave_channel = AL_RES_INVALID;

struct waveform {
	ALvoid *data;
	size_t length;
	ALint channels;
	ALenum format;
	ALint sample_bits;
	ALfloat sample_freq;
} wave_form;

static void waveform_free(struct waveform*);

static void cleanup(void)
{
	waveform_free(&wave_form);
	if (wave_channel != AL_RES_INVALID) {
		alDeleteBuffers(1, &wave_channel);
		wave_channel = AL_RES_INVALID;
	}
	if (wave_source != AL_RES_INVALID) {
		alDeleteSources(1, &wave_source);
		wave_source = AL_RES_INVALID;
	}
	if (gl != SMT_RES_INVALID) {
		smtFreegl(gl);
		gl = SMT_RES_INVALID;
	}
	if (win != SMT_RES_INVALID) {
		smtFreewin(win);
		win = SMT_RES_INVALID;
	}
}

#define SAMPLE_FREQUENCY 44100.0

/* works best for durations >= 0.1 seconds */
double wave_noise(double phase)
{
	static const long prime = 67867967L;
	return 2 * (double)(rand() % prime) / prime - 1;
}

/* only upper half */
double wave_half_sine(double phase)
{
	return sin(phase * M_PI);
}

double wave_sine(double phase)
{
	return sin(2.0 * phase * M_PI);
}

double wave_sawtooth(double phase)
{
	return 2 * phase - 1;
}

double wave_square(double phase)
{
	return phase >= 0.5 ? 1 : -1;
}

static void waveform_free(struct waveform *wave)
{
	if (wave->data) free(wave->data);
	wave->data = NULL;
}

#define wave_phase(x) wave_sawtooth(x)

static size_t waveform_init(struct waveform *wave, ALfloat frequency, ALfloat phase, ALfloat duration)
{
	size_t samples, i, wave_pos = 0, length = 0, blksz;
	int16_t *blk = NULL;
	samples = (size_t)floor(round(frequency * duration) / frequency * SAMPLE_FREQUENCY);
	blk = malloc(blksz = samples * sizeof(int16_t));
	if (!blk) goto fail;
	for (i = 0; i < samples; ++i) {
		double p = phase + frequency * (double)i / SAMPLE_FREQUENCY;
		blk[wave_pos++] = ((int16_t)(wave_phase(p - floor(p)) * 32767));
	}
	wave->data = blk;
	wave->length = blksz;
	wave->channels = 1;
	wave->format = AL_FORMAT_MONO16;
	wave->sample_bits = 16;
	wave->sample_freq = SAMPLE_FREQUENCY;
fail:
	if (!blksz) {
		if (blk) free(blk);
	}
	return blksz;
}

static inline void init(void)
{
	alGenSources(1, &wave_source);
	ALfloat freq = 440;
	ALfloat phase = 0;
	ALfloat sec = 0.1f;
	alGenBuffers(1, &wave_channel);
	waveform_init(&wave_form, freq, phase, sec);
	printf(
		"length: %zu\n"
		"channels: %d\n"
		"frequency: %.2f\n",
		wave_form.length,
		wave_form.channels,
		wave_form.sample_freq
	);
	alBufferData(wave_channel, wave_form.format, wave_form.data, wave_form.length, wave_form.sample_freq);
	ALint wave_size;
	alGetBufferi(wave_channel, AL_SIZE, &wave_size);
	printf("wave size: %d bytes\n", wave_size);
}

static inline void display(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	int16_t i, i_n, n, *data = wave_form.data;
	ALint pos;
	n = wave_form.length / sizeof(int16_t);
	i_n = n > 256 ? 256 : n;
	alGetSourcei(wave_source, AL_BYTE_OFFSET, &pos);
	pos = (pos % n) / sizeof(int16_t);
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < i_n; ++i) {
		glVertex2f((float)i / i_n, data[pos] / 32768.0f * .8f);
		pos = (pos + 1) % n;
	}
	glEnd();
}

static inline int glchk(void) {
	if ((gl_err = glGetError()) != GL_NO_ERROR)
		fprintf(stderr, "OpenGL error: %u\n", gl_err);
	return gl_err != GL_NO_ERROR;
}
static inline int alchk(void) {
	if ((al_err = alGetError()) != AL_NO_ERROR)
		fprintf(stderr, "OpenAL error: %u\n", al_err);
	return al_err != AL_NO_ERROR;
}

int main(int argc, char **argv)
{
	smtInit(&argc, argv);
	win = gl = SMT_RES_INVALID;
	srand(time(NULL));
	atexit(cleanup);
	if (smtCreatewin(&win, WIDTH, HEIGHT, TITLE, SMT_WIN_VISIBLE | SMT_WIN_BORDER))
		return 1;
	if (smtCreategl(&gl, win))
		return 1;
	init();
	alSourcei(wave_source, AL_BUFFER, wave_channel);
	alSourcei(wave_source, AL_LOOPING, AL_TRUE);
	alSourcef(wave_source, AL_GAIN, .4f);
	alSourcePlay(wave_source);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		display();
		glchk();
		alchk();
		smtSwapgl(win);
	}
end:
	cleanup();
	return 0;
}

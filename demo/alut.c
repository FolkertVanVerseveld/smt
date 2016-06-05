/* play default buffer from ALUT */
#include <smt/smt.h>
#include <AL/alut.h>

#ifdef AL_INVALID
#undef AL_INVALID
#endif
#define AL_INVALID ((ALuint)-1)

ALuint src = AL_INVALID;
ALuint buf = AL_INVALID;

static void cleanup(void)
{
	static unsigned freed = 0;
	if (!freed) freed = 1;
	if (buf != AL_INVALID)
		alDeleteBuffers(1, &buf);
	if (src != AL_INVALID)
		alDeleteSources(1, &src);
}

int main(int argc, char **argv)
{
	unsigned win, gl;
	smtInit(&argc, argv);
	smtCreatewin(&win, 300, 200, "sup", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	buf = alutCreateBufferHelloWorld();
	alGenSources(1, &src);
	alSourcei(src, AL_BUFFER, buf);
	alSourcePlay(src);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		smtSwapgl(win);
	}
end:
	cleanup();
	return 0;
}

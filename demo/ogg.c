#include <stdio.h>
#include <stdlib.h>
#include <smt/smt.h>
#include <AL/alut.h>

/*
AL lib says we have to free a source, but as far as I can tell we can't access the source that has been allocated
*/

int main(int argc, char **argv)
{
	unsigned win, gl;
	smtInit(&argc, argv);
	if (argc != 2) {
		fprintf(stderr, "usage: %s file\n", argc > 0 ? argv[0] : "ogg");
		return 1;
	}
	smtCreatewin(&win, 300, 200, argv[1], SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	ALuint buf, src;
	char *ogg;
	ALenum format;
	ALsizei freq;
	size_t n;
	if (smtOggfv(argv[1], &ogg, &n, &format, &freq) != 0)
		buf = alutCreateBufferHelloWorld();
	else {
		alGenBuffers(1, &buf);
		alBufferData(buf, format, ogg, n, freq);
		free(ogg);
	}
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
	alDeleteSources(1, &src);
	alDeleteBuffers(1, &buf);
	alcMakeContextCurrent(NULL);
end:
	return 0;
}

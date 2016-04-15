#include <smt/smt.h>
#include <AL/alut.h>

/*
AL lib says we have to free a source, but as far as I can tell we can't access the source that has been allocated
*/

int main(int argc, char **argv)
{
	unsigned win, gl;
	smtInit(&argc, argv);
	smtCreatewin(&win, 300, 200, "sup", SMT_WIN_VISIBLE | SMT_WIN_BORDER);
	smtCreategl(&gl, win);
	ALuint helloBuffer, helloSource;
	helloBuffer = alutCreateBufferHelloWorld();
	alGenSources(1, &helloSource);
	alSourcei(helloSource, AL_BUFFER, helloBuffer);
	alSourcePlay(helloSource);
	while (1) {
		unsigned ev;
		while ((ev = smtPollev()) != SMT_EV_DONE) {
			switch (ev) {
			case SMT_EV_QUIT: goto end;
			}
		}
		smtSwapgl(win);
	}
	alDeleteBuffers(1, &helloBuffer);
	alDeleteSources(1, &helloSource);
	alcMakeContextCurrent(NULL);
end:
	return 0;
}

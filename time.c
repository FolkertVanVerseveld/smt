#include "_smt.h"

unsigned smtTicks(void)
{
	return SDL_GetTicks();
}

unsigned smtSleep(unsigned ms)
{
	Uint32 t = SDL_GetTicks();
	SDL_Delay(ms);
	return SDL_GetTicks() - t;
}

void smtResett(struct smtTimer *t)
{
	t->elapsed = 0;
	t->last = smtTicks();
}

unsigned smtTickt(struct smtTimer *t)
{
	unsigned diff;
	t->next = smtTicks();
	diff = t->next < t->last ? t->delay : t->next - t->last;
	t->last = t->next;
	t->elapsed += diff;
	return diff;
}

unsigned smtSlicet(struct smtTimer *t, unsigned delay)
{
	unsigned dt = t->elapsed;
	if (dt <= delay) return 0;
	t->elapsed %= delay;
	return dt / delay;
}

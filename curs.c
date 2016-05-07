#include "_smt.h"

void _smt_freecurs(void)
{
	unsigned i;
	for (i = 0; i < SMT_CURS_MAX; ++i)
		if (_smt.curs.init & (1 << i)) {
			SDL_FreeCursor(_smt.curs.data[i]);
			_smt.curs.data[i] = NULL;
		}
}

int smtCursor(unsigned this, unsigned state)
{
	const SDL_SystemCursor tbl[SMT_CURS_MAX] = {
		0,
		SDL_SYSTEM_CURSOR_ARROW,
		SDL_SYSTEM_CURSOR_IBEAM,
		SDL_SYSTEM_CURSOR_WAIT,
		SDL_SYSTEM_CURSOR_CROSSHAIR,
		SDL_SYSTEM_CURSOR_WAITARROW,
		SDL_SYSTEM_CURSOR_SIZENWSE,
		SDL_SYSTEM_CURSOR_SIZEWE,
		SDL_SYSTEM_CURSOR_SIZENS,
		SDL_SYSTEM_CURSOR_SIZEALL,
		SDL_SYSTEM_CURSOR_NO,
		SDL_SYSTEM_CURSOR_HAND
	};
	if (this >= SMT_CURS_MAX)
		return SMT_ERR_OVERFLOW;
	if (_smt.curs.cur == this) goto skip;
	if (!this) {
		SDL_Cursor *def = SDL_GetDefaultCursor();
		if (!def) return SMT_ERR_STATE;
		_smt.curs.init |= 1 << 0;
		SDL_SetCursor(def);
		return 0;
	}
	SDL_Cursor *curs;
	if (!(_smt.curs.init & (1 << this))) {
		curs = SDL_CreateSystemCursor(tbl[this]);
		if (!curs) return SMT_ERR_STATE;
		_smt.curs.init |= 1 << this;
	} else
		curs = _smt.curs.data[this];
	_smt.curs.cur = this;
	SDL_SetCursor(curs);
skip:
	switch (state) {
	case SMT_CURS_HIDE:
		if (SDL_ShowCursor(0) != 0)
			return SMT_ERR_STATE;
		break;
	case SMT_CURS_TOGGLE: {
		int new, state = SDL_ShowCursor(-1);
		if (state != 0 && state != 1)
			return SMT_ERR_STATE;
		new = 1 - state;
		if (SDL_ShowCursor(new) != new)
			return SMT_ERR_STATE;
		break;
	}
	default:
		if (SDL_ShowCursor(1) != 1)
			return SMT_ERR_STATE;
	}
	return 0;
}

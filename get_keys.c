#include "terrain.h"

SDL_bool get_key_press(SDL_Event *e, int *angle)
{
	if (e->type == SDL_QUIT
	    || e->key.keysym.sym == SDLK_ESCAPE
	    || e->key.keysym.sym == SDLK_q)
	{
		return SDL_TRUE;
	}
	else if (e->key.keysym.sym == SDLK_RIGHT
		 || e->key.keysym.sym == SDLK_LEFT)
	{
		if (e->key.keysym.sym == SDLK_RIGHT)
			*angle = (*angle == 0 ? 359 : *angle - 1);
		else
			*angle = (*angle == 359 ? 0 : *angle + 1);
	}
	return SDL_FALSE;
}

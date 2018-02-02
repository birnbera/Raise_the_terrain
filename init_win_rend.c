#include "terrain.h"

/**
 * init_win_rend - Initialize window and renderer
 * @win: address of pointer to window
 * @rend: address of pointer to renderer
 */
void init_win_rend(SDL_Window **win, SDL_Renderer **rend)
{
	atexit(SDL_Quit);
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "Could not initialize SDL: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (SDL_CreateWindowAndRenderer(SCR_W, SCR_H, 0, win, rend))
	{
		fprintf(stderr, "Could not create window and/or renderer: %s",
			SDL_GetError());
		exit(EXIT_FAILURE);
	}
	on_exit(destroy_renderer, *rend);
	on_exit(destroy_window, *win);
}

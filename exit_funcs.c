#include "terrain.h"

/**
 * destroy_renderer - function called on exiting from program to deallocate
 * the SDL2 renderer.
 * @status: exit status
 * @rend: pointer to renderer
 */
void destroy_renderer(int status, void *rend)
{
    (void) status;

    if (rend)
	SDL_DestroyRenderer(rend);
}

/**
 * destroy_window - function called on exiting from program to deallocate
 * the SDL2 window.
 * @status: exit status
 * @win: pointer to window
 */
void destroy_window(int status, void *win)
{
    (void) status;

    if (win)
	SDL_DestroyWindow(win);
}

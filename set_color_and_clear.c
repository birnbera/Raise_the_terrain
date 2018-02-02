#include "terrain.h"

/**
 * reset_and_clear - Set drawing color for renderer to black and clear buffer,
 * then set color to white for drawing on buffer.
 * @rend: pointer to renderer
 */
void reset_and_clear(SDL_Renderer *rend)
{
	SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);
	SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

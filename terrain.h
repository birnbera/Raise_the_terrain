#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#define SCR_W 640
#define SCR_H 480
#define GRD_SZ 8

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

void close_file(int status, void *file);
void destroy_renderer(int status, void *rend);
void destroy_window(int status, void *win);
void init_grid(SDL_Point (*grid)[GRD_SZ], FILE *altitudes);
void draw_grid(SDL_Point (*grid)[GRD_SZ], SDL_Renderer *rend);

#endif /* _TERRAIN_H_ */

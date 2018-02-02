#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#define SCR_W 640
#define SCR_H 480
#define ZSCALE 0.5
#define INCLINATION 0.7
#define PI 3.141592654

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * float3d_t - struct to store a 3d point's coordinates as floats
 * @x: x coordinate
 * @y: y coordinate
 * @z: z coordinate
 */
typedef struct {
    float x;
    float y;
    float z;
} float3d_t;

void init_win_rend(SDL_Window **win, SDL_Renderer **rend);
void destroy_renderer(int status, void *rend);
void destroy_window(int status, void *win);
float3d_t **get_altitudes(char *fname, size_t *saverows, size_t *savecols);
float3d_t **allocate_coords(size_t nrows, size_t ncols);
void free_coords(float3d_t **coords, size_t i);
SDL_Point **allocate_grid(size_t nrows, size_t ncols);
void free_grid(SDL_Point **grid, size_t i);
void init_xycoords(float3d_t **coords, size_t nrows, size_t ncols);
void project_to_grid(SDL_Point **grid,
		     float3d_t **coords,
		     size_t nrows,
		     size_t ncols,
		     int angle);
void reset_and_clear(SDL_Renderer *rend);
void draw_grid(SDL_Point **grid,
	       size_t nrows,
	       size_t ncols,
	       SDL_Renderer *rend);
SDL_bool get_key_press(SDL_Event *e, int *angle);
#endif /* _TERRAIN_H_ */

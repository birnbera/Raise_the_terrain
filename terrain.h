#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#define SCR_W 640
#define SCR_H 480
#define GRD_SZ 8
#define ZSCALE 0.5
#define INCLINATION 0.7
#define PI 3.141592654

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>


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

void destroy_renderer(int status, void *rend);
void destroy_window(int status, void *win);
void get_zcoords(float3d_t coords[GRD_SZ][GRD_SZ], FILE *altitudes);
void init_xycoords(float3d_t coords[GRD_SZ][GRD_SZ]);
void project_to_grid(SDL_Point grid[GRD_SZ][GRD_SZ],
		     float3d_t coords[GRD_SZ][GRD_SZ],
		     int angle);
void draw_grid(SDL_Point grid[GRD_SZ][GRD_SZ], SDL_Renderer *rend);

#endif /* _TERRAIN_H_ */

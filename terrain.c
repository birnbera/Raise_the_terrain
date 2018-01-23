#include "terrain.h"

/**
 * main - entry point for program, contains main event loop: checks for key
 * input and calls rendering routines.
 * @argc: argument count
 * @argv: pointer to command line arguments
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE if some error occurs
 */
int main(int argc, char *argv[])
{
    size_t i, j;
    FILE *altitudes;
    SDL_Window *win = NULL;
    SDL_Renderer *rend = NULL;
    SDL_Event e;
    SDL_Point grid[GRD_SZ][GRD_SZ];
    float3d_t coords[GRD_SZ][GRD_SZ];
    int angle = 0;
    int quit = SDL_FALSE;

    if (argc < 2)
    {
	fprintf(stderr, "Usage: %s <altitudes>\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);
    if (SDL_Init(SDL_INIT_VIDEO))
    {
	fprintf(stderr, "Could not initialize SDL: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if (SDL_CreateWindowAndRenderer(SCR_W, SCR_H, 0, &win, &rend))
    {
	fprintf(stderr, "Could not create window and/or renderer: %s",
		SDL_GetError());
	exit(EXIT_FAILURE);
    }
    on_exit(destroy_renderer, rend);
    on_exit(destroy_window, win);
    SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rend);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);

    altitudes = fopen(argv[1], "r");

    init_xycoords(coords);
    get_zcoords(coords, altitudes);
    fclose(altitudes);

    project_to_grid(grid, coords, angle);

    draw_grid(grid, rend);
    SDL_RenderPresent(rend);
    while (!quit)
    {
	while (SDL_PollEvent(&e))
	{
	    if (e.type == SDL_QUIT
		|| e.key.keysym.sym == SDLK_ESCAPE
		|| e.key.keysym.sym == SDLK_q)
	    {
		quit = SDL_TRUE;
	    }
	    else if (e.key.keysym.sym == SDLK_RIGHT ||
		     e.key.keysym.sym == SDLK_LEFT)
	    {
		if (e.key.keysym.sym == SDLK_RIGHT)
		    angle = (angle == 0 ? 359 : angle - 1);
		else
		    angle = (angle == 359 ? 0 : angle + 1);

		SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);

		project_to_grid(grid, coords, angle);
		draw_grid(grid, rend);

		SDL_RenderPresent(rend);
	    }
	}
    }
    exit(EXIT_SUCCESS);
}

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

/**
 * get_zcoords - function to read matrix of z coordinates from file
 * @coords: 2d matrix of 3d points in which to place z coordinates
 * @altitudes: file pointer from which to read z coordinates
 */
void get_zcoords(float3d_t coords[GRD_SZ][GRD_SZ], FILE *altitudes)
{
    size_t i, j, n;
    char *line, *saveptr, *token;
    char *delims = "\n\r\t ";

    line = NULL;
    i = 0;
    while (i < GRD_SZ && getline(&line, &n, altitudes) >=0)
    {
	j = 0;
	token = strtok_r(line, delims, &saveptr);
	while (j < GRD_SZ && token != NULL)
	{
	    coords[i][j].z = atof(token) * ZSCALE;
	    j++;
	    token = strtok_r(NULL, delims, &saveptr);
	}
	i++;
    }
    free(line);
}

/**
 * init_xycoords - initialize a 2d grid of points such that the center of
 * the grid is origin of the x/y axes. The values are in local unit length
 * coordinates.
 * @coords: 2d matrix of 3d points in which to place z coordinates
 */
void init_xycoords(float3d_t coords[GRD_SZ][GRD_SZ])
{
    size_t i, j;
    float x, y;

    for (i = 0, x = GRD_SZ / -2.0 + 0.5; i < GRD_SZ; i++, x++)
    {
	for (j = 0, y = GRD_SZ / -2.0 + 0.5; j < GRD_SZ; j++, y++)
	{
	    coords[i][j].x = x;
	    coords[i][j].y = y;
	}
    }
}

/**
 * project_to_grid - takes a matrix of points in 3d coordinates and maps them
 * isometrically to 2d screen space and stores the result as integers using the
 * SDL_Point struct.
 * @grid: 2d matrix to represent points from `coords' in screen coordinates
 * @coords: 2d matrix of 3d points to be transformed into screen coordinates
 * @angle: angle between 0 and 359 degrees with which to rotate the points along
 * the x/y plane.
 */
void project_to_grid(SDL_Point grid[GRD_SZ][GRD_SZ],
		     float3d_t coords[GRD_SZ][GRD_SZ],
		     int angle)
{
    size_t i, j;
    float a;
    float wx, wy, Rx, Ry, xmin, xmax, ymax;
    float3d_t *coord;

    xmax = 0.7 * (GRD_SZ - 1);
    xmin = -xmax;
    ymax = 0.3 * 2 * (GRD_SZ - 1);

    for (i = 0; i < GRD_SZ; i++)
    {
	for (j = 0; j < GRD_SZ; j++)
	{
	    coord = &(coords[i][j]);

	    a = angle * PI / 180.0;
	    Rx = coord->x * cos(a) - coord->y * sin(a);
	    Ry = coord->x * sin(a) + coord->y * cos(a);

	    wx = INCLINATION * (Rx - Ry);
	    wy = (1 - INCLINATION) * (Rx + Ry);

	    wx -= xmin;
	    wx *= 0.8 * SCR_W / (xmax - xmin);
	    wx += 0.1 * SCR_W;

	    wy *= 0.5 * SCR_H / ymax;
	    wy += 0.5 * SCR_H;

	    wy -= 0.5 * coord->z;

	    grid[i][j].x = wx;
	    grid[i][j].y = wy;
	}
    }
}

/**
 * draw_grid - take transformed points in screen coordinates and draw lines
 * between neighboring points to give the appearance of a mesh.
 * @grid: 2d matrix of points to render to the screen
 * @renderer: back buffer rendering agent
 */
void draw_grid(SDL_Point grid[GRD_SZ][GRD_SZ], SDL_Renderer *renderer)
{
    size_t i, j;

    for (i = 0; i < GRD_SZ - 1; i++)
    {
	SDL_RenderDrawLines(renderer, grid[i], GRD_SZ);
	for (j = 0; j < GRD_SZ; j++)
	    SDL_RenderDrawLine(renderer,
			       grid[i][j].x, grid[i][j].y,
			       grid[i+1][j].x, grid[i+1][j].y);
    }
    SDL_RenderDrawLines(renderer, grid[i], GRD_SZ);
}

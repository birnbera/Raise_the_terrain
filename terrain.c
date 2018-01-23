#include "terrain.h"


int main(int argc, char *argv[])
{
    size_t i, j;
    FILE *altitudes;
    SDL_Window *win = NULL;
    SDL_Renderer *rend = NULL;
    SDL_Event e;
    SDL_Point grid[GRD_SZ][GRD_SZ];
    float3d_t coords[GRD_SZ][GRD_SZ];
    int hAngle = 0;
    int vAngle = 0;
    int quit = SDL_FALSE;

    if (argc < 2)
    {
	fprintf(stderr, "Usage: %s <altitudes>\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    altitudes = fopen(argv[1], "r");
    on_exit(close_file, altitudes);
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

    init_xycoords(coords);
    get_zcoords(coords, altitudes);
    project_to_grid(grid, coords, hAngle, vAngle);

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
	    else if (e.key.keysym.sym == SDLK_RIGHT
		     || e.key.keysym.sym == SDLK_LEFT
		     || e.key.keysym.sym == SDLK_UP
		     || e.key.keysym.sym == SDLK_DOWN)
	    {
		if (e.key.keysym.sym == SDLK_RIGHT)
		    hAngle = (hAngle == 0 ? 359 : hAngle - 1);
		else if (e.key.keysym.sym == SDLK_LEFT)
		    hAngle = (hAngle == 359 ? 0 : hAngle + 1);
		else if (e.key.keysym.sym == SDLK_UP)
		    vAngle = (vAngle == 0 ? 359 : vAngle - 1);
		else
		    vAngle = (vAngle == 359 ? 0 : vAngle + 1);

		SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);

		project_to_grid(grid, coords, hAngle, vAngle);
		draw_grid(grid, rend);

		SDL_RenderPresent(rend);
	    }
	}
    }
    exit(EXIT_SUCCESS);
}

void close_file(int status, void *file)
{
    (void) status;

    fclose(file);
}

void destroy_renderer(int status, void *rend)
{
    (void) status;

    if (rend)
	SDL_DestroyRenderer(rend);
}

void destroy_window(int status, void *win)
{
    (void) status;

    if (win)
	SDL_DestroyWindow(win);
}

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

void project_to_grid(SDL_Point grid[GRD_SZ][GRD_SZ],
		     float3d_t coords[GRD_SZ][GRD_SZ],
		     int hAngle,
		     int vAngle)
{
    size_t i, j;
    float ha, va;
    float wx, wy, Rx, Ry, Rz, xmin, xmax, ymax;
    float3d_t *coord;

    xmax = 0.7 * (GRD_SZ - 1);
    xmin = -xmax;
    ymax = 0.3 * 2 * (GRD_SZ - 1);

    for (i = 0; i < GRD_SZ; i++)
    {
	for (j = 0; j < GRD_SZ; j++)
	{
	    coord = &(coords[i][j]);

	    ha = hAngle * PI / 180.0;
	    va = vAngle * PI / 180.0;

	    Rx = coord->x * cos(ha) - coord->y * sin(ha);
	    Ry = coord->x * sin(ha) + coord->y * cos(ha);

	    Ry = Ry * cos(va) - coord->z * sin(va);
	    Rz = Ry * sin(va) + coord->z * cos(va);

	    wx = INCLINATION * (Rx - Ry);
	    wy = (1 - INCLINATION) * (Rx + Ry);

	    wx -= xmin;
	    wx *= 0.8 * SCR_W / (xmax - xmin);
	    wx += 0.1 * SCR_W;

	    wy *= 0.5 * SCR_H / ymax;
	    wy += 0.5 * SCR_H;

	    wy -= 0.5 * Rz;

	    grid[i][j].x = wx;
	    grid[i][j].y = wy;
	}
    }
}

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

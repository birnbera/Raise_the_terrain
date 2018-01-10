#include "terrain.h"

int main(int argc, char *argv[])
{
	FILE *altitudes;
	SDL_Window *win = NULL;
	SDL_Renderer *rend = NULL;
	SDL_Event e;
	SDL_Point grid[GRD_SZ][GRD_SZ];
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
	init_grid(grid, altitudes);
	draw_grid(grid, rend);
	SDL_RenderPresent(rend);
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT
			    || e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = SDL_TRUE;
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

void init_grid(SDL_Point (*grid)[GRD_SZ], FILE *altitudes)
{
	size_t i, j, n;
	double x, y, z, xmin, xmax, ymax;
	char *line, *saveptr, *token;
	char *delims = "\n\t\r ";


	xmax = 0.7 * GRD_SZ;
	xmin = -xmax;
	ymax = 0.3 * 2 * GRD_SZ;

	line = NULL;
	i = 0;
	while (getline(&line, &n, altitudes) >= 0)
	{
		token = strtok_r(line, delims, &saveptr);
		j = 0;
		while (token != NULL)
		{
			z = atof(token);
			x = 0.7 * (i - j);
			y = 0.3 * (i + j) - z;

			x -= xmin;
			x *= SCR_W / (xmax - xmin);
			y *= SCR_H / ymax;

			grid[i][j].x = lround(x);
			grid[i][j].y = lround(y);
			token = strtok_r(NULL, delims, &saveptr);
			y++;
		}
		x++;
	}
	free(line);
}

void draw_grid(SDL_Point (*grid)[GRD_SZ], SDL_Renderer *renderer)
{
    size_t x, y;

    for (x = 0; x < GRD_SZ - 1; x++)
    {
	SDL_RenderDrawLines(renderer, grid[x], GRD_SZ);
	for (y = 0; y < GRD_SZ; y++)
	    SDL_RenderDrawLine(renderer,
			       grid[x][y].x, grid[x][y].y,
			       grid[x+1][y].x, grid[x+1][y].y);
    }
    SDL_RenderDrawLines(renderer, grid[x], GRD_SZ);
}

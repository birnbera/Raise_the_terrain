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
	size_t i, j, nrows, ncols;
	int angle, tmp;
	SDL_bool quit;
	SDL_Event e;
	SDL_Window *win;
	SDL_Renderer *rend;
	float3d_t **coords;
	SDL_Point **grid;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <altitudes>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	coords = get_altitudes(argv[1], &nrows, &ncols);
	init_xycoords(coords);

	init_win_rend(&win, &rend);
	reset_and_clear(rend);

	project_to_grid(grid, coords, angle);
	draw_grid(grid, rend);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			tmp = angle;
			quit = get_key_press(&e, &angle);
		}
		if (tmp != angle)
		{
			reset_and_clear(rend);
			project_to_grid(grid, coords, angle);
			draw_grid(grid, rend);
		}
	}
	exit(EXIT_SUCCESS);
}

/**
 * init_xycoords - initialize a 2d grid of points such that the center of
 * the grid is origin of the x/y axes. The values are in local unit length
 * coordinates.
 * @coords: 2d matrix of 3d points in which to place z coordinates
 */
void init_xycoords(float3d_t **coords, size_t nrows, size_t ncols)
{
	size_t i, j;

	for (i = 0; i < nrows; i++)
	{
		for (j = 0; j < ncols; j++)
		{
			coords[i][j].y = i;
			coords[i][j].x = j;
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
void project_to_grid(SDL_Point **grid,
		     float3d_t **coords,
		     size_t nrows,
		     size_t ncols,
		     int angle)
{
	size_t i, j;
	float a;
	float wx, wy, Rx, Ry, xmin, xmax, ymax;
	float3d_t *coord;

	xmax = INCLINATION * ncols;
	xmin = -xmax;
	ymax = 0.3 * 2 * nrows;

	for (i = 0; i < nrows; i++)
	{
		for (j = 0; j < ncols; j++)
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
 * @rend: back buffer rendering agent
 */
void draw_grid(SDL_Point **grid,
	       size_t nrows,
	       size_t ncols,
	       SDL_Renderer *rend)
{
    size_t i, j;

    for (i = 0; i < nrows - 1; i++)
    {
	SDL_RenderDrawLines(rend, grid[i], ncols);
	for (j = 0; j < ncols; j++)
	    SDL_RenderDrawLine(rend,
			       grid[i][j].x, grid[i][j].y,
			       grid[i+1][j].x, grid[i+1][j].y);
    }
    SDL_RenderDrawLines(rend, grid[i], ncols);
    SDL_RenderPresent(rend);
}

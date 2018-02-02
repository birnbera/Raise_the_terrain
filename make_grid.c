#include "terrain.h"

/**
 * allocate_grid - Allocate nrows x ncols of SDL_Point structs
 * to store window coordinates of points to be rendered.
 * @nrows: number of rows to malloc
 * @ncols: number of columns to malloc
 *
 * Return: 2d array of SDL_Point structs, or NULL on failure
 */
SDL_Point **allocate_grid(size_t nrows, size_t ncols)
{
    SDL_Point **grid;
    size_t i;

	grid = malloc(sizeof(SDL_Point *) * nrows);
	if (!grid)
	{
		perror("Could not malloc space for SDL_Point rows");
		return (NULL);
	}
	for (i = 0; i < nrows; i++)
	{
		grid[i] = calloc(ncols, sizeof(SDL_Point));
		if (!(grid[i]))
		{
			perror("Could not malloc space for SDL_Point columns");
			free_grid(grid, i);
			return (NULL);
		}
	}
	return grid;
}

/**
 * free_grid - routine to free mallocated 2d array of SDL_Point structs
 * @grid: pointer to array to free
 * @i: number of rows to free
 */
void free_grid(SDL_Point **grid, size_t i)
{
	if (grid == NULL || *grid == NULL)
		return;
	while (i)
		free(grid[i--]);
	free(grid);
}

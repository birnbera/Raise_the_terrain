#include "terrain.h"

#define DELIMS "\n\r\t ,;"
#define TRUE 1
#define FALSE 0

/**
 * read_altitudes - Open file and parse into 2d array of elevations
 * @fname: name of file to open and read
 * @saverows: variable passed into function call to hold the
 * y-dimension from `fname`.
 * @savecols: variable passed into function call to hold the
 * x-dimension from `fname`.
 *
 * Return: pointer to heap-allocated 2d array of type `float3d_t` with each
 * struct's `z` member initialized from `fname`.
 */
float3d_t **read_altitudes(char *fname, size_t *saverows, size_t *savecols)
{
	FILE *altitudes;
	float3d_t **coords;
	char *line, *token, *saveptr;
	size_t n, i, nrows, ncols, maxcols;
	unsigned char uneven;

	altitudes = fopen(fname, "r");
	if (!altitudes)
	{
		perror("Failed to open file in read_altitudes");
		exit(EXIT_FAILURE);
	}
	uneven = FALSE;
	maxcols = 0;
	nrows = 0;
	line = NULL;
	while (getline(&line, &n, altitudes) != -1)
	{
		nrows++;
		token = strtok_r(line, DELIMS, &saveptr);
		for (ncols = 0; token != NULL; ncols++)
			token = strtok_r(NULL, DELIMS, &saveptr);
		if (ncols > maxcols)
		{
			uneven = TRUE;
			maxcols = ncols;
		}
	}
	coords = allocate_coords(nrows, ncols);
	if (!coords)
	{
		free(line);
		fclose(altitudes);
		exit(EXIT_FAILURE);
	}
	if (uneven)
		fprintf(stderr,
			"File [%s]: Uneven rows: padding with zeros...\n",
			fname);
	rewind(altitudes);
	row = 0;
	while (getline(&line, &n, altitudes) != -1)
	{
		token = strtok_r(line, DELIMS, &saveptr);
		col = 0;
		while (token)
		{
			coords[row][col].z = atof(token);
			token = strtok_r(NULL, DELIMS, &saveptr);
			col++;
		}
		row++;
	}
	free(line);
	fclose(altitudes);
	*saverows = nrows;
	*savecols = ncols;
	return (coords);
}

/**
 * allocate_coords - Allocate `nrows`x`ncols` array of `float3d_t` structs
 * @nrows: number of rows to malloc
 * @ncols: number of columns to malloc
 *
 * Return: 2d array of `float3d_t` structs, or NULL on failure
 */
float3d_t **allocate_coords(size_t nrows, size_t ncols)
{
	float3d_t **coords;
	size_t i;

	coords = malloc(size_of(float3d_t *) * nrows);
	if (!coords)
	{
		perror("Could not malloc space for altitudes' rows");
		return (NULL);
	}
	for (i = 0; i < nrows; i++)
	{
		coords[i] = calloc(ncols, size_of(float3d_t));
		if (!(coords[i]))
		{
			perror("Could not malloc space for altitudes' columns");
			free_coords(coords, i);
			return (NULL);
		}
	}
}

/**
 * free_coords - routine to free mallocated 2d array of `float3d_t` structs
 * @coords: pointer to array to free
 * @i: number of rows to free
 */
void free_coords(float3d_t **coords, size_t i);
{
	if (coords == NULL || *coords == NULL)
		return;
	while (i)
		free(coords[i--]);
	free(coords);
}

#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define XGRID 8
#define YGRID 8

void initGrid(SDL_Point (*grid)[XGRID][YGRID], size_t xgrid, size_t ygrid);
void scaleGrid(SDL_Point (*grid)[XGRID][YGRID], size_t xgrid, size_t ygrid);
void drawGrid(SDL_Renderer *renderer, SDL_Point (*grid)[XGRID][YGRID], size_t xgrid, size_t ygrid);
int getIsoX(float inclination, int x, int y);
int getIsoY(float inclination, int x, int y, int z);

FILE *altitudes;

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event e;
    SDL_Point grid[XGRID][YGRID];
    int quit = 0;

    if (argc < 2)
    {
	printf("Usage: %s altitudes\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    altitudes = fopen(argv[1], "r");
    if (SDL_Init(SDL_INIT_VIDEO))
	printf("Could not initialize SDL: %s", SDL_GetError());
    else
    {
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
					0, &window, &renderer))
	    printf("Could not create window and/or renderer: %s",
		   SDL_GetError());
	else
	{
	    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	    SDL_RenderClear(renderer);
	    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	    initGrid(&grid, XGRID, YGRID);
	    scaleGrid(&grid, XGRID, YGRID);
	    drawGrid(renderer, &grid, XGRID, YGRID);
	    SDL_RenderPresent(renderer);
	    while(!quit)
	    {
		while(SDL_PollEvent(&e))
		{
		    if (e.type == SDL_QUIT)
			quit = SDL_TRUE;
		    else if (e.key.keysym.sym == SDLK_ESCAPE)
			quit = 1;
		}
	    }
	    if (renderer)
		SDL_DestroyRenderer(renderer);
	    if (window)
		SDL_DestroyWindow(window);
	}
    }
    SDL_Quit();
    fclose(altitudes);
    return (0);
}

void initGrid(SDL_Point (*grid)[XGRID][XGRID], size_t xgrid, size_t ygrid)
{
    size_t x, y, n;
    int xpos, ypos, zpos, xstep, ystep;
    char *lineptr = NULL, *alt;

    /*xstep = SCREEN_WIDTH / (xgrid + 1);
      ystep = SCREEN_HEIGHT / (ygrid + 1);*/
    for (x = xpos = 0; x < xgrid; x++, xpos++)
    {
	if (getline(&lineptr, &n, altitudes) < 0)
	    break;
	alt = strtok(lineptr, "\n\t\r ");
	for (y = ypos = 0; y < ygrid; y++, ypos++)
	{
	    zpos = atoi(alt);
	    (*grid)[x][y].x = getIsoX(0.7, xpos, ypos);
	    (*grid)[x][y].y = getIsoY(0.7, xpos, ypos, zpos);
	    alt = strtok(NULL, "\n\t\r ");
	}
    }
    if (lineptr)
	free(lineptr);
}

void scaleGrid(SDL_Point (*grid)[XGRID][XGRID], size_t xgrid, size_t ygrid)
{
    int ymax, ymin, xmax, xmin;
    int xscale, xbias, yscale, ybias;
    size_t x, y;
    SDL_Point point;

    xmin = xmax = (*grid)[0][0].x;
    ymin = ymax = (*grid)[0][0].y;
    for (x = 0; x < xgrid; x++)
    {
	for (y = 0; y < ygrid; y++)
	{
	    point = (*grid)[x][y];
	    if (point.x < xmin)
		xmin = point.x;
	    if (point.x > xmax)
		xmax = point.x;
	    if (point.y < ymin)
		ymin = point.y;
	    if (point.y > ymax)
		ymax = point.y;
	}
    }
    xscale = (SCREEN_WIDTH - 50) / (xmax - xmin);
    xbias = -xmin + 25;
    yscale = (SCREEN_HEIGHT - 50) / (ymax - ymin);
    ybias = -ymin + 25;
    for (x = 0; x < xgrid; x++)
    {
	for (y = 0; y < ygrid; y++)
	{
	    (*grid)[x][y].x = ((*grid)[x][y].x - xmin) * xscale + xbias;
	    (*grid)[x][y].y = ((*grid)[x][y].y - ymin) * yscale + ybias;
	}
    }
}

void drawGrid(SDL_Renderer *renderer, SDL_Point (*grid)[XGRID][YGRID], size_t xgrid, size_t ygrid)
{
    size_t x, y;

    for (x = 0; x < xgrid-1; x++)
    {
	SDL_RenderDrawLines(renderer, (*grid)[x], xgrid);
	for (y = 0; y < ygrid; y++)
	    SDL_RenderDrawLine(renderer,
			       (*grid)[x][y].x, (*grid)[x][y].y,
			       (*grid)[x+1][y].x, (*grid)[x+1][y].y);
    }
    SDL_RenderDrawLines(renderer, (*grid)[x], xgrid);
}

int getIsoX(float inclination, int x, int y)
{
    return ((int) (lroundf(inclination * x) - lroundf(inclination * y)));
}

int getIsoY(float inclination, int x, int y, int z)
{
    return ((int) (lroundf((1 - inclination) * x) + lroundf((1 - inclination) * y) - z));
}

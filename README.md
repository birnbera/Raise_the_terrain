# Raise the terrain

This goal of this project was to create a rotatable 3D grid as shown below using the C graphics library SDL2.

<p align="center"><img text="3D grid in SDL2" src="https://github.com/birnbera/Raise_the_terrain/blob/master/ezgif.com-gif-maker.gif"></p>

### Compilation

The source has only been tested on Linux Ubuntu 16.04 with GCC 5.4.0. SDL2-based programs seem to fail when running on Ubuntu 14.04 due to some video driver incompatibilities.
Compilation requires linking to the SDL2 library as well as the math library.
To install SDL2 on Ubuntu:
```
sudo apt-get install libsdl2-2.0.0
```
This installs the library as well as a helper program `sdl2-config` that will generate the correct options for `gcc` to link with.
To compile the source code:
```
gcc terrain.c -lm $(sdl2-config --cflags --libs) -o terrain
```
Which will generate the executable `terrain`.

### Running `terrain`

The program must be run with an argument that links to a text file containing an 8x8 array of altitudes to use for each vertex of the grid. The number of rows and columns must be equal. If you wish to use a different dimension for your grid you can change value of `GRD_SZ` in `terrain.h` and recompile.
Example altitude file:
```
120 60 40 60 20 -20 -80 -120
40 20 30 30 -10 -40 -90 -110
20 30 10 06 -6 -20 -26 -90
00 -6 10 10 -6 -20 -20 -40
-20 -20 -18 -14 -40 -20 -20 -30
-10 -10 -10 -10 -8 -20 -20 -30
20 10 10 10 10 04 10 -10
30 24 24 22 20 18 14 16
```

# Raytracing

A 3D raytracing engine written in C++.
Currently in developement.
For now, the engine only rasterizes trianges and creates depth buffer.

The code code is written for Linux, but if you compile it, it should work on windows as well.

## TODO
* Fix a bug with random lines being shown
* Bouncing rays of triangles
* ~~Parallelize rendering~~
* ~~Check if a certain pixel is in a shadow~~

## Dependencies
* SDL2

## Compiling
Before you start compiling, make sure you have SDL2 library installed.

* If you want to compile with g++ compiler on Linux, everything is already in `compile.sh` file.

## List of all the files
* .gitignore - A gitignore file
* compile.sh - A Linux shell script that compiles the whole project
* main.cpp - The file with main function, that then includes classes and functions from raytracing.cpp file
* raytracing.cpp - The file with most of the code necessary
* raytracing.h - raytracing.cpp header file
* README.md - readme file

## How to create your own scene

A scene is a simple file in which there is information about all the triangles in the scene.

A scene file can handle three different instructions
* `tr` - render a triangle. An example of such instruction would be `tr 0 0 100 100 0 100 50 50 150 1 0 0`. After the instruction, there are 12 float numbers, that tell the parser: x, y, z coordinates of all three sides of triangle and r g b values for the color of triangle (values should be between 0 and 1)
* `lo` - load another scene. An example would be `lo 200 0 0 scenes/pyramid.scenes`. This tells the parser, to load scene `scenes/pyramid.scenes` and move it 200 units in x direction and 0 units in y and z directions. The three numbers can be float numbers as well.
* `ls` - Add a light source. The following three floats tell its position, and the fourth float tells its brightness. Example: `ls 300 500 20 1`
* `#` - Indicates a comment. Any line that does not have `tr`, `lo` or `ls` command in the beginning will be ignored, but I recommend you still use `#` as comments.

## How does it look?

The first image is a rendered default scene and the second one is visualized depth buffer.
![Rendered scene](https://i.imgur.com/2sYxYl3.png)
![Depth buffer](https://i.imgur.com/dpidazm.png)

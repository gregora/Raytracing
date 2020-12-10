# Raytracing

A 3D raytracing engine written in C++.
Currently in developement.

The code code is written for Linux, but if you compile it, it should work on windows as well.

## Dependencies
* SDL2


## How to create your own scene

A scene is a simple file in which there is information about all the triangles in the scene.

A scene file can handle three different instructions
* `tr` - render a triangle. An example of such instruction would be `tr 0 0 100 100 0 100 50 50 150 1 0 0`. After the instruction, there are 12 float numbers, that tell the parser: x, y, z coordinates of all three sides of triangle and r g b values for the color of triangle (values should be between 0 and 1)
* `lo` - load another scene. An example would be `lo 200 0 0 scenes/pyramid.sc`. This tells the parser, to load scene `scenes/pyramid.sc` and move it 200 units in x direction and 0 units in y and z directions.
* `#` - Indicates a comment. Any line that does not have `tr` or `lo` command in the beginning will be ignored, but I recommend you still use `#` as comments.

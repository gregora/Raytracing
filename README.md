# Raytracing

A 3D raytracing engine written in C++.
Currently in developement.


## How to create your own scene

A scene is a simple file in which there is information about all the triangles in the scene.

The format is simple. Each line represents a triangle and has 12 floating point numbers separated by a space.

First nine lines are for coordinates ```x y z``` of each vertex and the last three are for triangle's color ```red green blue```.
The color values should be between 0 and 1.

A white triangle would look something like this:
```-150 80 0 -150 -80 0 -150 0 50 1 1 1```

#include <vector>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "math.h"
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>


const float deg2rad = 0.017452778;

class Vector {

  public:
    float x = 0.01f;
    float y = 0.01f;
    float z = 0.01f;

    Vector(float setx = 0, float sety = 0, float setz = 0);

    Vector operator* (float n);

    Vector operator+ (Vector v);

    Vector operator- (Vector v);

    Vector operator*(Vector v);

    void Output();

    float Length();

    float ScalarProjectionOf(Vector v);

    void Normalize();

};

class Triangle {


  public:
    Vector position;
    Vector a;
    Vector b;

    float red;
    float green;
    float blue;

    Triangle(float* points);

    Vector Normal();

    float GetRayPlaneIntersection(Vector ray, Vector ray_position);

    float RayHitsTriangle(Vector ray, Vector ray_position);

    void SetColor(float r, float g, float b);

};


float DotProduct(Vector v1, Vector v2);


class Frame {

  public:
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    float fov = 90;

    float *** frame = nullptr;
    float ** depth_buffer = nullptr;

    Vector camera_position;
    float ray_length = 1000;

    int width;
    int height;
    std::vector<Triangle *> triangles;


    SDL_Window *window;
    SDL_Renderer *renderer;

    Frame(const int setwidth, const int setheight);

    void Render();

    void ToScreen(float * (*function)(Frame *, int x, int y) = nullptr);

    Vector GetCameraDirection();

    void CreateWindow(char * title);

    void Load(std::string file, float movex = 0, float movey = 0, float movez = 0);

  private:
    Vector camera_direction;


};

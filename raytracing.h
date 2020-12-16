#include "math.h"
#include <vector>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <thread>


const float deg2rad = 0.017452778;
const unsigned int cpu_num = std::thread::hardware_concurrency();

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

class LightSource {

  public:
    Vector position;
    float brightness = 1;

    LightSource(float x, float y, float z, float brightness = 1);

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
    std::vector<LightSource *> light_sources;

    float ambient_light = 0;
    float light_dumping_coefficient = 0.2;

    SDL_Window *window;
    SDL_Renderer *renderer;

    Frame(const int setwidth, const int setheight);

    void Render();

    static void RenderPart(Frame * frame, int from_width, int to_width, Vector sphere_tangent1, Vector sphere_tangent2);

    static float * GetPixelColor(Frame * frame, Vector ray, int reflections);

    void ToScreen(float * (*function)(Frame *, int x, int y) = nullptr);

    Vector GetCameraDirection();

    void CreateWindow(char * title);

    void Load(std::string file, float movex = 0, float movey = 0, float movez = 0);

  private:
    Vector camera_direction;


};

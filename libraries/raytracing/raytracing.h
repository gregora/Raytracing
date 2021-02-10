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
#include "../lodepng/lodepng.h"


const float deg2rad = 0.017452778;
const unsigned int cpu_num = std::thread::hardware_concurrency();

class Vector {

  public:
    float x = 0;
    float y = 0;
    float z = 0;

    Vector(float x = 0, float y = 0, float z = 0);

    Vector operator* (float n);

    Vector operator+ (Vector v);

    Vector operator- (Vector v);

    Vector operator*(Vector v);

    void Output();

    float Length();

    float LengthSquared();

    float ScalarProjectionOf(Vector v);

    void Normalize();

};


class Texture {

  public:
    int height;
    int width;
    float *** texture = nullptr;

    Texture(std::string file);

};

class Triangle {


  public:
    Vector position;
    Vector a;
    Vector b;

    float red;
    float green;
    float blue;

    Texture* texture = nullptr;

    float reflectiveness = 0;

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

float AngleBetweenVectors(Vector v1, Vector v2);

//a faster algorithm to approximate 1/sqrt(number)
float Q_rsqrt(float number);


class Frame {

  public:
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    float fov = 90;

    float *** frame = nullptr;
    float ** depth_buffer = nullptr;
    float ** brightness_buffer = nullptr;
    float max_brightness;

    Vector camera_position;
    float ray_length = 1000;

    int width;
    int height;

    std::vector<Triangle *> triangles;
    std::vector<LightSource *> light_sources;

    float ambient_light = 0;
    float sky_brightness = 1;
    float light_dumping_coefficient = 0.2;

    float sky_red = 0;
    float sky_green = 0;
    float sky_blue = 0;

    int reflections_number = 1;

    SDL_Window *window;
    SDL_Renderer *renderer;

    Frame(const int setwidth, const int setheight);

    void SetSkyColor(float red, float green, float blue);

    void Render();

    static void RenderPart(Frame * frame, int from_width, int to_width, Vector sphere_tangent1, Vector sphere_tangent2);

    static float * GetPixelColor(Frame * frame, Vector ray, Vector ray_position, int reflections, int avoid_triangle = -1); //there has to be an avoid_triangle argument, for reflection to not treat triangle itself as an obsticle

    void ToScreen(float * (*function)(Frame *, int x, int y) = nullptr);

    Vector GetCameraDirection();

    void CreateWindow(char * title);

    void Load(std::string file, float movex = 0, float movey = 0, float movez = 0);

    void SaveAsPng(std::string file);

  private:
    Vector camera_direction;

};


std::vector<std::string> SplitString(std::string string, std::string split_by);

#include <vector>

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

    bool RayHitsTriangle(Vector ray, Vector ray_position);


};


float DotProduct(Vector v1, Vector v2);


class Frame {

  public:
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    float fov = 90;

    float *** frame = nullptr;

    Vector camera_position;
    float ray_length = 1000;

    int width;
    int height;
    std::vector<Triangle *> triangles;

    Frame(const int setwidth, const int setheight);

    void Render();

    void Debug(bool show_middle = true);

    Vector GetCameraDirection();


  private:
    Vector camera_direction;


};


class Vector {

  public:
    float x = 0;
    float y = 0;
    float z = 0;

    Vector(float setx = 0, float sety = 0, float setz = 0);

    Vector operator* (float n);

    Vector operator+ (Vector v);

    Vector operator- (Vector v);

    Vector operator*(Vector v);

    void Output();

    float Length();

    float ScalarProjectionOf(Vector v);

};

class Triangle {


  public:
    Vector position;
    Vector a;
    Vector b;

    Triangle(float* points);

    Vector Normal();

    float GetRayPlaneIntersection(Vector ray, Vector ray_position);

    bool RayHitsTriangle(Vector ray, Vector ray_position);

};


float DotProduct(Vector v1, Vector v2);

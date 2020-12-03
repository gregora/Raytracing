#include "raytracing.h"
#include <iostream>
#include "math.h"

Vector::Vector(float setx, float sety, float setz){

  x = setx;
  y = sety;
  z = setz;

  if(x == 0) x = 0.01f;
  if(y == 0) y = 0.01f;
  if(x == 0) x = 0.01f;
  if(y == 0) y = 0.01f;
}

Vector Vector::operator* (float n){

  return Vector(n*x, n*y, n*z);

}

Vector Vector::operator*(Vector v){
  return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

Vector Vector::operator+ (Vector v){
  return Vector(v.x+x, v.y+y, v.z+z);
}

Vector Vector::operator- (Vector v){
  return Vector(x-v.x, y-v.y, z-v.z);
}

void Vector::Output(){
  std::cout << "(" << x << ", " << y << ", " << z << ")\n";
}

float Vector::Length(){
  return sqrt(x*x + y*y + z*z);
}

float Vector::ScalarProjectionOf(Vector v){
    return DotProduct(Vector(x, y, z), v) / (Length() * Length());
}

Triangle::Triangle(float* points){

  position.x = points[0];
  position.y = points[1];
  position.z = points[2];

  a = Vector(points[3], points[4], points[5]) - position;
  b = Vector(points[6], points[7], points[8]) - position;

}

Vector Triangle::Normal(){

  return a*b;

}

float Triangle::GetRayPlaneIntersection(Vector ray, Vector ray_position){

  float rdotn = DotProduct(Normal(), ray);
  return DotProduct(Normal(), position - ray_position) / rdotn;

}


bool Triangle::RayHitsTriangle(Vector ray, Vector ray_position){

  float k = GetRayPlaneIntersection(ray, ray_position);

  if(k > 1 || k < 0) return false;

  //////DOES NOT WORK

  Vector point_of_intersection;
  point_of_intersection = ray_position + ray*k - position;

  float m = (point_of_intersection.y*a.x - point_of_intersection.x*a.y) / (b.y*a.x - b.x*a.y);
  float n = (point_of_intersection.x - m*b.x)/ a.x;

  //point_of_intersection.Output();
  //(a*n + b*m).Output();

  if(m < 0 || m > 1) return false;
  if(n < 0 || n > 1) return false;

  if(n + m > 1) return false;

  return true;
}


float DotProduct(Vector v1, Vector v2){

  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

}

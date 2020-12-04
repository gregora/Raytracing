#include "raytracing.h"
#include <iostream>
#include "math.h"
#include <vector>

Vector::Vector(float setx, float sety, float setz){

  x = setx;
  y = sety;
  z = setz;

  if(x == 0) x = 0.01f;
  if(y == 0) y = 0.01f;
  if(z == 0) z = 0.01f;

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

  Vector point_of_intersection;
  point_of_intersection = ray_position + ray*k - position;

  float m = (point_of_intersection.y*a.x - point_of_intersection.x*a.y) / (b.y*a.x - b.x*a.y);
  if(m < 0 || m > 1) return false;

  float n = (point_of_intersection.x - m*b.x)/ a.x;
  if(n < 0 || n > 1) return false;

  if(n + m > 1) return false;

  return true;
}



float DotProduct(Vector v1, Vector v2){

  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

}


Frame::Frame(const int setwidth, const int setheight){

  width = setwidth;
  height = setheight;

  //create 3D array
  frame = new float** [3];
  for(int i = 0; i < 3; i++){
    frame[i] = new float* [width];
    for(int j = 0; j < width; j++){
      frame[i][j] = new float[height];
    }
  }

}


void Frame::Render(){

  GetCameraDirection();

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      bool hits = false;

      Vector current_ray = camera_direction;

      for(int m = 0; m < triangles.size(); m++){
        if(triangles[m] -> RayHitsTriangle(current_ray, camera_position)){
          hits = true;
        }
      }

      if(hits){
        frame[0][i][j] = 1;
        frame[1][i][j] = 1;
        frame[2][i][j] = 1;
      }else{
        frame[0][i][j] = 0;
        frame[1][i][j] = 0;
        frame[2][i][j] = 0;
      }

    }
  }

}

void Frame::Debug(){

  for(int j = 0; j < height; j++){
    for(int i = 0; i < width; i++){

      if(frame[0][i][j] == 1){
        std::cout << "0";
      }else{
        std::cout << " ";
      }

    }

    std::cout << "\n";
  }


}


Vector Frame::GetCameraDirection(){

  camera_direction = Vector(ray_length*cos(yaw*0.017452778)*cos(pitch*0.017452778), ray_length*sin(yaw*0.017452778)*cos(pitch*0.017452778), ray_length*sin(pitch*0.017452778));
  return camera_direction;

}

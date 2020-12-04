#include <iostream>
#include "raytracing.h"

using namespace std;

int main(){

  Frame frame(70, 20);

  float triangle_points[9] = {30, 80, 0, 30, -80, 0, 30, 0, 50};
  Triangle t(triangle_points);
  float triangle_points2[9] = {30, 80, 5, 30, 70, 5, 30, 75, 20};
  Triangle t2(triangle_points2);

  frame.triangles.push_back(&t);
  frame.triangles.push_back(&t2);


  frame.Render();
  frame.Debug();

}

#include <iostream>
#include <unistd.h>
#include "raytracing.h"

using namespace std;

int main(){

  Frame frame(140, 40);

  float triangle_points[9] = {150, 80, 0, 150, -80, 0, 150, 0, 50};
  Triangle t(triangle_points);

  float triangle_points2[9] = {-150, 80, 0, -150, -80, 0, -150, 0, 50};
  Triangle t2(triangle_points2);

  frame.triangles.push_back(&t);
  frame.triangles.push_back(&t2);
  frame.camera_position.z = 25;
  frame.pitch = 0;
  frame.yaw = 0;

  for(int i = 0; i > -1; i++){

    frame.yaw = i;

    frame.Render();
    frame.Debug();

    unsigned int microsecond = 1000000;
    usleep(0.01 * microsecond);
  }

}

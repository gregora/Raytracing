#include <iostream>
#include <unistd.h>
#include "raytracing.h"

using namespace std;


float * Function(Frame * fr, int x, int y){

  int type = 0;

  float * test = new float[3];

  if(type == 0){

    test[0] = (fr -> frame[0][x][y]) / (1 + fr -> depth_buffer[x][y]);
    test[1] = (fr -> frame[1][x][y]) / (1 + fr -> depth_buffer[x][y]);
    test[2] = (fr -> frame[2][x][y]) / (1 + fr -> depth_buffer[x][y]);

    if(fr -> depth_buffer[x][y] >= 0.9){

      test[0] = 0;
      test[1] = 0.3;
      test[2] = 0.4;
    }

  }else if(type == 1){

    test[0] = 1 / (1 + fr -> depth_buffer[x][y]);
    test[1] = 1 / (1 + fr -> depth_buffer[x][y]);
    test[2] = 1 / (1 + fr -> depth_buffer[x][y]);

  }


  return test;

}

int main(){

  Frame frame(200, 200);


  float ground_points[9] = {-50000, -50000, 0, 50000, -50000, 0, 0, 50000, 0};
  Triangle ground(ground_points);
  ground.SetColor(0, 1, 0);

  float triangle_points[9] = {150, 80, 0, 150, -80, 0, 150, 0, 50};
  Triangle t(triangle_points);
  t.SetColor(1, 1, 1);


  float triangle_points3[9] = {100, 20, 0, 100, -20, 0, 100, 20, 50};
  Triangle t3(triangle_points3);
  t3.SetColor(0, 1, 1);

  float triangle_points2[9] = {-150, 80, 0, -150, -80, 0, -150, 0, 50};
  Triangle t2(triangle_points2);
  t2.SetColor(1, 0, 1);

  frame.triangles.push_back(&t);
  frame.triangles.push_back(&t2);
  frame.triangles.push_back(&t3);
  frame.triangles.push_back(&ground);
  frame.camera_position.z = 25;
  frame.pitch = 0;
  frame.yaw = 0;
  frame.roll = 0;


  frame.CreateWindow("Raytracing");

  //while(true){
    frame.Render(Function);
  //}

  for(int i = 0; i > -1; i++){

    frame.roll = i;
    std::cout << i % 360 << endl;
    frame.Render(Function);

  }

}

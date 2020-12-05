#include <iostream>
#include <unistd.h>
#include "raytracing.h"

using namespace std;


float * Function(Frame * fr, int x, int y){

  int type = 0;

  float * test = new float[3];
  float dist = fr -> depth_buffer[x][y];

  if(type == 0){

    test[0] = (fr -> frame[0][x][y]) / (1 + dist);
    test[1] = (fr -> frame[1][x][y]) / (1 + dist);
    test[2] = (fr -> frame[2][x][y]) / (1 + dist);

    if(dist >= 0.9){

      test[0] = 0;
      test[1] = 0.3;
      test[2] = 0.4;
    }

  }else if(type == 1){

    test[0] = 1 / (1 + dist);
    test[1] = 1 / (1 + dist);
    test[2] = 1 / (1 + dist);

  }


  return test;

}

int main(){

  Frame frame(200, 200);


  /*float ground_points[9] = {-50000, -50000, 0, 50000, -50000, 0, 0, 50000, 0};
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
  */

  frame.Load("scenes/scene1.sc");

  frame.camera_position.z = 25;
  frame.pitch = 0;
  frame.yaw = 0;
  frame.roll = 0;


  frame.CreateWindow("Raytracing");

  int i = 0;

  SDL_ShowCursor(SDL_DISABLE);
  SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);

  float speedz = 0;

  while(true){

    SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);

    i++;
    frame.Render(Function);

    int x;
    int y;
    speedz -= 1;


    frame.camera_position.z += speedz;
    if(frame.camera_position.z < 25) frame.camera_position.z = 25;


    SDL_GetMouseState(&x, &y);
    if(i > 50){
      frame.yaw += x - frame.height/2;
      frame.pitch -= y - frame.width/2;
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    float walking_speed = 10;

    if (keys[SDL_SCANCODE_W]){
      frame.camera_position = frame.camera_position + Vector(cos(frame.yaw * 3.14 / 180), sin(frame.yaw * 3.14 / 180), 0)*walking_speed;
    }else if(keys[SDL_SCANCODE_S]){
      frame.camera_position = frame.camera_position - Vector(cos(frame.yaw * 3.14 / 180), sin(frame.yaw * 3.14 / 180), 0)*walking_speed;
    }else if(keys[SDL_SCANCODE_A]){
      frame.camera_position = frame.camera_position - Vector(-sin(frame.yaw * 3.14 / 180), cos(frame.yaw * 3.14 / 180), 0)*walking_speed;
    }else if(keys[SDL_SCANCODE_D]){
      frame.camera_position = frame.camera_position + Vector(-sin(frame.yaw * 3.14 / 180), cos(frame.yaw * 3.14 / 180), 0)*walking_speed;
    }

    if(keys[SDL_SCANCODE_SPACE]){
      speedz = 10;
    }

    if(keys[SDL_SCANCODE_ESCAPE]){
      exit(EXIT_SUCCESS);
    }


  }

}

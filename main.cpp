#include <iostream>
#include <unistd.h>
#include "raytracing.h"

using namespace std;


float * Function(Frame * fr, int x, int y){

  int type = 0;

  float * test = new float[3];
  float dist = fr -> depth_buffer[x][y];

  if(type == 0){

    test[0] = (fr -> frame[0][x][y]) / (1 + dist*3);
    test[1] = (fr -> frame[1][x][y]) / (1 + dist*3);
    test[2] = (fr -> frame[2][x][y]) / (1 + dist*3);

    if(dist >= 0.9){
      test[0] = 0.0;
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

  bool debug  = true;
  bool enable_controls = true;

  int width, height;
  string file;

  if(!debug){

    cout << "Width: ";
    cin >> width;
    cout << "Height: ";
    cin >> height;


    cout << "Scene: ";
    cin >> file;

  }else{
    file = "scenes/scene1.sc";
    width = 100;
    height = 100;
  }

  Frame frame(width, height);



  frame.Load(file);

  frame.camera_position.z = 25;
  frame.pitch = 0;
  frame.yaw = 0;
  frame.roll = 0;

  frame.CreateWindow("Raytracing");

  int i = 0;

  SDL_ShowCursor(SDL_DISABLE);
  if(enable_controls) SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);

  float speedz = 0;

  while(true){

    if(enable_controls) SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);

    i++;
    frame.Render();
    frame.ToScreen(Function);

    int x;
    int y;
    speedz -= 1;


    frame.camera_position.z += speedz;
    if(frame.camera_position.z < 25) frame.camera_position.z = 25;


    SDL_GetMouseState(&x, &y);

    if(enable_controls){
      if(x < 500) frame.yaw += x - frame.height/2;
      if(y < 500) frame.pitch -= y - frame.width/2;
    }

    if (frame.pitch > 90 && frame.pitch < 180) frame.pitch = 90;
    if (frame.pitch > 180 && frame.pitch < 270) frame.pitch = 270;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    float walking_speed = 10;

    if(enable_controls){
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

    }

    if(keys[SDL_SCANCODE_ESCAPE]){
      exit(EXIT_SUCCESS);
    }


  }

}

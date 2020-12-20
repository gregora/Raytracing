#include <iostream>
#include <unistd.h>
#include "raytracing.h"

using namespace std;


float * PostProcessing(Frame * fr, int x, int y){

  int type = 1;

  float * pixel = new float[3];
  float dist = fr -> depth_buffer[x][y];

  if(type == 0){
    //change nothing
    pixel[0] = fr -> frame[0][x][y];
    pixel[1] = fr -> frame[1][x][y];
    pixel[2] = fr -> frame[2][x][y];

  }else if(type == 1){

    //render depth buffer
    pixel[0] = 1 / (1 + dist*2000);
    pixel[1] = 1 / (1 + dist*2000);
    pixel[2] = 1 / (1 + dist*2000);

  }


  return pixel;

}

int main(){

  bool debug  = false;
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
    file = "scenes/scene1.scene";
    width = 100;
    height = 100;
  }

  Frame frame(width, height);

  frame.Load(file);

  frame.camera_position.z = 25;
  frame.yaw = 0;
  frame.pitch = -30;
  frame.roll = 0;

  frame.ambient_light = 0.5;

  frame.ray_length = 1000000;

  frame.SetSkyColor(0.2, 0.8, 1);

  frame.CreateWindow("Raytracing");

  int i = 0;

  if(enable_controls){
    SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);
    SDL_ShowCursor(SDL_DISABLE);
  }


  float speedz = 0;

  while(true){

    i++;

    //rotate light source
    if(file == "scenes/scene1.scene") frame.light_sources[0] -> position = Vector(2000*sin(i * deg2rad), 2000*cos(i * deg2rad), 600);

    if(enable_controls) SDL_WarpMouseInWindow(frame.window, frame.width/2, frame.height/2);

    frame.Render();
    frame.ToScreen(PostProcessing);

    int x;
    int y;
    speedz -= 1;

    //add gravity
    frame.camera_position.z += speedz;

    //fix observer height to 2m
    if(frame.camera_position.z < 2) frame.camera_position.z = 2;

    SDL_GetMouseState(&x, &y);

    if(enable_controls){
      if(x < 500) frame.yaw += x - frame.width/2;
      if(y < 500) frame.pitch -= y - frame.height/2;
    }else{
      frame.pitch = -10;
      frame.camera_position.z = 300;
    }

    //max pitch
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

#include <iostream>
#include "raytracing.h"

using namespace std;

int main(){

  float triangle_points[9] = {30, 0, 80, 30, 0, -80, 30, 50, 0};
  Triangle t(triangle_points);
  float triangle_points2[9] = {30, 5, 80, 30, 5, 70, 30, 20, 75};
  Triangle t2(triangle_points2);
  Vector ray(1000, 300, 0);

  float scale = 20;

  int screen_width = 530;
  int screen_height = 170;

  for(int i = (int) (-screen_height/2); i < (int) (screen_height/2); i++){
    for(int j = (int) (-screen_width/2); j < (int) (screen_width/2); j++){
      bool curr_pix = t.RayHitsTriangle(Vector(ray.x, ray.y - i*scale, ray.z + j*scale), Vector(0, 0, 0));
      bool curr_pix2 = t2.RayHitsTriangle(Vector(ray.x, ray.y - i*scale, ray.z + j*scale), Vector(0, 0, 0));

      if(curr_pix == true || curr_pix2 == true){
        cout << "0";
      }else{
        cout << ".";
      }
    }

    cout << "\n";
  }

}

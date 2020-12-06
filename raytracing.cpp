#include "raytracing.h"
#include <iostream>
#include "math.h"
#include <vector>
#include <limits>
#include <fstream>
#include <string>
#include <algorithm>

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

void Vector::Normalize(){
    float length = Length();
    x = x / length;
    y = y / length;
    z = z / length;
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


void Triangle::SetColor(float r, float g, float b){

  red = r;
  green = g;
  blue = b;

}

float Triangle::RayHitsTriangle(Vector ray, Vector ray_position){

  float k = GetRayPlaneIntersection(ray, ray_position);

  if(k > 1 || k < 0) return -1;

  Vector point_of_intersection;
  point_of_intersection = ray_position + ray*k - position;

  float m = (point_of_intersection.y*a.x - point_of_intersection.x*a.y) / (b.y*a.x - b.x*a.y);
  if(m < 0 || m > 1) return -1;

  float n = (point_of_intersection.x - m*b.x)/ a.x;
  if(n < 0 || n > 1) return -1;

  if(n + m > 1) return -1;

  return k;
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


  //create 2D array
  depth_buffer = new float* [width];
  for(int i = 0; i < width; i++){
    depth_buffer[i] = new float[height];
  }

}


void Frame::Render(float * (*function)(Frame *, int x, int y)){

  if(yaw > 360) yaw = (int)yaw % 360;
  if(yaw < 0) yaw = 360 - (-(int)yaw % 360);
  if(pitch > 360) pitch = (int)pitch % 360;
  if(pitch < 0) pitch = 360 - (-(int)pitch % 360);
  if(roll > 360) roll = (int)roll % 360;
  if(roll < 0) roll = 360 - (-(int)roll % 360);




  GetCameraDirection();

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  Vector cam = camera_direction;

  //tangents are calculated, so that we know in what directions to shoot the rays
  float t1 = - sqrt(1 / (1 + camera_direction.x*camera_direction.x / (camera_direction.y * camera_direction.y)));
  if(yaw > 180) t1 = -t1;
  float t2 = - t1 * camera_direction.x / camera_direction.y;
  float t3  = tan(roll/180*3.14) * sqrt(t1*t1 + t2*t2);

  Vector sphere_tangent1 = Vector(t1, t2, t3);
  Vector sphere_tangent2 = sphere_tangent1 * camera_direction;

  if(roll > 90 && roll <= 270){
    sphere_tangent1 = sphere_tangent1 * (-1);
    sphere_tangent2 = sphere_tangent2 * (-1);
  }

  sphere_tangent1.Normalize();
  sphere_tangent2.Normalize();

  //calculate how far a ray should move considering fov
  float move_coeficient1 = 2 * tan(0.5*fov/180*3.14) * ray_length / width;
  float move_coeficient2 = 2 * tan(0.5*fov*height/width/180*3.14) * ray_length / height;

  sphere_tangent1 = sphere_tangent1 * move_coeficient1;
  sphere_tangent2 = sphere_tangent2 * move_coeficient2;

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      bool hits = false;
      Vector current_ray = camera_direction + sphere_tangent1*(i - width/2) + sphere_tangent2*(j - height/2);

      int triangle_num = -1;
      float min_triangle_distance = std::numeric_limits<float>::max();

      for(int m = 0; m < triangles.size(); m++){

        float triangle_distance = triangles[m] -> RayHitsTriangle(current_ray, camera_position);

        if(triangle_distance != -1){
          if(triangle_distance < min_triangle_distance){
            triangle_num = m;
            min_triangle_distance = triangle_distance;
          }
        }
      }

      if(triangle_num != -1){
        frame[0][i][j] = triangles[triangle_num] -> red;
        frame[1][i][j] = triangles[triangle_num] -> green;
        frame[2][i][j] = triangles[triangle_num] -> blue;

        depth_buffer[i][j] = min_triangle_distance;

      }else{
        frame[0][i][j] = 0;
        frame[1][i][j] = 0;
        frame[2][i][j] = 0;

        depth_buffer[i][j] = std::numeric_limits<float>::max();
      }



      if(function != nullptr){
        float * a = function(this, i, j);

        frame[0][i][j] = a[0];
        frame[1][i][j] = a[1];
        frame[2][i][j] = a[2];

        delete[] a;

      }


      SDL_SetRenderDrawColor(renderer, frame[0][i][j]*255, frame[1][i][j]*255, frame[2][i][j]*255, 255);

      SDL_RenderDrawPoint(renderer, i, j);


    }
  }

  SDL_RenderPresent(renderer);


  //close on x
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT){
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_SUCCESS);
      };
  }


}

Vector Frame::GetCameraDirection(){

  camera_direction = Vector(ray_length*cos(yaw*0.017452778)*cos(pitch*0.017452778), ray_length*sin(yaw*0.017452778)*cos(pitch*0.017452778), ray_length*sin(pitch*0.017452778));
  return camera_direction;

}

void Frame::CreateWindow(char * title){

  SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
  SDL_SetWindowTitle(window, title);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

}


void Frame::Load(std::string file, float movex, float movey, float movez){

  int lines = 0;
  std::string line;
  std::ifstream myfile(file);


  if (myfile.is_open()){
    while (std::getline (myfile, line)){
      lines++;//count the number of lines
    }
  }else{
    std::cout << "Unable to open file " << file << "\n";
  }

  std::ifstream myfile2(file);

  int i = 0;
  while (std::getline (myfile2, line)){


    if(line[0] == '#'){
      //ignore comments
    }else if(line[0] == 't' && line[1] == 'r') {
      //add triangles

      line = line.substr(3, line.size() - 1);

      float triangle[9];

      float red, green, blue;

      for(int j = 0; j < 12; j++){
        int previous_space = 0;
        int spacepos = line.find(" ");

        float coord = std::stof(line.substr(previous_space, spacepos));

        if(j < 9){
          //move for desired transform
          if(j % 3 == 0){
            coord = coord + movex;
          }else if((j - 1) % 3 == 0){
            coord = coord + movey;
          }else if((j - 2) % 3 == 0){
            coord = coord + movez;
          }

          triangle[j] = coord;

        }else if(j == 9){
          red = coord;
        }else if(j == 10){
          green = coord;
        }else if(j = 11){
          blue = coord;
        }

        previous_space = spacepos;

        line = line.substr(previous_space + 1, line.size());
      }

      Triangle* t = new Triangle(triangle);
      t -> SetColor(red, green, blue);
      triangles.push_back(t);

    }else if(line[0] == 'l' && line[1] == 'o'){

      line = line.substr(3, line.size());
      float x = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float y = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float z = std::stof(line.substr(0, line.find(" ")));

      Load(line.substr(line.find(" ") + 1, line.size()), x + movex, y + movey, z + movez);

    }

    i++;
  }

}

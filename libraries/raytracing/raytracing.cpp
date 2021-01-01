#include "raytracing.h"

Vector::Vector(float x, float y, float z){

  this -> x = x;
  this -> y = y;
  this -> z = z;

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

float Vector::LengthSquared(){
  return x*x + y*y + z*z;
}

float Vector::ScalarProjectionOf(Vector v){
  return DotProduct(Vector(x, y, z), v) / LengthSquared();
}

void Vector::Normalize(){
  float length_squared = LengthSquared();

  x = x * Q_rsqrt(length_squared);
  y = y * Q_rsqrt(length_squared);
  z = z * Q_rsqrt(length_squared);
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

//intersection between the plane of a triangle and the given ray
float Triangle::GetRayPlaneIntersection(Vector ray, Vector ray_position){

  float rdotn = DotProduct(Normal(), ray);

  if(rdotn == 0) return -1;
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
  Vector p = point_of_intersection;

  float m,n;

  //solve system of linear equations for m and n
  if(a.x != 0){

    if(a.x*b.y - a.y*b.x != 0){
      m = (p.y*a.x - p.x*a.y) / (b.y*a.x - b.x*a.y);
    }else{
      m = (a.x*p.z - a.z*p.x) / (a.x*b.z - a.z*b.x);
    }

    if(m < 0 || m > 1) return -1;
    n = (p.x - m*b.x)/ a.x;
  }else if(a.y != 0){

    if(a.y*b.x - a.x*b.y != 0){
      m = (a.y*p.x - a.x*p.y) / (a.y*b.x - a.x*b.y);
    }else{
      m = (a.y*p.z - a.z*p.y) / (a.y*b.z - a.z*b.y);
    }

    if(m < 0 || m > 1) return -1;
    n = (p.y - b.y*m)/a.y;
  }else{

    if(a.z*b.x - a.x*b.z != 0){
      m = (a.z*p.x - a.x*p.z) / (a.z*b.x - a.x*b.z);
    }else{
      m = (a.z*p.y - a.y*p.z) / (a.z*b.y - a.y*b.z);
    }

    if(m < 0 || m > 1) return -1;
    n = (p.z - b.z*m)/a.z;

  }

  if(n < 0 || n > 1) return -1;
  if(n + m > 1) return -1;

  return k;
}


LightSource::LightSource(float x, float y, float z, float brightness){
  position.x = x;
  position.y = y;
  position.z = z;

  this -> brightness = brightness;
}


float DotProduct(Vector v1, Vector v2){

  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;

}

float AngleBetweenVectors(Vector v1, Vector v2){
  return acos(DotProduct(v1, v2) / (v1.Length() * v2.Length()));
}


//a faster algorithm to approximate 1/sqrt(number)
float Q_rsqrt(float number){

	const float x2 = number * 0.5F;
	const float threehalfs = 1.5F;

	union {
		float f;
		uint32_t i;
	} conv  = { .f = number };
	conv.i  = 0x5f3759df - ( conv.i >> 1 );
	conv.f  *= threehalfs - ( x2 * conv.f * conv.f );
	return conv.f;
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

  //create 2D array
  brightness_buffer = new float* [width];
  for(int i = 0; i < width; i++){
    brightness_buffer[i] = new float[height];
  }

}

void Frame::SetSkyColor(float red, float green, float blue){
  sky_red = red;
  sky_green = green;
  sky_blue = blue;
}


void Frame::Render(){

  if(yaw > 360) yaw = (int)yaw % 360;
  if(yaw < 0) yaw = 360 - (-(int)yaw % 360);
  if(pitch > 360) pitch = (int)pitch % 360;
  if(pitch < 0) pitch = 360 - (-(int)pitch % 360);
  if(roll > 360) roll = (int)roll % 360;
  if(roll < 0) roll = 360 - (-(int)roll % 360);

  GetCameraDirection();


  Vector cam = camera_direction;

  //tangents are calculated, so that we know in what directions to shoot the rays
  if(camera_direction.y == 0) camera_direction.y = 0.01f;
  float t1 = - Q_rsqrt(1 + camera_direction.x*camera_direction.x / (camera_direction.y * camera_direction.y));
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


  int screen_width_per_cpu = width / cpu_num;
  std::thread * threads[cpu_num];

  max_brightness = -1;

  //dispatch threads to render parts of screen
  for(int cpu = 0; cpu < cpu_num; cpu++){

    //due to width being integer divided by cpu_num also being integer, the last portion of the screen is a few pixels wider
    if(cpu + 1 != cpu_num){
      std::thread * t = new std::thread(RenderPart, this, screen_width_per_cpu*cpu, screen_width_per_cpu*(cpu + 1), sphere_tangent1, sphere_tangent2);
      threads[cpu] = t;
    }else{
      std::thread * t = new std::thread(RenderPart, this, screen_width_per_cpu*cpu, width, sphere_tangent1, sphere_tangent2);
      threads[cpu] = t;
    }

  }

  //wait for all the threads to finish
  for(int cpu = 0; cpu < cpu_num; cpu++){
    threads[cpu] -> join();
  }

}


//render a part of the frame from given width to given width
void Frame::RenderPart(Frame * frame, int from_width, int to_width, Vector sphere_tangent1, Vector sphere_tangent2){

  for(int i = from_width; i < to_width; i++){
    for(int j = 0; j < frame -> height; j++){

      //calculate current ray
      Vector current_ray = frame -> camera_direction + sphere_tangent1*(i - frame -> width/2) + sphere_tangent2*(j - frame -> height/2);

      float * colors = GetPixelColor(frame, current_ray, frame -> camera_position, frame -> reflections_number + 1);

      frame -> frame[0][i][j] = colors[0];
      frame -> frame[1][i][j] = colors[1];
      frame -> frame[2][i][j] = colors[2];

      frame -> depth_buffer[i][j] = colors[3];

      frame -> brightness_buffer[i][j] = colors[4];

      //set max brightness of a frame
      if(frame -> max_brightness == -1 || frame -> max_brightness < colors[4]){
        frame -> max_brightness = colors[4];
      }

      delete(colors);

    }
  }
}



float * Frame::GetPixelColor(Frame * frame, Vector ray, Vector ray_position, int reflections, int avoid_triangle){

  float * return_colors = new float[5]; //return r,g,b and depth

  bool hits = false;

  int triangle_num = -1;
  float min_triangle_distance = std::numeric_limits<float>::max();

  //iterate through all triangles
  for(int m = 0; m < frame -> triangles.size(); m++){
    if(m != avoid_triangle){
      //actually cast the ray
      float triangle_distance = frame -> triangles[m] -> RayHitsTriangle(ray, ray_position);

      if(triangle_distance != -1){
        if(triangle_distance < min_triangle_distance){
          triangle_num = m;
          min_triangle_distance = triangle_distance;
        }
      }
    }
  }

  //handle if there was no collision
  if(triangle_num != -1){

    Triangle t = *(frame -> triangles[triangle_num]);

    //get r,g,b values for pixel
    float r = t.red;
    float g = t.green;
    float b = t.blue;

    Vector collision_position;
    collision_position = ray*min_triangle_distance + (ray_position);


    //calculate if the pixel is in shadow
    float combined_light_power = frame -> ambient_light + frame -> sky_brightness; //how much light even is there in the scene?
    float received_light_power = frame -> ambient_light;  //how much light did the pixel get?

    for(int light = 0; light < frame -> light_sources.size(); light ++){

      Vector triangle_normal = t.Normal();

      combined_light_power += (frame->light_sources[light] -> brightness);

      Vector ray_to_light_source;
      ray_to_light_source = (*(frame -> light_sources[light])).position - collision_position;

      float projection1 = triangle_normal.ScalarProjectionOf(ray);
      float projection2 = triangle_normal.ScalarProjectionOf(ray_to_light_source);


      if(projection1*projection2 > 0){

      }else{

        //iterate through all the triangles
        bool is_covered = false;

        for(int m = 0; m < frame -> triangles.size(); m++){
          if(m != triangle_num){

            //actually cast the ray
            float triangle_distance = frame -> triangles[m] -> RayHitsTriangle(ray_to_light_source, collision_position);

            //if pixel is obscured by a triangle, no light from the light source will hit it
            if(triangle_distance != -1){
              is_covered = true;
              break;
            }

          }
        }

        //if pixel is not obscured, calculate its brightness
        if (!is_covered){
          float bounce_ray_distance = ray_to_light_source.Length() + (collision_position - ray_position).Length(); //calculate distance from the observer
          float dumping_coef = frame -> light_dumping_coefficient; //get dumping_coef

          float ang_t_ls = AngleBetweenVectors(ray_to_light_source, triangle_normal); //angle between triangle normal and light source

          ang_t_ls = abs(ang_t_ls);
          if(3.14 - ang_t_ls < ang_t_ls) ang_t_ls = 3.14 - ang_t_ls;

          bounce_ray_distance = bounce_ray_distance * bounce_ray_distance; //inverse square law for light intensity
          bounce_ray_distance =  bounce_ray_distance * dumping_coef/1000000; //apply dumping coefficient

          received_light_power += (frame -> light_sources[light] -> brightness) / (1 + bounce_ray_distance) / (1 + ang_t_ls); //calculate actual intensity
        }

      }
    }

    //calculate pixel color from its brightness
    if(combined_light_power != 0){

      return_colors[0] = r;
      return_colors[1] = g;
      return_colors[2] = b;

      return_colors[4] = received_light_power;
    }else{
      return_colors[0] = 0;
      return_colors[1] = 0;
      return_colors[2] = 0;

      return_colors[4] = 0;
    }

    return_colors[3] = min_triangle_distance;

    float triangle_reflectiveness = t.reflectiveness; //t is the triangle with which the ray collided

    //check if we need to cast another ray for reflection
    if(reflections > 1 && triangle_reflectiveness > 0){

      Vector t_normal = t.Normal();
      float k = - DotProduct(t_normal, ray)/DotProduct(t_normal, t_normal);
      Vector bounce_ray = (ray + t_normal*k)*2 - ray;

      //get reflection colors by recursively calling GetPixelColor function
      float * reflection_colors = GetPixelColor(frame, bounce_ray, collision_position, reflections - 1, triangle_num);

      //calculate the new colors
      return_colors[0] = (1 - triangle_reflectiveness)*return_colors[0] + triangle_reflectiveness*reflection_colors[0];
      return_colors[1] = (1 - triangle_reflectiveness)*return_colors[1] + triangle_reflectiveness*reflection_colors[1];
      return_colors[2] = (1 - triangle_reflectiveness)*return_colors[2] + triangle_reflectiveness*reflection_colors[2];

      return_colors[4] = (1 - triangle_reflectiveness)*return_colors[4] + triangle_reflectiveness*reflection_colors[4];

      delete(reflection_colors);
    }

  }else{
    return_colors[0] = frame -> sky_red;
    return_colors[1] = frame -> sky_green;
    return_colors[2] = frame -> sky_blue;

    return_colors[3] = std::numeric_limits<float>::max();

    return_colors[4] = frame -> sky_brightness;
  }

  return return_colors;

}


void Frame::ToScreen(float * (*function)(Frame *, int x, int y)){

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){

      if(function != nullptr){
        //call a user given post processing function, and pass it arguments
        float * a = function(this, i, j);

        frame[0][i][j] = a[0];
        frame[1][i][j] = a[1];
        frame[2][i][j] = a[2];

        delete[] a;

      }

      float pixel_brightness = brightness_buffer[i][j];

      //render pixel to screen
      SDL_SetRenderDrawColor(renderer, frame[0][i][j]*255*pixel_brightness/(1 + max_brightness), frame[1][i][j]*255*pixel_brightness/(1 + max_brightness), frame[2][i][j]*255*pixel_brightness/(1 + max_brightness), 255);
      SDL_RenderDrawPoint(renderer, i, j);

    }
  }

  //render frame
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

  //calculate camera direction ray from yaw, pitch and roll
  camera_direction = Vector(ray_length*cos(yaw*deg2rad)*cos(pitch*deg2rad), ray_length*sin(yaw*deg2rad)*cos(pitch*deg2rad), ray_length*sin(pitch*deg2rad));
  return camera_direction;

}

void Frame::CreateWindow(char * title){

  SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
  SDL_SetWindowTitle(window, title);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

}


//load a scene from a file (usually .scene)
void Frame::Load(std::string file, float movex, float movey, float movez){

  int lines = 0;
  std::string line;
  std::ifstream myfile(file);


  if (myfile.is_open()){
    while (std::getline (myfile, line)){
      lines++; //count the number of lines in the file
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
      //add triangle

      if(line[line.size() - 1] != ' '){
        line.append(" ");
      }

      line = line.substr(3, line.size() - 1);

      float triangle[9];

      float red, green, blue;
      float reflectiveness = 0;
      int j = 0;

      while(line.find(" ") != std::string::npos){

        int spacepos = line.find(" ");

        float coord = std::stof(line.substr(0, spacepos));

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
        }else if(j == 11){
          blue = coord;
        }else if(j == 12){
          reflectiveness = coord;
        }

        line = line.substr(spacepos + 1, line.size());
        j++;

      }

      Triangle* t = new Triangle(triangle);
      t -> SetColor(red, green, blue);
      t -> reflectiveness = reflectiveness;
      triangles.push_back(t);

    }else if(line[0] == 'l' && line[1] == 'o'){
      //recursively load another scene
      line = line.substr(3, line.size());
      float x = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float y = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float z = std::stof(line.substr(0, line.find(" ")));

      Load(line.substr(line.find(" ") + 1, line.size()), x + movex, y + movey, z + movez);

    }else if(line[0] == 'l' && line[1] == 's'){
      //light source
      line = line.substr(3, line.size());
      float x = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float y = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float z = std::stof(line.substr(0, line.find(" ")));
      line = line.substr(line.find(" ") + 1, line.size());
      float brightness = std::stof(line.substr(0, line.find(" ")));

      LightSource* l = new LightSource(x, y, z, brightness);
      light_sources.push_back(l);

    }

    i++;
  }

}

void Frame::SaveAsPng(std::string file){

  unsigned char* img = new unsigned char[3*width*height];
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){

      float pixel_brightness = brightness_buffer[x][y];

      img[3*x + 3*width*y] = (int) (frame[0][x][y]*255*pixel_brightness/(1 + max_brightness));
      img[3*x + 3*width*y + 1] = (int) (frame[1][x][y]*255*pixel_brightness/(1 + max_brightness));
      img[3*x + 3*width*y + 2] = (int) (frame[2][x][y]*255*pixel_brightness/(1 + max_brightness));

    }
  }
  lodepng::encode(file, img, width, height, LCT_RGB);
  delete[] img;

}

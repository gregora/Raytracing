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



Texture::Texture(std::string file){

  unsigned w, h;

  std::vector<unsigned char> vector;

  lodepng::decode(vector, w, h, file);

  width = w;
  height = h;

  //create 3D array
  texture = new float** [3];
  for(int i = 0; i < 3; i++){
    texture[i] = new float* [width];
    for(int j = 0; j < width; j++){
      texture[i][j] = new float[height];
    }
  }


  //actually create the texture
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      texture[0][i][j] = ((float) vector[4*i + 4*width*j]) / 255;
      texture[1][i][j] = ((float) vector[4*i + 4*width*j + 1]) / 255;
      texture[2][i][j] = ((float) vector[4*i + 4*width*j + 2]) / 255;
    }
  }

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

  Vector normal = Normal();
  float rdotn = DotProduct(normal, ray);

  if(rdotn == 0) return -1;
  return DotProduct(normal, position - ray_position) / rdotn;

}


void Triangle::SetColor(float r, float g, float b){

  red = r;
  green = g;
  blue = b;

}

float* Triangle::RayHitsTriangle(Vector ray, Vector ray_position){

  float k = GetRayPlaneIntersection(ray, ray_position);

  float* ret = new float[4];
  ret[0] = 0;
  ret[1] = 0;
  ret[2] = 0;
  ret[3] = 0;

  if(k > 1 || k < 0) return ret;

  Vector p = ray_position + ray*k - position;

  float m,n;

  //solve system of linear equations for m and n
  if(a.x != 0){

    if(a.x*b.y - a.y*b.x != 0){
      n = (p.y*a.x - p.x*a.y) / (b.y*a.x - b.x*a.y);
    }else{
      n = (a.x*p.z - a.z*p.x) / (a.x*b.z - a.z*b.x);
    }

    if(n < 0 || n > 1) return ret;
    m = (p.x - n*b.x) / a.x;
  }else if(a.y != 0){

    if(a.y*b.x - a.x*b.y != 0){
      n = (a.y*p.x - a.x*p.y) / (a.y*b.x - a.x*b.y);
    }else{
      n = (a.y*p.z - a.z*p.y) / (a.y*b.z - a.z*b.y);
    }

    if(n < 0 || n > 1) return ret;
    m = (p.y - b.y*n)/a.y;
  }else{

    if(a.z*b.x - a.x*b.z != 0){
      n = (a.z*p.x - a.x*p.z) / (a.z*b.x - a.x*b.z);
    }else{
      n = (a.z*p.y - a.y*p.z) / (a.z*b.y - a.y*b.z);
    }

    if(n < 0 || n > 1) return ret;
    m = (p.z - b.z*n)/a.z;

  }

  if(m < 0 || m > 1) return ret;
  if(n + m > 1) return ret;

  ret[0] = 1;
  ret[1] = k;
  ret[2] = m;
  ret[3] = n;

  return ret;
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

  if(yaw > 360) yaw = fmod(yaw, 360);
  if(yaw < 0) yaw = 360 - fmod(-yaw, 360);
  if(pitch > 360) pitch = fmod(pitch, 360);
  if(pitch < 0) pitch = 360 - fmod(-pitch, 360);
  if(roll > 360) roll = fmod(roll, 360);
  if(roll < 0) roll = 360 - fmod(-roll, 360);

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

      float r = 0;
      float g = 0;
      float b = 0;

      float depth = 0;
      float brightness = 0;

      for(int s1 = 0; s1 < frame -> spp; s1++){
        for(int s2 = 0; s2 < frame -> spp; s2++){

          //calculate current ray
          Vector current_ray = frame -> camera_direction + sphere_tangent1*(i - frame -> width/2) + sphere_tangent2*(j - frame -> height/2) + /* super-sampling bit*/ sphere_tangent1*(((float)s1)/(frame -> spp)) + sphere_tangent2*(((float)s2)/(frame -> spp));

          float * colors = GetPixelColor(frame, current_ray, frame -> camera_position, frame -> reflections_number + 1);

          r += colors[0];
          g += colors[1];
          b += colors[2];

          depth += colors[3];
          brightness += colors[4];

          delete[] colors;
        }
      }

      int spp_squared = frame -> spp * frame -> spp;
      frame -> frame[0][i][j] = r / spp_squared;
      frame -> frame[1][i][j] = g / spp_squared;
      frame -> frame[2][i][j] = b / spp_squared;

      frame -> depth_buffer[i][j] = depth / spp_squared;

      frame -> brightness_buffer[i][j] = brightness / spp_squared;

      //set max brightness of a frame
      if(frame -> max_brightness == -1 || frame -> max_brightness < brightness / spp_squared){
        frame -> max_brightness = brightness / spp_squared;
      }


    }
  }
}



float * Frame::GetPixelColor(Frame * frame, Vector ray, Vector ray_position, int reflections, int avoid_triangle){

  float * return_colors = new float[5]; //return r,g,b and depth

  bool hits = false;

  int triangle_num = -1;
  float min_triangle_distance = std::numeric_limits<float>::max();

  float scale_m, scale_n;

  //iterate through all triangles
  for(int m = 0; m < frame -> triangles.size(); m++){
    if(m != avoid_triangle){
      //actually cast the ray
      float* data = (frame -> triangles[m] -> RayHitsTriangle(ray, ray_position));
      float triangle_distance = data[1];

      if(data[0] != 0){
        if(triangle_distance < min_triangle_distance){
          triangle_num = m;
          min_triangle_distance = triangle_distance;
          scale_m = data[2];
          scale_n = data[3];
        }
      }
      delete[] data;
    }
  }

  //handle if there was a collision
  if(triangle_num != -1){

    Triangle t = *(frame -> triangles[triangle_num]);

    //get r,g,b values for pixel
    float r,g,b;
    if(t.texture == nullptr){
      //if triangle has no texture, use its colors
      r = t.red;
      g = t.green;
      b = t.blue;
    }else{
      //if triangle does have a texture, use it

      int tex_width = t.texture -> width;
      int tex_height = t.texture -> height;
      float *** tr_texture = t.texture -> texture;

      int x = (int) (t.tex_position.x + scale_m * t.tex_a.x + scale_n * t.tex_b.x);
      int y = tex_height - (int) (t.tex_position.y + scale_m * t.tex_a.y + scale_n * t.tex_b.y);

      r = tr_texture[0][x][y];
      g = tr_texture[1][x][y];
      b = tr_texture[2][x][y];

    }


    Vector collision_position;
    collision_position = ray*min_triangle_distance + (ray_position);


    //calculate if the pixel is in shadow
    float combined_light_power = frame -> ambient_light; //how much light even is there in the scene?
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
            float* data = (frame -> triangles[m] -> RayHitsTriangle(ray_to_light_source, collision_position));

            //if pixel is obscured by a triangle, no light from the light source will hit it
            if(data[0] != 0){
              is_covered = true;
              delete[] data;
              break;
            }

            delete[] data;

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

      delete[] reflection_colors;
    }

  }else{

    if(frame -> skydome == nullptr){ //if no skydome is set, use rgb
      return_colors[0] = frame -> sky_red;
      return_colors[1] = frame -> sky_green;
      return_colors[2] = frame -> sky_blue;
    }else{

      Vector ray_normal = ray;
      ray_normal.Normalize();

      unsigned int u = (int) ((0.5 + atan2(ray_normal.x, ray_normal.y) / 6.28)*(frame -> skydome -> width));
      unsigned int v = (int) ((0.5 - asin(ray_normal.z) / 3.14)*(frame -> skydome -> height));

      u = u % frame -> skydome -> width;
      v = v % frame -> skydome -> height;

      return_colors[0] = frame -> skydome -> texture[0][u][v];
      return_colors[1] = frame -> skydome -> texture[1][u][v];
      return_colors[2] = frame -> skydome -> texture[2][u][v];


    }

    return_colors[3] = std::numeric_limits<float>::max();
    return_colors[4] = 1;
  }

  return return_colors;

}

void Frame::ApplyPixelBrightness(int x, int y){

  if(depth_buffer[x][y] < std::numeric_limits<float>::max()){
    frame[0][x][y] = frame[0][x][y]*brightness_buffer[x][y]/(1 + max_brightness);
    frame[1][x][y] = frame[1][x][y]*brightness_buffer[x][y]/(1 + max_brightness);
    frame[2][x][y] = frame[2][x][y]*brightness_buffer[x][y]/(1 + max_brightness);
  }

}

void Frame::ToScreen(float * (*function)(Frame *, int x, int y)){

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){

      ApplyPixelBrightness(i, j);

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
      SDL_SetRenderDrawColor(renderer, frame[0][i][j]*255, frame[1][i][j]*255, frame[2][i][j]*255, 255);
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

    std::vector<std::string> args = SplitString(line, " ");

    if(args[0] == "#"){
      //ignore comments
    }else if(args[0] == "tr") {
      //add triangle

      float triangle[9];

      float red, green, blue;
      float reflectiveness = 0;

      triangle[0] = std::stof(args[1]) + movex;
      triangle[1] = std::stof(args[2]) + movey;
      triangle[2] = std::stof(args[3]) + movez;

      triangle[3] = std::stof(args[4]) + movex;
      triangle[4] = std::stof(args[5]) + movey;
      triangle[5] = std::stof(args[6]) + movez;

      triangle[6] = std::stof(args[7]) + movex;
      triangle[7] = std::stof(args[8]) + movey;
      triangle[8] = std::stof(args[9]) + movez;

      red = std::stof(args[10]);
      green = std::stof(args[11]);
      blue = std::stof(args[12]);

      if(args.size() > 13) {
        reflectiveness = std::stof(args[13]);
      }

      Triangle* t = new Triangle(triangle);
      t -> SetColor(red, green, blue);
      t -> reflectiveness = reflectiveness;
      triangles.push_back(t);

    }else if(args[0] == "ttr") {
      //add textured triangle


      float triangle[9];
      float reflectiveness = 0;

      //coordinates of triangle in 3d space
      triangle[0] = std::stof(args[1]) + movex;
      triangle[1] = std::stof(args[2]) + movey;
      triangle[2] = std::stof(args[3]) + movez;

      triangle[3] = std::stof(args[4]) + movex;
      triangle[4] = std::stof(args[5]) + movey;
      triangle[5] = std::stof(args[6]) + movez;

      triangle[6] = std::stof(args[7]) + movex;
      triangle[7] = std::stof(args[8]) + movey;
      triangle[8] = std::stof(args[9]) + movez;

      std::string path = args[10];

      //coordinates on texture
      Vector* tex_1 = new Vector(std::stof(args[11]), std::stof(args[12]));
      Vector* tex_2 = new Vector(std::stof(args[13]) - std::stof(args[11]), std::stof(args[14]) - std::stof(args[12]));
      Vector* tex_3 = new Vector(std::stof(args[15]) - std::stof(args[11]), std::stof(args[16]) - std::stof(args[12]));

      //reflectiveness - optional argument
      if(args.size() > 17) {
        reflectiveness = std::stof(args[17]);
      }

      Texture* tex = new Texture(path);
      Triangle* t = new Triangle(triangle);
      t -> texture = tex;
      t -> reflectiveness = reflectiveness;

      t -> tex_position = *tex_1;
      t -> tex_a = *tex_2;
      t -> tex_b = *tex_3;

      triangles.push_back(t);

    }else if(args[0] == "lo"){
      //recursively load another scene

      float x = std::stof(args[1]);
      float y = std::stof(args[2]);
      float z = std::stof(args[3]);

      Load(args[4], x + movex, y + movey, z + movez);

    }else if(args[0] == "ls"){
      //light source
      float x = std::stof(args[1]);
      float y = std::stof(args[2]);
      float z = std::stof(args[3]);
      float brightness = std::stof(args[4]);

      LightSource* l = new LightSource(x + movex, y + movey, z + movez, brightness);
      light_sources.push_back(l);

    }else if(args[0] == "sky"){
      Texture* tex = new Texture(args[1]);
      skydome = tex;

    }

    i++;
  }

}

void Frame::SaveAsPng(std::string file){

  unsigned char* img = new unsigned char[3*width*height];
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){

      ApplyPixelBrightness(x, y);

      float pixel_brightness = brightness_buffer[x][y];

      img[3*x + 3*width*y] = (int) (frame[0][x][y]*255);
      img[3*x + 3*width*y + 1] = (int) (frame[1][x][y]*255);
      img[3*x + 3*width*y + 2] = (int) (frame[2][x][y]*255);

    }
  }
  lodepng::encode(file, img, width, height, LCT_RGB);
  delete[] img;

}

std::vector<std::string> SplitString(std::string string, std::string split_by){

  std::vector<std::string> ret;

  while(string.find(split_by) != std::string::npos){
    int spacepos = string.find(split_by);
    ret.push_back(string.substr(0, spacepos));
    string = string.substr(spacepos + split_by.size(), string.size());
  }

  ret.push_back(string.substr(0, string.find(split_by)));

  return ret;

}

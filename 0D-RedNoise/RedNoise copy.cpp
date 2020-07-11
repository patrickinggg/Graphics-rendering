#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <MyObject.h>
#include <PatObject.h>
#include <algorithm>    // std::max
#include <AdjacentTable.h>
#include <MyMaterial.h>
#include <MyCamera.h>
#include <MyMatrix.h>
#include <MyRayTracer.h>
#include <map>
#include <math.h>
#include <unordered_map>
#include <MyPointLight.h>
using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 320

void RT();
void readObject(float scale);

void draw();
void update();
void handleEvent(SDL_Event event);
std::vector<PatObject> obj_loader(string file_name);
unordered_map<string, Colour> mat_loader(std::vector<string> mtl_files);
string convertToString(char* a, int size);
void renderWireFrame(vector<PatObject> objs);
void drawTriangles(CanvasTriangle triangle);
void drawline(CanvasPoint from, CanvasPoint to, uint32_t colour);
std::vector<float> interpolation(float from, float to, int numOfElements);
void drawFilledTrianglesDepth(CanvasTriangle triangle);
void drawFilledSceneWithDepth();
std::vector<glm::vec3> vec3interpolation(glm::vec3 from, glm::vec3 to, int numOfElements);
vector<CanvasPoint> sort_triangle(CanvasTriangle triangle);
mat4 toTranslationMatrix(vec3 vec);
mat4 Yrotation(float rotation);

float* interpolationf(float from, float to, int numOfVal);
void fillTriangleFlatDepth(CanvasTriangle triangle, Colour brush);
CanvasTriangle sortTriangle(CanvasTriangle triangle);
void fillTriangleDepth(CanvasTriangle inputTriangle, Colour brush);
void drawFilledSceneWithLookAt(mat4 world);
void drawFilledSceneWithDepth_ambiant_lighting(float ambiant_strength);
void drawFilledSceneWithDepth_diffuse_lighting(MyPointLight lightsource);
void drawFilledSceneWithDepth_specular_lighting(MyPointLight lightsource);
void fillTriangleDepth_phong(CanvasTriangle inputTriangle, Colour brush, MyPointLight lightsource);
void fillTriangleFlatDepth_phong(CanvasTriangle triangle, Colour brush, MyPointLight lightsource);
float getBrightness(vec3 vecPoint, vec3 lightPos, vec3 viewPos, vec3 normal, float specStrength);

vector<AdjacentTable> adjacent_faces(string file_name);
void phong_shading(MyPointLight lightsource);

mat4 lookAt(vec3 from, vec3 to);


DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

string mtlFileName;
vector<MyObject*> objs;
MyObject* currentObj;
vector<vec3*> verticesRepo;
MyMaterial* currentMat;
map<string,MyMaterial*> matRepo;

MyCamera plainCamera = MyCamera();
float rotationY = 3.1416;
float depthBuffer[WIDTH][HEIGHT];
int objz = 5;
MyRayTracer raytracer;
mat4 camera( 1.0f, 0.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f,
             0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, 0.0f, 0.0f, 1.0f);
float rotation = 3.14;
vec3 lookAtfrom = vec3(-1.0f, 1.0f, 7.0f);
int main(int argc, char* argv[])
{
  //---testing here---
  readObject(1.0);
  RT();
  //------------------
  // std::vector<PatObject> obj = obj_loader("cb.obj");

  SDL_Event event;
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);
    update();
    draw();
    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

mat4 toTranslationMatrix(vec3 vec){
  return mat4(1.0f, 0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
               vec.x, vec.y, vec.z, 1.0f);
   // return mat4(1.0f, 0.0f, 0.0f, vec.x,
   //              0.0f, 1.0f, 0.0f, vec.x,
   //              0.0f, 0.0f, 1.0f, vec.x,
   //              0.0f, 0.0f, vec.z, 1.0f);
}

mat4 Yrotation(float rotation){
  return mat4(cos(rotation), 0.0f, -sin(rotation), 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               sin(rotation), 0.0f, cos(rotation), 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
}

void draw()
{
  // clear depth buffer
  for (int j = 0; j < HEIGHT; j++){
    for (int i = 0; i < WIDTH; i++){
      depthBuffer[i][j] = 0;
    }
  }
  // mat4 world = lookAt(lookAtfrom, vec3(0.0f, 2.5f, 0.0f));
  // drawFilledSceneWithLookAt(world);
  // drawFilledSceneWithDepth();
  // drawFilledSceneWithDepth_ambiant_lighting(0.1);
  MyPointLight light = MyPointLight();
  // drawFilledSceneWithDepth_diffuse_lighting(light);
  // drawFilledSceneWithDepth_specular_lighting(light);
  phong_shading(light);
  // window.clearPixels();
  // for(int y=0; y<window.height ;y++) {
  //   for(int x=0; x<window.width ;x++) {
  //     float red = 120;
  //     float green = 120;
  //     float blue = 120;
  //     uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
  //     window.setPixelColour(x, y, colour);
  //   }
  // }

}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)

  rotationY += 0.005;
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) {
      window.clearPixels();
      rotation += 0.05;
      // drawFilledSceneWithDepth();

    }
    else if(event.key.keysym.sym == SDLK_RIGHT){
      window.clearPixels();
      rotation -= 0.05;
      // drawFilledSceneWithDepth();

    }
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;

    else if(event.key.keysym.sym == SDLK_1){ // lookAt
      window.clearPixels();

      lookAtfrom.x += 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';
    }
    else if(event.key.keysym.sym == SDLK_2){ // lookAt
      window.clearPixels();

      lookAtfrom.y += 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }
    else if(event.key.keysym.sym == SDLK_3){ // lookAt
      window.clearPixels();

      lookAtfrom.z += 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }

    else if(event.key.keysym.sym == SDLK_4){ // lookAt
      window.clearPixels();

      lookAtfrom.x -= 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';
    }
    else if(event.key.keysym.sym == SDLK_5){ // lookAt
      window.clearPixels();

      lookAtfrom.y -= 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }
    else if(event.key.keysym.sym == SDLK_6){ // lookAt
      window.clearPixels();

      lookAtfrom.z -= 1.0f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }
    else if(event.key.keysym.sym == SDLK_u){

    }
    else if(event.key.keysym.sym == SDLK_f){
      std::vector<PatObject> obj = obj_loader("cb.obj");
      drawFilledSceneWithDepth();
    }
    else if(event.key.keysym.sym == SDLK_r){//test reading obj_file
      std::vector<PatObject> obj = obj_loader("cb.obj");
      for (int i = 0; i < obj.size(); i++){
        std::vector<ModelTriangle> thisobj = obj[i].triangles;
        for (int j = 0; j < thisobj.size(); j++) {
          std::cout << thisobj[j] << '\n';
          // std::cout << thisobj[j].colour << '\n';
        }
        // std::cout << obj[i].name << '\n';

      }
    }
    else if(event.key.keysym.sym == SDLK_d){
      std::vector<AdjacentTable> v = adjacent_faces("cb.obj");
      for (int i = 0; i < v.size(); i++){
        vec3 vertex = v[i].vertex;
        std::vector<ModelTriangle> triangles = v[i].adj_faces;
        std::cout << vertex.x << " " << vertex.y << " " << vertex.z << '\n';
        for (int j = 0; j < triangles.size(); j++){
          std::cout << triangles[j] << '\n';
        }
      }
    }
    else if(event.key.keysym.sym == SDLK_w){//render Wireframe
      std::vector<PatObject> obj = obj_loader("cb.obj");
      renderWireFrame(obj);
    }
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}

void readObject(float scale){
  // ifstream f("cornell-box.obj", ios::in);
  ifstream f("cb.obj", ios::in);
  while(!f.eof()){
  // for (int i = 0; i < 10; i++){
    string str;
    getline(f, str);
    if (str.length() < 1){continue;}
    string* segs = split(str, 32);
    string arg = *segs;
    if (arg == "mtllib"){
      mtlFileName = *(segs+1);
      ifstream f2(mtlFileName, ios::in);
      MyMaterial* tmpMat = new MyMaterial();
      while(!f2.eof()){
        string str2;
        getline(f2, str2);
        if (str2.length() < 1){continue;}
        string* segs2 = split(str2, 32);
        string arg2 = *segs2;
        if (arg2 == "newmtl"){
          tmpMat = new MyMaterial();
          matRepo[*(segs2+1)] = tmpMat;
        }
        else if (arg2 == "Kd"){
          int tmpRed = (int)(255.0 * stof(*(segs2+1)));
          int tmpGreen = (int)(255.0 * stof(*(segs2+2)));
          int tmpBlue = (int)(255.0 * stof(*(segs2+3)));
          Colour* tmpC = new Colour(tmpRed, tmpGreen, tmpBlue);
          (*tmpMat).mat_color = tmpC;
        }
      }
      f2.close();

      // cout<<matRepo.size()<<endl;
    }
    else if(arg == "o"){
      currentObj = new MyObject();
      objs.push_back(currentObj);
      (*currentObj).name = *(segs+1);
      // cout<<"see obj: "<<(*currentObj).name<<endl;
    }
    else if(arg == "usemtl"){
      MyMaterial* tmpMat = matRepo[*(segs+1)];
      currentMat = tmpMat;
    }
    else if(arg == "v"){
      float tmpX = stof(*(segs+1));
      float tmpY = stof(*(segs+2));
      float tmpZ = stof(*(segs+3));
      vec3* tmpVec = new vec3(tmpX * scale, tmpY * scale, tmpZ * scale);
      verticesRepo.push_back(tmpVec);
      // cout<<"see v: "<<(*tmpVec).z<<endl;
    }
    else if(arg == "f"){
      string* f_args1 = split(*(segs+1), '/');
      string* f_args2 = split(*(segs+2), '/');
      string* f_args3 = split(*(segs+3), '/');
      int index1 = stoi(*f_args1);
      int index2 = stoi(*f_args2);
      int index3 = stoi(*f_args3);
      ModelTriangle* tmpTri = new ModelTriangle();
      tmpTri->vertices[0] = *verticesRepo[index1 - 1];
      tmpTri->vertices[1] = *verticesRepo[index2 - 1];
      tmpTri->vertices[2] = *verticesRepo[index3 - 1];
      tmpTri->colour = *((*currentMat).mat_color);
      (*currentObj).mesh.push_back(tmpTri);
      // cout<<"see f: "<<index1<<endl;
    }

  }
  f.close();

  // for (int i = 0; i < (int)objs.size(); i++){
  //   cout<<(*objs[i]).name<<endl;
  // }
  // cout<<(*(*objs[1]).mesh[0]).colour.red<<endl;
}

std::vector<PatObject> obj_loader(string file_name)
{
  std::ifstream myfile;
  std::vector<string> mtl_file;
  std::vector<vec3> vertices;
  vector<ModelTriangle> triangles;
  vector<PatObject> objects;
  char mtllib[256];
  char file_path[256];
  char line[50];
  char current_colour[50];
  char current_obj[50];
  char check_obj[50];
  char coordinate[256];
  float x;
  float y;
  float z;
  int a;
  int b;
  int c;

  myfile.open(file_name, std::ios::binary);



  while (myfile.good()) {
    myfile.getline(mtllib, 256);
    // std::cout << "the file is " <<mtllib << '\n';

    if(sscanf(mtllib, "mtllib %s", file_path) == 1){

      mtl_file.push_back(file_path);
    }
  }
  myfile.clear();                 // clear fail and eof bits
  myfile.seekg(0, std::ios::beg); // back to the start!

  while (myfile.good()) {
    myfile.getline(coordinate, 256);

    if(sscanf(coordinate, "v %f %f %f", &x, &y, &z) == 3){
      // std::cout << x << " " << y << " " << z << '\n';
      vec3 vertex = vec3(x, y, z);
      vertices.push_back(vertex);
    }
  }

  myfile.clear();                 // clear fail and eof bits
  myfile.seekg(0, std::ios::beg); // back to the start!

  unordered_map<string, Colour> material = mat_loader(mtl_file);

  while (myfile.good()) {
    myfile.getline(line, 50);


    if(sscanf(line, "usemtl %s", current_colour)){
      continue;
    }
    // objects[current_obj]
    if(sscanf(line, "o %s", check_obj)){
      // std::cout << check_obj << '\n';
      int n = strcmp(check_obj, current_obj);
      // std::cout << n << '\n';
      if(n != 0){

        if(triangles.size() != 0) {
          PatObject thisobj = PatObject(convertToString(current_obj, sizeof(current_obj)), triangles);
          objects.push_back(thisobj);
        }

        triangles.clear();
      }

      for (int i = 0; i < sizeof(current_obj); i++) {
        current_obj[i] =0;
      }

      for (int i = 0; i < sizeof(check_obj); i++) {
        current_obj[i] = check_obj[i];
      }

    }

    if(sscanf(line, "f %d/ %d/ %d", &a, &b, &c)){
      vec3 a_vertex = vertices[a-1];
      vec3 b_vertex = vertices[b-1];
      vec3 c_vertex = vertices[c-1];

      ModelTriangle triangle = ModelTriangle(a_vertex, b_vertex, c_vertex, material[current_colour]);
      // std::cout << triangle << '\n';
      // break;
      triangles.push_back(triangle);
    }
    // myfile.get();
    // std::cout << myfile.good() << '\n';
    // if(!myfile.good()) {
    //   MyObject thisobj = MyObject(convertToString(current_obj, sizeof(current_obj)), triangles);
    //   objects.push_back(thisobj);
    }
    PatObject thisobj = PatObject(convertToString(current_obj, sizeof(current_obj)), triangles);
    objects.push_back(thisobj);


  return objects;
}

std::vector<AdjacentTable> adjacent_faces(string file_name)
{
  std::ifstream myfile;
  std::vector<vec3> vertices;
  vector<ModelTriangle> triangles;
  vector<PatObject> objects;

  char line[50];

  char coordinate[256];
  float x;
  float y;
  float z;
  int a;
  int b;
  int c;

  myfile.open(file_name, std::ios::binary);
  std::vector<AdjacentTable> adjacent_table;
  std::vector<ModelTriangle> actual_faces;
  std::vector<vec3> index_faces;

  while (myfile.good()) {
    myfile.getline(coordinate, 256);

    if(sscanf(coordinate, "v %f %f %f", &x, &y, &z) == 3){
      // std::cout << x << " " << y << " " << z << '\n';
      vec3 vertex = vec3(x, y, z);
      vertices.push_back(vertex);
    }
  }

  myfile.clear();                 // clear fail and eof bits
  myfile.seekg(0, std::ios::beg); // back to the start!


  while (myfile.good()) {
    myfile.getline(line, 50);

    if(sscanf(line, "f %d/ %d/ %d", &a, &b, &c) == 3){
      // std::cout << a << " " << b << " " << c << '\n';

      vec3 a_vertex = vertices[a-1];
      vec3 b_vertex = vertices[b-1];
      vec3 c_vertex = vertices[c-1];
      vec3 indices = vec3(a-1, b-1, c-1);
      ModelTriangle triangle = ModelTriangle(a_vertex, b_vertex, c_vertex);
      index_faces.push_back(indices);
      actual_faces.push_back(triangle);
    }
  }

  for (int i = 0; i < vertices.size(); i++){
    std::vector<ModelTriangle> adj_faces;
    for (int j = 0; j < actual_faces.size(); j++){
        vec3 faces = index_faces[j];
        if (faces.x == i || faces.y == i || faces.z == i) {
          adj_faces.push_back(actual_faces[j]);
        }
    }
    AdjacentTable table = AdjacentTable(vertices[i], adj_faces);
    adjacent_table.push_back(table);
  }

  return adjacent_table;
}


unordered_map<string, Colour> mat_loader(std::vector<string> mtl_files)
{
  unordered_map<string, Colour> material;
  std::ifstream myfile;
  char line[256];
  char colour[256];
  float red;
  float green;
  float blue;


  for (int i = 0; i < mtl_files.size(); i++) {
    std::ifstream myfile;
    myfile.open(mtl_files[i], std::ios::binary);
    while (myfile.good()) {
      myfile.getline(line, 256);

      if(sscanf(line, "newmtl %s", colour) == 1){
        myfile.getline(line, 256);
        sscanf(line, "Kd %f %f %f", &red, &green, &blue);
        Colour c = Colour(round(red * 255), round(green * 255), round(blue * 255));
        material[colour] = c;
      }
    }
  }

  return material;
}

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

void renderWireFrame(vector<PatObject> objs)
{
  window.clearPixels();

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();
        // std::cout << triangle << '\n';
        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];

          // vec4 cameraView = camera * vec4(vecPoint, 1.0f);

          vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
                                  (vecPoint.y/(vecPoint.z-5)) * -100,
                                 0);

          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + (projection.x),
                                                   0.7 * HEIGHT - projection.y);
        }

        screenTriangle.colour = Colour(255,255,255);
        drawTriangles(screenTriangle);
      }
    }

}

void drawTriangles(CanvasTriangle triangle)
{
  CanvasPoint a = triangle.vertices[0];
  CanvasPoint b = triangle.vertices[1];
  CanvasPoint c = triangle.vertices[2];

  uint32_t colour = (255<<24) + (int(triangle.colour.red)<<16) + (int(triangle.colour.green)<<8) + int(triangle.colour.blue);

  drawline(a, b, colour);
  drawline(b, c, colour);
  drawline(c, a, colour);
}

void drawline(CanvasPoint from, CanvasPoint to, uint32_t colour)
{
  // window.clearPixels();

  float xdiff = to.x - from.x;
  float ydiff = to.y - from.y;
  float numberOfSteps = std::max(abs(xdiff), abs(ydiff));
  float xStepSize = xdiff/numberOfSteps;
  float yStepSize = ydiff/numberOfSteps;

  // uint32_t colour = (255<<24) + (int(255)<<16) + (int(255)<<8) + int(255);

  for (float i=0.0; i<numberOfSteps; i++) {
    float x = from.x + (xStepSize*i);
    float y = from.y + (yStepSize*i);
    window.setPixelColour(round(x), round(y), colour);
  }

}

void drawHalfTriangleDepth(CanvasTriangle triangle)
{
  Colour c = triangle.colour;
  uint32_t colour = (255<<24) + (int(c.red)<< 16) + (int(c.green)<< 8) + int(c.blue);

  CanvasPoint p_top = triangle.vertices[0];
  CanvasPoint p_left = triangle.vertices[1];
  CanvasPoint p_right = triangle.vertices[2];

  int lineCount = abs(p_left.y - p_top.y) + 1;

  std::vector<vec3> leftside = vec3interpolation(vec3(p_top.x, p_top.y, p_top.depth), vec3(p_left.x, p_left.y, p_left.depth), lineCount);
  std::vector<vec3> rightside = vec3interpolation(vec3(p_top.x, p_top.y, p_top.depth), vec3(p_right.x, p_right.y, p_right.depth), lineCount);

  for (int i = 0; i < lineCount; i++){
    int leftx = (int)round(leftside[i].x);
    int rightx = (int)round(rightside[i].x);
    int pos_y = (int)round(leftside[i].y);

    int linePixelCount = rightx - leftx +1;
    std::vector<float> z_values = interpolation(leftside[i].z, rightside[i].z, linePixelCount);
    for (int j = 0; j < linePixelCount; j++){
      int pos_x = j + leftx;
      if (z_values[j] > depthBuffer[pos_y][pos_x]){
        window.setPixelColour(pos_x, pos_y, colour);
        depthBuffer[pos_y][pos_x] = z_values[j];
      }
    }
  }

}
//
// void drawFilledTrianglesDepth(CanvasTriangle triangle)
// {
//   // vec3 cameraPoint = vec3(30.0, 0.0, 10.0);
//
//   std::vector<CanvasPoint> sorted = sort_triangle(triangle);
//   Colour c = triangle.colour;
//   uint32_t colour = (255<<24) + (int(c.red)<< 16) + (int(c.green)<< 8) + int(c.blue);
//
//   int numOfElements_ac = round(abs(sorted[0].y - sorted[2].y) + 1);
//   std::vector<vec3> line_ac = vec3interpolation(vec3(sorted[0].x, sorted[0].y, sorted[0].depth), vec3(sorted[2].x, sorted[2].y, sorted[2].depth), numOfElements_ac);
//
//   float leftpoint_x;
//   float leftpoint_z;
//   for (vec3 v : line_ac) {
//     if (round(v.y) == round(sorted[1].y)){
//       leftpoint_x = v.x;
//       leftpoint_z = v.z;
//     }
//   }
//
//
//
//   int numOfElements_a_mid = round(abs(sorted[0].y - sorted[1].y) + 1);
//   std::vector<vec3> line_a_mid = vec3interpolation(vec3(sorted[0].x, sorted[0].y, 1/sorted[0].depth), vec3(sorted[1].x, sorted[1].y, 1/sorted[1].depth), numOfElements_a_mid);
//
//   int numOfElements_b_mid = round(abs(sorted[1].y - sorted[2].y) + 1);
//   std::vector<vec3> line_b_mid = vec3interpolation(vec3(sorted[1].x, sorted[1].y, 1/sorted[1].depth), vec3(sorted[2].x, sorted[2].y, 1/sorted[2].depth), numOfElements_b_mid);
//
//   int numOfElements_ac_mid1 = round(abs(sorted[0].y - sorted[1].y) + 1);
//   std::vector<vec3> line_ac_mid1 = vec3interpolation(vec3(sorted[0].x, sorted[0].y, 1/sorted[0].depth), vec3(leftpoint_x, sorted[1].y, 1/leftpoint_z), numOfElements_ac_mid1);
//
//   int numOfElements_ac_mid2 = round(abs(sorted[1].y - sorted[2].y) + 1);
//   std::vector<vec3> line_ac_mid2 = vec3interpolation(vec3(leftpoint_x, sorted[1].y, 1/leftpoint_z), vec3(sorted[2].x, sorted[2].y, 1/sorted[2].depth), numOfElements_ac_mid2);
//
//
//   for (int i = 0; i < numOfElements_a_mid; i ++) {
//     int leftside = round(std::min(line_a_mid[i].x, line_ac_mid1[i].x));
//     int rightside = round(std::max(line_a_mid[i].x, line_ac_mid1[i].x));
//     int ele_inbetween = rightside - leftside + 1;
//     std::vector<float> z_values;
//
//     if (line_a_mid[i].x < line_ac_mid1[i].x) {
//       z_values = interpolation(line_a_mid[i].z, line_ac_mid1[i].z, ele_inbetween);
//     } else {
//       z_values = interpolation(line_ac_mid1[i].z, line_a_mid[i].z, ele_inbetween);
//     }
//
//     int x = 0;
//     for (int j = leftside ; j <= rightside; j++ ){
//       // float z_inverse = 0.0f;
//       // if (z_values[x] == 0) {
//       //   z_inverse = 0.0f;
//       // } else {
//       //   z_inverse = 1 / z_values[x];
//       // }
//        float z_inverse = z_values[x];
//        if (z_inverse < depthBuffer[int(sorted[0].y + i)][j]) {
//          depthBuffer[int(sorted[0].y + i)][j] = z_inverse;
//          window.setPixelColour(j, sorted[0].y + i, colour);
//        }
//        x++;
//     }
//   }
//
//   for (int i = 0; i < numOfElements_b_mid; i ++) {
//     int leftside = round(std::min(line_b_mid[i].x, line_ac_mid2[i].x));
//     int rightside = round(std::max(line_b_mid[i].x, line_ac_mid2[i].x));
//     int ele_inbetween = rightside - leftside + 1;
//     std::vector<float> z_values;
//
//     if (line_b_mid[i].x < line_ac_mid2[i].x) {
//       z_values = interpolation(line_b_mid[i].z, line_ac_mid2[i].z, ele_inbetween);
//     } else {
//       z_values = interpolation(line_ac_mid2[i].z, line_b_mid[i].z, ele_inbetween);
//     }
//
//     int x = 0;
//     for (int j = leftside ; j <= rightside; j++ ){
//     //   float z_inverse;
//     //   if (z_values[x] == 0) {
//     //     z_inverse = 0.0f;
//     //   } else {
//     //     z_inverse = 1 / z_values[x];
//     //   }
//        float z_inverse = z_values[x];
//        if (z_inverse < depthBuffer[int(sorted[1].y + i)][j]) {
//          depthBuffer[int(sorted[1].y + i)][j] = z_inverse;
//          window.setPixelColour(j, sorted[1].y + i, colour);
//        }
//        x++;
//     }
//   }
// }


// void drawFilledTrianglesDepth(CanvasTriangle triangle)
// {
//   // vec3 cameraPoint = vec3(30.0, 0.0, 10.0);
//
//   std::vector<CanvasPoint> sorted = sort_triangle(triangle);
//   Colour c = triangle.colour;
//   uint32_t colour = (255<<24) + (int(c.red)<< 16) + (int(c.green)<< 8) + int(c.blue);
//
//   int numOfElements_ac = round(abs(sorted[0].y - sorted[2].y) + 1);
//   std::vector<vec3> line_ac = vec3interpolation(vec3(sorted[0].x, sorted[0].y, sorted[0].depth), vec3(sorted[2].x, sorted[2].y, sorted[2].depth), numOfElements_ac);
//
//   float leftpoint_x;
//   float leftpoint_z;
//   for (vec3 v : line_ac) {
//     if (round(v.y) == round(sorted[1].y)){
//       leftpoint_x = v.x;
//       leftpoint_z = v.z;
//     }
//   }
//
//   CanvasTriangle topTri;
//   CanvasTriangle bottomTri;
//   if (leftpoint_x > sorted[1].x){
//     topTri = CanvasTriangle(sorted[0], sorted[1], CanvasPoint(leftpoint_x, sorted[1].y), c);
//     bottomTri = CanvasTriangle(sorted[2], sorted[1], CanvasPoint(leftpoint_x, sorted[1].y), c);
//   } else {
//     topTri = CanvasTriangle(sorted[0], CanvasPoint(leftpoint_x, sorted[1].y), sorted[1], c);
//     bottomTri = CanvasTriangle(sorted[2], sorted[1], CanvasPoint(leftpoint_x, sorted[1].y), c);
//   }
//
//   drawHalfTriangleDepth(topTri);
//   // drawHalfTriangleDepth(bottomTri);
//
// }



std::vector<float> interpolation(float from, float to, int numOfElements)
{
  std::vector<float> v;

  if (numOfElements <= 1){
    v.push_back(from);
    return v;
  }

  float distance = to - from;
  float step = distance / (float)(numOfElements - 1);
  float currentValue = from;

  for (int i = 0; i < numOfElements; i++) {
    v.push_back(currentValue);
    currentValue += step;
  }

  return v;
}

void drawFilledSceneWithDepth()
{
    mat4 rotation_matrix = Yrotation(rotation);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();

        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          // vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
          //                        (vecPoint.y/(vecPoint.z-5)) * -100,
          //                        vecPoint.z);
           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));

        }
        screenTriangle.colour = triangle.colour;
        fillTriangleDepth(screenTriangle, screenTriangle.colour);
      }
    }
}

void drawFilledSceneWithDepth_ambiant_lighting(float ambiant_strength)
{
    mat4 rotation_matrix = Yrotation(rotation);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();

        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          // vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
          //                        (vecPoint.y/(vecPoint.z-5)) * -100,
          //                        vecPoint.z);
           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));

        }
        screenTriangle.colour.red = triangle.colour.red * ambiant_strength;
        screenTriangle.colour.green = triangle.colour.green * ambiant_strength;
        screenTriangle.colour.blue = triangle.colour.blue * ambiant_strength;
        fillTriangleDepth(screenTriangle, screenTriangle.colour);
      }
    }
}

void drawFilledSceneWithDepth_diffuse_lighting(MyPointLight lightsource)
{
    mat4 rotation_matrix = Yrotation(rotation);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();
        vec3 a = triangle.vertices[0];
        vec3 b = triangle.vertices[1];
        vec3 c = triangle.vertices[2];
        vec3 a_b = b - a;
        vec3 a_c = c - a;
        vec3 normal = cross(a_b, a_c);
        vec3 normal_unit = normalize(normal);
        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec3 lightdir = normalize(lightsource.position - vecPoint);
          float diff = glm::dot(lightdir, normal_unit);
          if (diff < 0){
            diff = 0.0;
          }
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          // vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
          //                        (vecPoint.y/(vecPoint.z-5)) * -100,
          //                        vecPoint.z);
           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));
          screenTriangle.vertices[k].brightness = diff;
        }
        screenTriangle.colour = triangle.colour;
        fillTriangleDepth(screenTriangle, screenTriangle.colour);
      }
    }
}

void drawFilledSceneWithDepth_specular_lighting(MyPointLight lightsource)
{
    float specularStrength = 0.5;

    mat4 rotation_matrix = Yrotation(rotation);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();
        vec3 a = triangle.vertices[0];
        vec3 b = triangle.vertices[1];
        vec3 c = triangle.vertices[2];
        vec3 a_b = b - a;
        vec3 a_c = c - a;
        vec3 normal = cross(a_b, a_c);
        vec3 normal_unit = normalize(normal);
        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec3 lightdir = normalize(lightsource.position - vecPoint);
          vec3 reflectDir = reflect(-lightdir, normal_unit);

          float diff = glm::dot(lightdir, normal_unit);
          vec3 view = normalize(vec3(0.0f, -2.0f, 5.0f) - vecPoint);
          float spec = std::pow(std::max(double(glm::dot(view, reflectDir)), 0.0), 32);

          if (diff < 0){
            diff = 0.0;
          }
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          // vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
          //                        (vecPoint.y/(vecPoint.z-5)) * -100,
          //                        vecPoint.z);
           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));
          screenTriangle.vertices[k].brightness = (specularStrength * spec) + diff + 0.1;
        }
        screenTriangle.colour = triangle.colour;
        fillTriangleDepth(screenTriangle, screenTriangle.colour);
      }
    }
}
void phong_shading(MyPointLight lightsource)
{

    mat4 rotation_matrix = Yrotation(rotation);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");
    std::vector<AdjacentTable> table = adjacent_faces("cb.obj");
    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();

        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec3 normal = vec3(0.0,0.0,0.0);

          for (int n = 0; n < table.size(); n++){
            if (vecPoint == table[n].vertex){
              for (int m = 0; m < table[n].adj_faces.size(); m++){
                vec3 d = table[n].adj_faces[m].vertices[0];
                vec3 e = table[n].adj_faces[m].vertices[1];
                vec3 f = table[n].adj_faces[m].vertices[2];
                vec3 d_e = e - d;
                vec3 d_f = f - d;
                vec3 face_normal = normalize(cross(d_e, d_f));
                normal = normal + face_normal;
              }
              normal.x = normal.x / table[n].adj_faces.size();
              normal.y = normal.y / table[n].adj_faces.size();
              normal.z = normal.z / table[n].adj_faces.size();

            }
          }
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));
          screenTriangle.vertices[k].brightness = 1;
          screenTriangle.vertices[k].normal = normalize(normal);
          // std::cout << normal.x << " " << normal.y << " " << normal.z << '\n';
        }
        screenTriangle.colour = triangle.colour;
        fillTriangleDepth_phong(screenTriangle, screenTriangle.colour, lightsource);
      }
    }
}


void drawFilledSceneWithLookAt(mat4 world)
{
    mat4 rotation_matrix = Yrotation(rotation);
    world = world * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("cb.obj");

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();

        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          // vec3 projection = vec3((vecPoint.x/(vecPoint.z-5)) * -100,
          //                        (vecPoint.y/(vecPoint.z-5)) * -100,
          //                        vecPoint.z);
           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.7 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));

        }
        screenTriangle.colour = triangle.colour;
        fillTriangleDepth(screenTriangle, screenTriangle.colour);
      }
    }
}


mat4 lookAt(vec3 from, vec3 to)
{
    vec3 tmp = vec3(0, 1, 0);
    vec3 forward = normalize(from - to);
    vec3 right = cross(normalize(tmp), forward);
    vec3 up = cross(forward, right);

    return mat4(right.x, right.y, right.z,0.0f,
                up.x, up.y, up.z, 0.0f,
                forward.x, forward.y, forward.z,0.0f,
                from.x, from.y, from.z, 1.0f);
}

vector<CanvasPoint> sort_triangle(CanvasTriangle triangle)
{
  std::vector<CanvasPoint> sorted;

  for (int i = 0; i < 3; i++) {
    if (triangle.vertices[i].y <= triangle.vertices[(i+1)%3].y &&
          triangle.vertices[i].y <= triangle.vertices[(i+2)%3].y) {
      sorted.push_back(triangle.vertices[i]);

      if (triangle.vertices[(i+1)%3].y < triangle.vertices[(i+2)%3].y) {
        sorted.push_back(triangle.vertices[(i+1)%3]);
        sorted.push_back(triangle.vertices[(i+2)%3]);
      } else {
        sorted.push_back(triangle.vertices[(i+2)%3]);
        sorted.push_back(triangle.vertices[(i+1)%3]);
      }
    }
  }

  return sorted;
}

std::vector<glm::vec3> vec3interpolation(glm::vec3 from, glm::vec3 to, int numOfElements)
{
  std::vector<glm::vec3> v;
  if (numOfElements <= 1) {
    v.push_back(from);
    return v;
  }

  glm::vec3 distance = to - from;
  glm::vec3 step = distance / (float)(numOfElements - 1);
  glm::vec3 currentValue = from;

  for (int i = 0; i < numOfElements; i++) {
    v.push_back(currentValue);
    currentValue += step;
  }

  return v;
}


void fillTriangleDepth(CanvasTriangle inputTriangle, Colour brush)
{
  // swap
  CanvasTriangle triangle = sortTriangle(inputTriangle);
  if (triangle.vertices[0].y == triangle.vertices[1].y || triangle.vertices[0].y == triangle.vertices[2].y
      || triangle.vertices[1].y == triangle.vertices[2].y){
    fillTriangleFlatDepth(triangle, brush);
    return;
  }

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpZ2Z0 = triangle.vertices[2].depth - triangle.vertices[0].depth;
  float tmpbrightness = triangle.vertices[2].brightness - triangle.vertices[0].brightness;

  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;

  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;
  float tmpMiddleZ = triangle.vertices[0].depth + tmpZ2Z0 * tmpProgress;
  float diff_mid = triangle.vertices[0].brightness + tmpbrightness * tmpProgress;

  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y, tmpMiddleZ, diff_mid);

  CanvasTriangle tmpTri_a = CanvasTriangle(triangle.vertices[0], mid, triangle.vertices[1]);
  CanvasTriangle tmpTri_b = CanvasTriangle(mid, triangle.vertices[1], triangle.vertices[2]);
  fillTriangleFlatDepth(tmpTri_a, brush);
  fillTriangleFlatDepth(tmpTri_b, brush);

}

CanvasTriangle sortTriangle(CanvasTriangle triangle)
{
  if (triangle.vertices[2].y < triangle.vertices[1].y){
    CanvasPoint tmp = triangle.vertices[1];
    triangle.vertices[1] = triangle.vertices[2];
    triangle.vertices[2] = tmp;
  }
  if (triangle.vertices[1].y < triangle.vertices[0].y){
    CanvasPoint tmp = triangle.vertices[0];
    triangle.vertices[0] = triangle.vertices[1];
    triangle.vertices[1] = tmp;
  }
  if (triangle.vertices[2].y < triangle.vertices[1].y){
    CanvasPoint tmp = triangle.vertices[1];
    triangle.vertices[1] = triangle.vertices[2];
    triangle.vertices[2] = tmp;
  }
  CanvasTriangle result = CanvasTriangle(triangle.vertices[0], triangle.vertices[1],
      triangle.vertices[2], triangle.colour);
  return result;
}

void fillTriangleFlatDepth(CanvasTriangle triangle, Colour brush)
{
  // need sort first!

  CanvasPoint p1 = triangle.vertices[0];
  CanvasPoint p2 = triangle.vertices[1];
  CanvasPoint p3 = triangle.vertices[2];
  int lineCount;
  CanvasPoint pLeft; CanvasPoint pRight; CanvasPoint pTop;
  if (p2.y == p3.y){
    lineCount = (int)round(p3.y) - (int)round(p1.y) + 1;
    pTop = p1;
    if (p2.x > p3.x){
      pLeft = p3; pRight = p2;
    }
    else{
      pLeft = p2; pRight = p3;
    }
  }
  else if (p1.y == p2.y){
    lineCount = (int)round(p3.y) - (int)round(p1.y) + 1;
    pTop = p3;
    if (p1.x > p2.x){
      pLeft = p2; pRight = p1;
    }
    else{
      pLeft = p1; pRight = p2;
    }
  }
  else{
    lineCount = (int)round(p2.y) - (int)round(p1.y) + 1;
    pTop = p2;
    if (p1.x > p3.x){
      pLeft = p3; pRight = p1;
    }
    else{
      pLeft = p1; pRight = p3;
    }
  }
  // draw
  if (lineCount < 2){
    if (pTop.x >= 0 && pTop.x < WIDTH && pTop.y >= 0 && pTop.y < HEIGHT){
      float red = brush.red * pTop.brightness; float green = brush.green*pTop.brightness; float blue = brush.blue* pTop.brightness;
      uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(pTop.x, pTop.y, colorInt);
    }
  }
  else{
    float* tmpY = interpolationf(pTop.y, pLeft.y, lineCount);
    float* tmpL = interpolationf(pTop.x, pLeft.x, lineCount);
    float* tmpLZ = interpolationf(pTop.depth, pLeft.depth, lineCount);
    float* tmpBriL = interpolationf(pTop.brightness, pLeft.brightness, lineCount);
    float* tmpR = interpolationf(pTop.x, pRight.x, lineCount);
    float* tmpRZ = interpolationf(pTop.depth, pRight.depth, lineCount);
    float* tmpBriR = interpolationf(pTop.brightness, pRight.brightness, lineCount);

    for (int j = 0; j < lineCount; j++){
      int tmpLineL = (int)round(*(tmpL+j));
      int tmpLineR = (int)round(*(tmpR+j));
      float BriL = *(tmpBriL+j);
      float BriR = *(tmpBriR+j);
      int pos_y = (int)round(*(tmpY+j));
      if (tmpLineR <= tmpLineL){
        if (tmpLineL >= 0 && tmpLineL < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
          if ((*tmpLZ) > depthBuffer[tmpLineL][pos_y]){
            float red = brush.red * BriL; float green = brush.green* BriL; float blue = brush.blue* BriL;
            uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

            window.setPixelColour(tmpLineL, pos_y, colorInt);
            depthBuffer[tmpLineL][pos_y] = *tmpLZ;
            // std::cout << "/* message */" << '\n';

          }
        }
      }
      else{
        int linePixelCount = tmpLineR - tmpLineL + 1;
        float tmpLineLZ = *(tmpLZ+j);
        float tmpLineRZ = *(tmpRZ+j);
        float* briLine = interpolationf(BriL, BriR, linePixelCount);
        float* tmpLineZ = interpolationf(tmpLineLZ, tmpLineRZ, linePixelCount);
        for (int i = 0; i < linePixelCount; i++){
          int pos_x = i + tmpLineL;
          if (pos_x >= 0 && pos_x < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
            if ((*(tmpLineZ+i)) > depthBuffer[pos_x][pos_y]){
              float red = brush.red * (*(briLine+i)); float green = brush.green* (*(briLine+i)); float blue = brush.blue* (*(briLine+i));
              uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
              window.setPixelColour(pos_x, pos_y, colorInt);
              depthBuffer[pos_x][pos_y] = *(tmpLineZ+i);
            }
          }
        }
        delete []tmpLineZ;
        delete []briLine;
      }
    }
    delete []tmpY; delete []tmpL; delete []tmpR;
    delete []tmpLZ; delete []tmpRZ; delete []tmpBriL; delete []tmpBriR;
  }
}

float getBrightness(vec3 vecPoint, vec3 lightPos, vec3 viewPos, vec3 normal, float specStrength){
  float ambiant_strength = 0.1;
  vec3 lightdir = normalize(lightPos - vecPoint);
  vec3 reflectDir = reflect(-lightdir, normal);

  float diff = std::max(double(glm::dot(lightdir, normal)),0.0);

  vec3 view = normalize(viewPos - vecPoint);
  float spec = std::pow(std::max(double(glm::dot(view, reflectDir)), 0.0), 32);
  float brightness = (specStrength * spec) + diff + ambiant_strength;
  if (brightness > 1){
    brightness = 1.0;
  }
  return brightness;
}

void fillTriangleFlatDepth_phong(CanvasTriangle triangle, Colour brush, MyPointLight lightsource)
{
  // need sort first!
  float specularStrength = 1.0;
  CanvasPoint p1 = triangle.vertices[0];
  CanvasPoint p2 = triangle.vertices[1];
  CanvasPoint p3 = triangle.vertices[2];
  int lineCount;
  CanvasPoint pLeft; CanvasPoint pRight; CanvasPoint pTop;
  if (p2.y == p3.y){
    lineCount = (int)round(p3.y) - (int)round(p1.y) + 1;
    pTop = p1;
    if (p2.x > p3.x){
      pLeft = p3; pRight = p2;
    }
    else{
      pLeft = p2; pRight = p3;
    }
  }
  else if (p1.y == p2.y){
    lineCount = (int)round(p3.y) - (int)round(p1.y) + 1;
    pTop = p3;
    if (p1.x > p2.x){
      pLeft = p2; pRight = p1;
    }
    else{
      pLeft = p1; pRight = p2;
    }
  }
  else{
    lineCount = (int)round(p2.y) - (int)round(p1.y) + 1;
    pTop = p2;
    if (p1.x > p3.x){
      pLeft = p3; pRight = p1;
    }
    else{
      pLeft = p1; pRight = p3;
    }
  }
  // draw
  if (lineCount < 2){
    if (pTop.x >= 0 && pTop.x < WIDTH && pTop.y >= 0 && pTop.y < HEIGHT){
      vec3 vecPoint = vec3(pTop.x, pTop.y, pTop.depth);
      float brightness = getBrightness(vecPoint, lightsource.position, vec3(0.0f, -2.0f, 5.0f), pTop.normal, specularStrength);
      float red = brush.red * brightness; float green = brush.green* brightness; float blue = brush.blue* brightness;
      uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(pTop.x, pTop.y, colorInt);
    }
  }
  else{
    float* tmpY = interpolationf(pTop.y, pLeft.y, lineCount);
    float* tmpL = interpolationf(pTop.x, pLeft.x, lineCount);
    float* tmpLZ = interpolationf(pTop.depth, pLeft.depth, lineCount);
    std::vector<vec3> tmpNormalL = vec3interpolation(pTop.normal, pLeft.normal, lineCount);
    float* tmpR = interpolationf(pTop.x, pRight.x, lineCount);
    float* tmpRZ = interpolationf(pTop.depth, pRight.depth, lineCount);
    std::vector<vec3> tmpNormalR = vec3interpolation(pTop.normal, pRight.normal, lineCount);

    for (int j = 0; j < lineCount; j++){
      int tmpLineL = (int)round(*(tmpL+j));
      int tmpLineR = (int)round(*(tmpR+j));
      vec3 normalL = tmpNormalL[j];
      vec3 normalR = tmpNormalR[j];
      int pos_y = (int)round(*(tmpY+j));
      if (tmpLineR <= tmpLineL){
        if (tmpLineL >= 0 && tmpLineL < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
          if ((*tmpLZ) > depthBuffer[tmpLineL][pos_y]){
            vec3 vecPoint = vec3(tmpLineL, pos_y,(*tmpLZ));
            float brightness = getBrightness(vecPoint, lightsource.position, vec3(0.0f, -2.0f, 5.0f), tmpNormalL[0], specularStrength);
            float red = brush.red * brightness; float green = brush.green* brightness; float blue = brush.blue* brightness;
            uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

            window.setPixelColour(tmpLineL, pos_y, colorInt);
            depthBuffer[tmpLineL][pos_y] = *tmpLZ;

          }
        }
      }
      else{
        int linePixelCount = tmpLineR - tmpLineL + 1;
        float tmpLineLZ = *(tmpLZ+j);
        float tmpLineRZ = *(tmpRZ+j);
        std::vector<vec3> normalLine = vec3interpolation(normalL, normalR, linePixelCount);
        float* tmpLineZ = interpolationf(tmpLineLZ, tmpLineRZ, linePixelCount);
        for (int i = 0; i < linePixelCount; i++){
          int pos_x = i + tmpLineL;
          if (pos_x >= 0 && pos_x < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
            if ((*(tmpLineZ+i)) > depthBuffer[pos_x][pos_y]){

              vec3 vecPoint = vec3(pos_x, pos_y, (*(tmpLineZ+i)));
              float brightness = getBrightness(vecPoint, lightsource.position, vec3(0.0f, -2.0f, 5.0f), normalLine[i], specularStrength);
              float red = brush.red * brightness; float green = brush.green* brightness; float blue = brush.blue* brightness;
              uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
              window.setPixelColour(pos_x, pos_y, colorInt);
              depthBuffer[pos_x][pos_y] = *(tmpLineZ+i);
            }
          }
        }
        delete []tmpLineZ;
      }
    }
    delete []tmpY; delete []tmpL; delete []tmpR;
    delete []tmpLZ; delete []tmpRZ;
  }
}





void fillTriangleDepth_phong(CanvasTriangle inputTriangle, Colour brush, MyPointLight lightsource)
{
  // swap
  CanvasTriangle triangle = sortTriangle(inputTriangle);
  if (triangle.vertices[0].y == triangle.vertices[1].y || triangle.vertices[0].y == triangle.vertices[2].y
      || triangle.vertices[1].y == triangle.vertices[2].y){
    fillTriangleFlatDepth(triangle, brush);
    return;
  }

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpZ2Z0 = triangle.vertices[2].depth - triangle.vertices[0].depth;
  vec3 tmpnormal = triangle.vertices[2].normal - triangle.vertices[0].normal;

  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;

  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;
  float tmpMiddleZ = triangle.vertices[0].depth + tmpZ2Z0 * tmpProgress;
  vec3 tmpNormalMid = vec3(triangle.vertices[0].normal.x + tmpnormal.x * tmpProgress,
                          triangle.vertices[0].normal.y + tmpnormal.y * tmpProgress,
                        triangle.vertices[0].normal.z + tmpnormal.z * tmpProgress);

  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y, tmpMiddleZ);
  mid.normal = tmpNormalMid;
  // std::cout << mid.normal.x << '\n';

  CanvasTriangle tmpTri_a = CanvasTriangle(triangle.vertices[0], mid, triangle.vertices[1]);
  CanvasTriangle tmpTri_b = CanvasTriangle(mid, triangle.vertices[1], triangle.vertices[2]);
  fillTriangleFlatDepth_phong(tmpTri_a, brush, lightsource);
  fillTriangleFlatDepth_phong(tmpTri_b, brush, lightsource);

}

float* interpolationf(float from, float to, int numOfVal)
{
  if (numOfVal <= 1){
    float* r = new float[1];
    *r = from;
    return r;
  }
    float* result = new float[numOfVal];
    float dist = to - from;
    float interval = dist / (numOfVal-1);
    for (int i = 0; i<numOfVal; i++){
      *(result+i) = from + i*interval;
    }
    return result;
}



void RT(){
  raytracer = MyRayTracer();
  raytracer.Render();
}

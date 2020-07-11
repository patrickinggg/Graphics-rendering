#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
// #include <MyObject.h>
#include <PatObject.h>
#include <algorithm>    // std::max
#include <AdjacentTable.h>
// #include <MyMaterial.h>
// #include <MyCamera.h>
// #include <MyMatrix.h>
// #include <MyRayTracer.h>
// #include <PatMaterial.h>
#include <MyTexture.h>
#include <map>
#include <math.h>
#include <unordered_map>
#include <MyPointLight.h>

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480

void readObject(float scale);

void draw();
void update();
void handleEvent(SDL_Event event);
std::vector<PatObject> obj_loader(string file_name);
unordered_map<string, PatMaterial> mat_loader(std::vector<string> mtl_files);
string convertToString(char* a, int size);
void renderWireFrame(mat4 world);
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
void fillTriangleFlatDepth(CanvasTriangle triangle);
CanvasTriangle sortTriangle(CanvasTriangle triangle);
void fillTriangleDepth(CanvasTriangle inputTriangle);
void drawFilledSceneWithLookAt(mat4 world);
void drawFilledSceneWithDepth_gouraud_shading(MyPointLight lightsource);
void fillTriangleDepth_phong(CanvasTriangle inputTriangle, PatMaterial brush, MyPointLight lightsource);
void fillTriangleFlatDepth_phong(CanvasTriangle triangle, PatMaterial brush, MyPointLight lightsource);
vec3 getBrightness(PatMaterial mat, vec3 vecPoint, vec3 lightPos, vec3 viewPos, vec3 normal, float specStrength);
float getIntensity(MyPointLight light, vec3 vecPoint);
vec3 get_ambiant(Pat_Colour colour, float intensity);
vec3 get_diffuse(Pat_Colour colour, vec3 normal, vec3 vecPoint, vec3 lightPos, float intensity);
vec3 get_specular(Pat_Colour colour, vec3 normal, vec3 vecPoint, vec3 lightPos, float intensity, vec3 viewPos);
vec3 phong_reflection_model(PatMaterial material, vec3 normal, vec3 vecPoint, vec3 lightPos, float intensity, float ambiant_intensity, vec3 viewPos);
std::vector<PatObject> obj_loader_normal(string file_name);
void save_ppm(DrawingWindow indow);
mat4 Xrotation(float rotation);
mat4 Zrotation(float rotation);

vector<AdjacentTable> adjacent_faces(string file_name);
void phong_shading(MyPointLight lightsource);

mat4 lookAt(vec3 from, vec3 to);


DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

string mtlFileName;
vector<vec3*> verticesRepo;
int framenum = 0;
vec3 camera = vec3(0.0f, -2.0f, 8.0f);
float depthBuffer[WIDTH][HEIGHT];
int objz = 5;

float rotationY = 3.14;
float rotationX = 3.14;
float rotationZ = 3.14;
vec3 lookAtfrom = vec3(0.0f, -2.0f, 8.0f);
MyPointLight light = MyPointLight(vec3(-0.3, 4.5, -2.517968));

int main(int argc, char* argv[])
{

  //---testing here---
  // readObject(1.0);
  // RT();
  // //------------------
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

}

mat4 Yrotation(float rotation){
  return mat4(cos(rotation), 0.0f, -sin(rotation), 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               sin(rotation), 0.0f, cos(rotation), 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
}

mat4 Xrotation(float rotation){
  return mat4(1.0f, 0.0f, 0.0f, 0.0f,
               0.0f, cos(rotation), sin(rotation), 0.0f,
               0.0f, -sin(rotation), cos(rotation), 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
}

mat4 Zrotation(float rotation){
  return mat4(cos(rotation), sin(rotation), 0.0f, 0.0f,
               -sin(rotation), cos(rotation), 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
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
  mat4 rotation_matrix_Y = Yrotation(rotationY);
  mat4 rotation_matrix_X = Xrotation(rotationX);
  mat4 rotation_matrix_Z = Zrotation(rotationZ);
  mat4 translationMatrix = toTranslationMatrix(camera);
  mat4 world = translationMatrix * rotation_matrix_Y * rotation_matrix_X * rotation_matrix_Z ;



  drawFilledSceneWithDepth_gouraud_shading(light);
  // renderWireFrame(world);
  // phong_shading(light);


}



void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)

  // rotationY += 0.005;
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) {
      window.clearPixels();
      light.position.x -= 1;
      // rotation += 0.05;

    }
    else if(event.key.keysym.sym == SDLK_RIGHT){
      window.clearPixels();
      light.position.x += 1;


    }
    else if(event.key.keysym.sym == SDLK_UP){
      window.clearPixels();
      // camera.y += 1;
      light.position.y += 1;
    }
    else if(event.key.keysym.sym == SDLK_DOWN){
      window.clearPixels();
      // camera.y -= 1;
      light.position.y -= 1;

    }
    else if(event.key.keysym.sym == SDLK_1){ // lookAt
      window.clearPixels();
      lookAtfrom.x += 0.10f;
    }
    else if(event.key.keysym.sym == SDLK_2){ // lookAt
      window.clearPixels();
      lookAtfrom.x -= 0.10f;
    }
    else if(event.key.keysym.sym == SDLK_3){ // lookAt
      window.clearPixels();
      lookAtfrom.y += 0.1f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';
    }
    else if(event.key.keysym.sym == SDLK_4){ // lookAt
      window.clearPixels();
      lookAtfrom.y -= 0.1f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';
    }
    else if(event.key.keysym.sym == SDLK_5){ // lookAt
      window.clearPixels();
      lookAtfrom.z += 0.1f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }
    else if(event.key.keysym.sym == SDLK_6){ // lookAt
      window.clearPixels();
      lookAtfrom.z -= 0.1f;
      std::cout << lookAtfrom.x << " "<< lookAtfrom.y << " " << lookAtfrom.z << '\n';

    }
    else if(event.key.keysym.sym == SDLK_a){
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationY -= 0.05;

    }
    else if(event.key.keysym.sym == SDLK_s){
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationY += 0.05;

    }
    else if(event.key.keysym.sym == SDLK_f){
      std::cout << "frame saved" << '\n';
      save_ppm(window);
      framenum += 1;
    }
    else if(event.key.keysym.sym == SDLK_q){//test reading obj_file
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationX += 0.05;
    }
    else if(event.key.keysym.sym == SDLK_w){
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationX -= 0.05;
    }
    else if(event.key.keysym.sym == SDLK_z){//render Wireframe
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationZ += 0.05;
    }
    else if(event.key.keysym.sym == SDLK_x){//render Wireframe
      // save_ppm(window);
      // framenum += 1;
      window.clearPixels();
      rotationZ -= 0.05;
    }
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}

// void readObject(float scale){
//   // ifstream f("cornell-box.obj", ios::in);
//   ifstream f("cb.obj", ios::in);
//   while(!f.eof()){
//   // for (int i = 0; i < 10; i++){
//     string str;
//     getline(f, str);
//     if (str.length() < 1){continue;}
//     string* segs = split(str, 32);
//     string arg = *segs;
//     if (arg == "mtllib"){
//       mtlFileName = *(segs+1);
//       ifstream f2(mtlFileName, ios::in);
//       MyMaterial* tmpMat = new MyMaterial();
//       while(!f2.eof()){
//         string str2;
//         getline(f2, str2);
//         if (str2.length() < 1){continue;}
//         string* segs2 = split(str2, 32);
//         string arg2 = *segs2;
//         if (arg2 == "newmtl"){
//           tmpMat = new MyMaterial();
//           matRepo[*(segs2+1)] = tmpMat;
//         }
//         else if (arg2 == "Kd"){
//           int tmpRed = (int)(255.0 * stof(*(segs2+1)));
//           int tmpGreen = (int)(255.0 * stof(*(segs2+2)));
//           int tmpBlue = (int)(255.0 * stof(*(segs2+3)));
//           Colour* tmpC = new Colour(tmpRed, tmpGreen, tmpBlue);
//           (*tmpMat).mat_color = tmpC;
//         }
//       }
//       f2.close();
//
//       // cout<<matRepo.size()<<endl;
//     }
//     else if(arg == "o"){
//       currentObj = new MyObject();
//       objs.push_back(currentObj);
//       (*currentObj).name = *(segs+1);
//       // cout<<"see obj: "<<(*currentObj).name<<endl;
//     }
//     else if(arg == "usemtl"){
//       MyMaterial* tmpMat = matRepo[*(segs+1)];
//       currentMat = tmpMat;
//     }
//     else if(arg == "v"){
//       float tmpX = stof(*(segs+1));
//       float tmpY = stof(*(segs+2));
//       float tmpZ = stof(*(segs+3));
//       vec3* tmpVec = new vec3(tmpX * scale, tmpY * scale, tmpZ * scale);
//       verticesRepo.push_back(tmpVec);
//       // cout<<"see v: "<<(*tmpVec).z<<endl;
//     }
//     else if(arg == "f"){
//       string* f_args1 = split(*(segs+1), '/');
//       string* f_args2 = split(*(segs+2), '/');
//       string* f_args3 = split(*(segs+3), '/');
//       int index1 = stoi(*f_args1);
//       int index2 = stoi(*f_args2);
//       int index3 = stoi(*f_args3);
//       ModelTriangle* tmpTri = new ModelTriangle();
//       tmpTri->vertices[0] = *verticesRepo[index1 - 1];
//       tmpTri->vertices[1] = *verticesRepo[index2 - 1];
//       tmpTri->vertices[2] = *verticesRepo[index3 - 1];
//       tmpTri->colour = *((*currentMat).mat_color);
//       (*currentObj).mesh.push_back(tmpTri);
//       // cout<<"see f: "<<index1<<endl;
//     }
//
//   }
//   f.close();
//
//   // for (int i = 0; i < (int)objs.size(); i++){
//   //   cout<<(*objs[i]).name<<endl;
//   // }
//   // cout<<(*(*objs[1]).mesh[0]).colour.red<<endl;
// }


std::vector<PatObject> obj_loader(string file_name)
{
  std::ifstream myfile;
  std::vector<string> mtl_file;
  std::vector<vec3> vertices;
  std::vector<vec3> face_normal;
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
  int d;
  int e;
  int f;

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

  while (myfile.good()) {
    myfile.getline(coordinate, 256);

    if(sscanf(coordinate, "vn %f %f %f", &x, &y, &z) == 3){
      // std::cout << x << " " << y << " " << z << '\n';
      vec3 normal = vec3(x, y, z);
      face_normal.push_back(normal);
    }
  }

  myfile.clear();                 // clear fail and eof bits
  myfile.seekg(0, std::ios::beg); // back to the start!


  unordered_map<string, PatMaterial> material = mat_loader(mtl_file);

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

    if(sscanf(line, "f %d/%d %d/%d %d/%d", &a, &b, &c, &d, &e, &f)){
      vec3 a_vertex = vertices[a-1];
      vec3 b_vertex = vertices[c-1];
      vec3 c_vertex = vertices[e-1];
      vec3 f_normal = face_normal[b-1];
      ModelTriangle triangle = ModelTriangle(a_vertex, b_vertex, c_vertex, material[current_colour]);
      triangle.face_normal = f_normal;

      triangles.push_back(triangle);
    }

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


unordered_map<string, PatMaterial> mat_loader(std::vector<string> mtl_files)
{
  unordered_map<string, PatMaterial> material;
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
        PatMaterial mat = PatMaterial();
        myfile.getline(line, 256);
        sscanf(line, "Kd %f %f %f", &red, &green, &blue);
        Pat_Colour c1 = Pat_Colour(red, green, blue);
        mat.diffuse_color = c1;

        sscanf(line, "Ka %f %f %f", &red, &green, &blue);
        Pat_Colour c2 = Pat_Colour(red, green, blue);
        mat.ambient_color = c2;

        sscanf(line, "Ks %f %f %f", &red, &green, &blue);
        Pat_Colour c3 = Pat_Colour(red, green, blue);
        mat.specular_color = c3;
        material[colour] = mat;
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

void renderWireFrame(mat4 world)
{
  window.clearPixels();
  std::vector<PatObject> objs = obj_loader("pat_cb.obj");
  mat4 rotation_matrix_Y = Yrotation(rotationY);
  mat4 rotation_matrix_X = Xrotation(rotationX);
  mat4 rotation_matrix_Z = Zrotation(rotationZ);

  // mat4 translationMatrix = toTranslationMatrix(camera);
  // mat4 world = translationMatrix * rotation_matrix_Y * rotation_matrix_X * rotation_matrix_Z ;

    for (int i = 0; i < objs.size(); i++) {
      PatObject object = objs[i];

      for (int j = 0; j < object.triangles.size(); j++) {
        ModelTriangle triangle = object.triangles[j];
        CanvasTriangle screenTriangle = CanvasTriangle();
        // std::cout << triangle << '\n';
        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];
          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH - projection.x * 20 ,
                                                  0.5 * HEIGHT + (-projection.y * 20 ),
                                                  (projection.z));
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
    mat4 rotation_matrix = Yrotation(rotationY);
    mat4 translationMatrix = toTranslationMatrix(vec3(0.0f, -2.0f, 5.0f));//camera position
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
        fillTriangleDepth(screenTriangle);
      }
    }
}


float getIntensity(MyPointLight light, vec3 vecPoint){
  float dis = glm::sqrt(std::pow((light.position.x - vecPoint.x), 2) + std::pow((light.position.y - vecPoint.y), 2) + std::pow((light.position.z - vecPoint.z), 2) ) ;
  // std::cout << "dis= " << dis << '\n';
  float intensity = 25 / (dis * dis + 1);
  return intensity;
}

vec3 get_ambiant(Pat_Colour colour, float intensity){
  // std::cout << colour.red << " " << colour.green << " " << colour.blue << '\n';
  return vec3(colour.red * intensity, colour.green * intensity, colour.blue * intensity);
}

vec3 get_diffuse(Pat_Colour colour, vec3 normal, vec3 vecPoint, vec3 lightPos, float intensity){
  vec3 lightdir = normalize(lightPos - vecPoint);
  float diffuse_intensity = std::max(glm::dot(normal, lightdir),0.0f);
  diffuse_intensity = diffuse_intensity * intensity;
  return vec3(colour.red * diffuse_intensity, colour.green * diffuse_intensity, colour.blue * diffuse_intensity);
}

vec3 get_specular(Pat_Colour colour, vec3 normal, vec3 vecPoint, vec3 lightPos, float intensity, vec3 viewPos){
  vec3 lightdir = normalize(lightPos - vecPoint);
  vec3 reflectDir = reflect(-lightdir, normal);

  vec3 view = normalize(viewPos - vecPoint);
  float spec = std::pow(std::max(double(glm::dot(view, reflectDir)), 0.0), 32);
  spec = spec * intensity;
  return vec3(colour.red * spec, colour.green * spec, colour.blue * spec);
}

vec3 phong_reflection_model(PatMaterial material, vec3 normal, vec3 vecPoint, vec3 lightPos,
                                float intensity, float ambiant_intensity, vec3 viewPos){
  vec3 ambiant = get_ambiant(material.ambient_color, ambiant_intensity);
  vec3 diffuse = get_diffuse(material.diffuse_color, normal, vecPoint, lightPos, intensity );
  vec3 specular = get_specular(material.specular_color, normal, vecPoint, lightPos, intensity, viewPos );
  vec3 sum = ambiant + diffuse + specular;

  if (sum.x > 1.0f){
    sum.x = 1.0f;
  }
  if (sum.y > 1.0f){
    sum.y = 1.0f;
  }
  if (sum.z > 1.0f){
    sum.z = 1.0f;
  }

  return sum;
}

void drawFilledSceneWithDepth_gouraud_shading(MyPointLight lightsource)
{
    mat4 rotation_matrix = Yrotation(rotationY);
    mat4 translationMatrix = toTranslationMatrix(camera);
    mat4 world = translationMatrix * rotation_matrix;
    std::vector<PatObject> objs = obj_loader("pat_cb.obj");

    vec4 cameralLight = world * vec4(lightsource.position.x, lightsource.position.y, lightsource.position.z, 1.0f);
    lightsource.position = vec3(cameralLight.x, cameralLight.y, cameralLight.z);
    // std::cout << lightsource.position.x << " " << lightsource.position.y << " " << lightsource.position.z << '\n';
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
        // vec3 normal = cross(a_b, a_c);
        // vec3 normal_unit = normalize(normal);
        vec3 normal_unit = triangle.face_normal;

        for (int k = 0; k < 3; k++) {
          vec3 vecPoint = triangle.vertices[k];

          vec4 cameraView = world * vec4(vecPoint.x ,vecPoint.y ,vecPoint.z ,1.0f);

          float intensity  = getIntensity(lightsource, vecPoint);
          vec3 overall_intensity = phong_reflection_model(triangle.material, normal_unit, vec3(cameraView.x, cameraView.y, cameraView.z), lightsource.position, intensity, 0.15, camera);

          vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH - projection.x * 20 ,
                                                  0.5 * HEIGHT + (-projection.y * 20 ),
                                                  (projection.z));
          screenTriangle.vertices[k].pat_brightness = overall_intensity;
        }
        fillTriangleDepth(screenTriangle);
      }
    }
}

void save_ppm(DrawingWindow indow){
  MyTexture saveFile = MyTexture();
  saveFile.tex_height = HEIGHT;
  saveFile.tex_width = WIDTH;
  saveFile.InitializeBuffer();
  saveFile.ReadScreen(indow);
  saveFile.SavePPM("frame" + std::to_string(framenum) + ".ppm");
  saveFile.Dispose();
}



void phong_shading(MyPointLight lightsource)
{

    mat4 rotation_matrix = Yrotation(rotationY);
    mat4 translationMatrix = toTranslationMatrix(camera);
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
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH - projection.x * 15 ,
                                                  0.5 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));
          float intensity = getIntensity(lightsource, vecPoint);
          screenTriangle.vertices[k].normal = normalize(normal);
          screenTriangle.vertices[k].intensity = intensity;

          // std::cout << normal.x << " " << normal.y << " " << normal.z << '\n';
        }
        screenTriangle.material = triangle.material;
        fillTriangleDepth_phong(screenTriangle, screenTriangle.material, lightsource);
      }
    }
}


void drawFilledSceneWithLookAt(mat4 world)
{
    mat4 rotation_matrix = Yrotation(rotationY);
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

           vec3 projection = vec3((cameraView.x/(cameraView.z)) * 8,
                                  (cameraView.y/(cameraView.z)) * 8,
                                  1 / cameraView.z);
          screenTriangle.vertices[k] = CanvasPoint(0.5 * WIDTH + projection.x * 15 ,
                                                  0.7 * HEIGHT + (-projection.y * 15 ),
                                                  (projection.z));

        }
        fillTriangleDepth(screenTriangle);
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


void fillTriangleDepth(CanvasTriangle inputTriangle)
{
  // swap
  CanvasTriangle triangle = sortTriangle(inputTriangle);
  if (triangle.vertices[0].y == triangle.vertices[1].y || triangle.vertices[0].y == triangle.vertices[2].y
      || triangle.vertices[1].y == triangle.vertices[2].y){
    fillTriangleFlatDepth(triangle);
    return;
  }

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpZ2Z0 = triangle.vertices[2].depth - triangle.vertices[0].depth;
  vec3 tmpbrightness = triangle.vertices[2].pat_brightness - triangle.vertices[0].pat_brightness;

  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;

  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;
  float tmpMiddleZ = triangle.vertices[0].depth + tmpZ2Z0 * tmpProgress;
  vec3 diff_mid = triangle.vertices[0].pat_brightness + tmpbrightness * tmpProgress;

  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y, tmpMiddleZ, diff_mid);

  CanvasTriangle tmpTri_a = CanvasTriangle(triangle.vertices[0], mid, triangle.vertices[1]);
  CanvasTriangle tmpTri_b = CanvasTriangle(mid, triangle.vertices[1], triangle.vertices[2]);
  fillTriangleFlatDepth(tmpTri_a);
  fillTriangleFlatDepth(tmpTri_b);

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

void fillTriangleFlatDepth(CanvasTriangle triangle)
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
      float red = 255 * pTop.pat_brightness.x; float green = 255* pTop.pat_brightness.y; float blue = 255* pTop.pat_brightness.z;

      uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(pTop.x, pTop.y, colorInt);
    }
  }
  else{
    float* tmpY = interpolationf(pTop.y, pLeft.y, lineCount);
    float* tmpL = interpolationf(pTop.x, pLeft.x, lineCount);
    float* tmpLZ = interpolationf(pTop.depth, pLeft.depth, lineCount);
    std::vector<vec3> tmpBriL = vec3interpolation(pTop.pat_brightness, pLeft.pat_brightness, lineCount);
    float* tmpR = interpolationf(pTop.x, pRight.x, lineCount);
    float* tmpRZ = interpolationf(pTop.depth, pRight.depth, lineCount);
    std::vector<vec3> tmpBriR = vec3interpolation(pTop.pat_brightness, pRight.pat_brightness, lineCount);

    for (int j = 0; j < lineCount; j++){
      int tmpLineL = (int)round(*(tmpL+j));
      int tmpLineR = (int)round(*(tmpR+j));
      vec3 BriL = tmpBriL[j];
      vec3 BriR = tmpBriR[j];
      int pos_y = (int)round(*(tmpY+j));
      if (tmpLineR <= tmpLineL){
        if (tmpLineL >= 0 && tmpLineL < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
          if ((*tmpLZ) > depthBuffer[tmpLineL][pos_y]){
            float red = 255 * BriL.x; float green = 255* BriL.y; float blue = 255* BriL.z;

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
        std::vector<vec3> briLine = vec3interpolation(BriL, BriR, linePixelCount);
        float* tmpLineZ = interpolationf(tmpLineLZ, tmpLineRZ, linePixelCount);
        for (int i = 0; i < linePixelCount; i++){
          int pos_x = i + tmpLineL;
          if (pos_x >= 0 && pos_x < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
            if ((*(tmpLineZ+i)) > depthBuffer[pos_x][pos_y]){
              float red = 255 * briLine[i].x; float green = 255* briLine[i].y; float blue = 255* briLine[i].z;

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

vec3 getBrightness(PatMaterial mat, vec3 vecPoint, vec3 lightPos, vec3 viewPos, vec3 normal, float specStrength){

  float ambiant_strength = 0.1;
  vec3 lightdir = normalize(lightPos - vecPoint);
  vec3 reflectDir = reflect(-lightdir, normal);
  float diff = std::max(double(glm::dot(lightdir, normal)),0.0);
  vec3 view = normalize(viewPos - vecPoint);
  float spec = std::pow(std::max(double(glm::dot(view, reflectDir)), 0.0), 32) * specStrength;

  int brightness_r = round(mat.ambient_color.red * ambiant_strength + mat.diffuse_color.red * diff + mat.specular_color.red * spec);
  int brightness_g = round(mat.ambient_color.green * ambiant_strength + mat.diffuse_color.green * diff + mat.specular_color.green * spec);
  int brightness_b = round(mat.ambient_color.blue * ambiant_strength + mat.diffuse_color.blue * diff + mat.specular_color.blue * spec);

  if (brightness_r > 255){
    brightness_r = 255;
  }
  if (brightness_g > 255){
    brightness_g = 255;
  }
  if (brightness_b > 255){
    brightness_b = 255;
  }
  // std::cout << brightness_r << " " << brightness_g << " " << brightness_b << '\n';
  vec3 brightness = vec3(brightness_r, brightness_g, brightness_b);
  return brightness;
}

void fillTriangleFlatDepth_phong(CanvasTriangle triangle, PatMaterial brush, MyPointLight lightsource)
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
      vec3 original_vecPoint = pTop.original_vecPoint;
      float intensity = pTop.intensity;
      vec3 overall_intensity = phong_reflection_model(brush, pTop.normal, original_vecPoint, lightsource.position, intensity, 0.2, camera);

      uint32_t colorInt = (255<<24) + (int(overall_intensity.x * 255) <<16) + (int(overall_intensity.y * 255) <<8) + int(overall_intensity.z * 255);
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
            vec3 original_vecPoint = vec3((vecPoint.x - 0.5 * WIDTH)/(-15.0), (vecPoint.y - 0.5 * HEIGHT)/(-15.0), vecPoint.z );

            // vec3 brightness = getBrightness(brush,vecPoint, lightsource.position, camera, tmpNormalL[0], specularStrength);
            float intensity = getIntensity(lightsource, original_vecPoint);
            vec3 overall_intensity = phong_reflection_model(brush, tmpNormalL[0], original_vecPoint, lightsource.position, intensity, 0.2, camera);
            uint32_t colorInt = (255<<24) + (int(overall_intensity.x * 255) <<16) + (int(overall_intensity.y * 255) <<8) + int(overall_intensity.z * 255);

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
              vec3 original_vecPoint = vec3((vecPoint.x - 0.5 * WIDTH)/(-15.0), (vecPoint.y - 0.5 * HEIGHT)/(-15.0), vecPoint.z );

              // vec3 brightness = getBrightness(brush, vecPoint, lightsource.position, camera, normalLine[i], specularStrength);
              float intensity = getIntensity(lightsource, original_vecPoint);
              vec3 overall_intensity = phong_reflection_model(brush,  normalLine[i], original_vecPoint, lightsource.position, intensity, 0.2, camera);
              uint32_t colorInt = (255<<24) + (int(overall_intensity.x * 255) <<16) + (int(overall_intensity.y * 255) <<8) + int(overall_intensity.z * 255);
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


void fillTriangleDepth_phong(CanvasTriangle inputTriangle, PatMaterial brush, MyPointLight lightsource)
{
  // swap
  CanvasTriangle triangle = sortTriangle(inputTriangle);
  if (triangle.vertices[0].y == triangle.vertices[1].y || triangle.vertices[0].y == triangle.vertices[2].y
      || triangle.vertices[1].y == triangle.vertices[2].y){
    fillTriangleFlatDepth_phong(triangle, brush, lightsource);
    return;
  }

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpZ2Z0 = triangle.vertices[2].depth - triangle.vertices[0].depth;
  vec3 tmpnormal = triangle.vertices[2].normal - triangle.vertices[0].normal;
  float tmpIntensity = triangle.vertices[2].intensity - triangle.vertices[0].intensity;

  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;

  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;
  float tmpMiddleZ = triangle.vertices[0].depth + tmpZ2Z0 * tmpProgress;
  vec3 tmpNormalMid = vec3(triangle.vertices[0].normal.x + tmpnormal.x * tmpProgress,
                          triangle.vertices[0].normal.y + tmpnormal.y * tmpProgress,
                        triangle.vertices[0].normal.z + tmpnormal.z * tmpProgress);
  float tmpIntensityMid = triangle.vertices[0].intensity + tmpIntensity * tmpProgress;
  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y, tmpMiddleZ);
  mid.normal = tmpNormalMid;
  mid.intensity = tmpIntensityMid;
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


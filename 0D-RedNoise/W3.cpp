#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <MyObject.h>
#include <MyMaterial.h>
#include <MyCamera.h>
#include <MyMatrix.h>
#include <map>
#include <math.h>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 320

float* interpolation(float from, float to, int numOfVal);
void drawLine(CanvasPoint p1, CanvasPoint p2, vec3 colour);
void drawTriangle(CanvasTriangle triangle);
void fillTriangle(CanvasTriangle triangle, Colour brush);
void fillTriangleFlat(CanvasTriangle triangle, Colour brush);
CanvasTriangle sortTriangle(CanvasTriangle triangle);
void fillTriangleFlatDepth(CanvasTriangle triangle, Colour brush);
void fillTriangleDepth(CanvasTriangle inputTriangle, Colour brush);
int depthTesting(int pos_x, int pos_y, float value);

void readObject(float scale);
void renderWireframes();
void renderPlainColor();
void renderColorDepth();

void draw();
void update();
void handleEvent(SDL_Event event);

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

int main(int argc, char* argv[])
{
  //---testing here---
  readObject(1.0);
  //------------------

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

void draw()
{
  // clear depth buffer
  for (int j = 0; j < HEIGHT; j++){
    for (int i = 0; i < WIDTH; i++){
      depthBuffer[i][j] = 0;
    }
  }

  window.clearPixels();
  for(int y=0; y<window.height ;y++) {
    for(int x=0; x<window.width ;x++) {
      float red = 120;
      float green = 120;
      float blue = 120;
      uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(x, y, colour);
    }
  }

  // CanvasPoint testP1 = CanvasPoint(200,20);
  // CanvasPoint testP2 = CanvasPoint(50,50);
  // CanvasPoint testP3 = CanvasPoint(100,270);
  // Colour testBrush = Colour(255,255,255);
  // CanvasTriangle testTri = CanvasTriangle(testP1, testP2, testP3);
  // fillTriangle(testTri, testBrush);

  // renderPlainColor();
  renderColorDepth();

  renderWireframes();


}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
  
  rotationY += 0.005;
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;

    else if(event.key.keysym.sym == SDLK_u){

    }
    else if(event.key.keysym.sym == SDLK_f){

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

void renderWireframes(){
  MyMatrix rot_y = MyMatrix(4);
  rot_y.setEye(1);
  rot_y.setV(1, 1, cos(rotationY));
  rot_y.setV(1, 3, sin(rotationY));
  rot_y.setV(3, 1, -sin(rotationY));
  rot_y.setV(3, 3, cos(rotationY));
  MyMatrix move_camera = MyMatrix(4);
  move_camera.setEye(1);
  move_camera.setV(1, 4, -plainCamera.position.x);
  move_camera.setV(2, 4, -plainCamera.position.y);
  move_camera.setV(3, 4, -plainCamera.position.z);
  MyMatrix mat_world = move_camera.dot(rot_y);

  for (int i = 0; i < (int)(objs.size()); i++){
    MyObject* tmpObj = objs[i];
    for (int j = 0; j < (int)((*tmpObj).mesh.size()); j++){
      ModelTriangle* tmpTri = (*tmpObj).mesh[j];
      CanvasTriangle screenTri = CanvasTriangle();
      for (int k = 0; k < 3; k++){
        vec3 vtx = (*tmpTri).vertices[k];
        // vtx = rotate180.dot3(vtx);
        // vtx = vtx - plainCamera.position;
        // MyMatrix projection = MyMatrix(3);
        // projection.setEye(plainCamera.f / vtx.z);
        // vec3 screenPoint = projection.dot3(vtx);
        // screenTri.vertices[k] = CanvasPoint(0.5*WIDTH*(1+screenPoint.x),
            // 0.5*HEIGHT*(1-screenPoint.y));
        
        vec3 vec_world = mat_world.dot3(vtx);
        vec3 vec_proj = vec3(vec_world.x * (plainCamera.f / vec_world.z), 
            vec_world.y * (plainCamera.f / vec_world.z), 
            plainCamera.f);
        
        screenTri.vertices[k] = CanvasPoint(0.5*WIDTH*(1+vec_proj.x),
            0.5*HEIGHT*(1-vec_proj.y));
      }
      screenTri.colour = Colour(0,0,0);
      drawTriangle(screenTri);
    }
  }
}

void renderPlainColor(){
  MyMatrix rot_y = MyMatrix(4);
  rot_y.setEye(1);
  rot_y.setV(1, 1, cos(rotationY));
  rot_y.setV(1, 3, sin(rotationY));
  rot_y.setV(3, 1, -sin(rotationY));
  rot_y.setV(3, 3, cos(rotationY));
  MyMatrix move_camera = MyMatrix(4);
  move_camera.setEye(1);
  move_camera.setV(1, 4, -plainCamera.position.x);
  move_camera.setV(2, 4, -plainCamera.position.y);
  move_camera.setV(3, 4, -plainCamera.position.z);
  MyMatrix mat_world = move_camera.dot(rot_y);

  for (int i = 0; i < (int)(objs.size()); i++){
    MyObject* tmpObj = objs[i];
    for (int j = 0; j < (int)((*tmpObj).mesh.size()); j++){
      ModelTriangle* tmpTri = (*tmpObj).mesh[j];
      CanvasTriangle screenTri = CanvasTriangle();
      for (int k = 0; k < 3; k++){
        vec3 vtx = (*tmpTri).vertices[k];
        vec3 vec_world = mat_world.dot3(vtx);
        vec3 vec_proj = vec3(vec_world.x * (plainCamera.f / vec_world.z), 
            vec_world.y * (plainCamera.f / vec_world.z), 
            plainCamera.f);
        
        screenTri.vertices[k] = CanvasPoint(0.5*WIDTH*(1+vec_proj.x),
            0.5*HEIGHT*(1-vec_proj.y));
      }
      fillTriangle(screenTri, (*tmpTri).colour);
    }
  }
}

void renderColorDepth(){
  MyMatrix rot_y = MyMatrix(4);
  rot_y.setRotateY(rotationY);
  MyMatrix move_camera = MyMatrix(4);
  move_camera.setTranslate(-plainCamera.position.x, -plainCamera.position.y, -plainCamera.position.z);
  MyMatrix mat_world = move_camera.dot(rot_y);

  for (int i = 0; i < (int)(objs.size()); i++){
    MyObject* tmpObj = objs[i];
    for (int j = 0; j < (int)((*tmpObj).mesh.size()); j++){
      ModelTriangle* tmpTri = (*tmpObj).mesh[j];
      CanvasTriangle screenTri = CanvasTriangle();
      for (int k = 0; k < 3; k++){
        vec3 vtx = (*tmpTri).vertices[k];
        vec3 vec_world = mat_world.dot3(vtx);
        vec3 vec_proj = vec3(vec_world.x * (plainCamera.f / vec_world.z), 
            vec_world.y * (plainCamera.f / vec_world.z), 
            1.0 / vec_world.z);
        
        screenTri.vertices[k] = CanvasPoint(0.5*WIDTH*(1+vec_proj.x),
            0.5*HEIGHT*(1-vec_proj.y), vec_proj.z);
      }
      fillTriangleDepth(screenTri, (*tmpTri).colour);
    }
  }
}

float* interpolation(float from, float to, int numOfVal){
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

void drawLine(CanvasPoint p1, CanvasPoint p2, Colour colour){
  float h_dist = p1.x - p2.x;
  if (h_dist < 0) {h_dist = - h_dist;}
  float v_dist = p1.y - p2.y;
  if (v_dist < 0) {v_dist = - v_dist;}
  int max_dist = (int)round(h_dist);
  if (v_dist > h_dist) {max_dist = (int)round(v_dist);}
  max_dist += 1;

  float* h_interp = interpolation(p1.x, p2.x, max_dist);
  float* v_interp = interpolation(p1.y, p2.y, max_dist);
  
  float red = colour.red;
  float green = colour.green;
  float blue = colour.blue;
  uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

  for (int i = 0; i < max_dist; i++){
    float x = *(h_interp+i);
    float y = *(v_interp+i);
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
      window.setPixelColour(x, y, colorInt);
    }
  }

  delete []h_interp;
  delete []v_interp;
}

void drawTriangle(CanvasTriangle triangle){
  drawLine(triangle.vertices[0], triangle.vertices[1], triangle.colour);
  drawLine(triangle.vertices[1], triangle.vertices[2], triangle.colour);
  drawLine(triangle.vertices[2], triangle.vertices[0], triangle.colour);
}

void fillTriangleFlat(CanvasTriangle triangle, Colour brush){
  // need sort first!
  float red = brush.red; float green = brush.green; float blue = brush.blue;
  uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

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
      window.setPixelColour(pTop.x, pTop.y, colorInt);
    }
  }
  else{
    float* tmpY = interpolation(pTop.y, pLeft.y, lineCount);
    float* tmpL = interpolation(pTop.x, pLeft.x, lineCount);
    float* tmpR = interpolation(pTop.x, pRight.x, lineCount);
    for (int j = 0; j < lineCount; j++){
      int tmpLineL = (int)round(*(tmpL+j));
      int tmpLineR = (int)round(*(tmpR+j));
      int pos_y = (int)round(*(tmpY+j));
      if (tmpLineR <= tmpLineL){
        if (tmpLineL >= 0 && tmpLineL < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
          window.setPixelColour(tmpLineL, pos_y, colorInt);
        }
      }
      else{
        for (int i = tmpLineL; i < tmpLineR; i++){
          int pos_x = i;
          if (pos_x >= 0 && pos_x < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
            window.setPixelColour(pos_x, pos_y, colorInt);
          }
        }
      }
    }
    delete []tmpY; delete []tmpL; delete []tmpR;
  }


}

void fillTriangle(CanvasTriangle inputTriangle, Colour brush){
  // swap
  CanvasTriangle triangle = sortTriangle(inputTriangle);
  if (triangle.vertices[0].y == triangle.vertices[1].y || triangle.vertices[0].y == triangle.vertices[2].y
      || triangle.vertices[1].y == triangle.vertices[2].y){
    fillTriangleFlat(triangle, brush);
    return;
  }

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * (triangle.vertices[1].y -
      triangle.vertices[0].y) / tmpY2Y0;
  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y);

  CanvasTriangle tmpTri_a = CanvasTriangle(triangle.vertices[0], mid, triangle.vertices[1]);
  CanvasTriangle tmpTri_b = CanvasTriangle(mid, triangle.vertices[1], triangle.vertices[2]);
  fillTriangleFlat(tmpTri_a, brush);
  fillTriangleFlat(tmpTri_b, brush);

}

void fillTriangleFlatDepth(CanvasTriangle triangle, Colour brush){
  // need sort first!
  float red = brush.red; float green = brush.green; float blue = brush.blue;
  uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

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
      int depthResult = depthTesting(pTop.x, pTop.y, pTop.depth);
      if (depthResult){
        window.setPixelColour(pTop.x, pTop.y, colorInt);
      }
    }
  }
  else{
    float* tmpY = interpolation(pTop.y, pLeft.y, lineCount);
    float* tmpL = interpolation(pTop.x, pLeft.x, lineCount);
    float* tmpLZ = interpolation(pTop.depth, pLeft.depth, lineCount);
    float* tmpR = interpolation(pTop.x, pRight.x, lineCount);
    float* tmpRZ = interpolation(pTop.depth, pRight.depth, lineCount);
    for (int j = 0; j < lineCount; j++){
      int tmpLineL = (int)round(*(tmpL+j));
      int tmpLineR = (int)round(*(tmpR+j));
      int pos_y = (int)round(*(tmpY+j));
      if (tmpLineR <= tmpLineL){
        if (tmpLineL >= 0 && tmpLineL < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
          int depthResult = depthTesting(tmpLineL, pos_y, *tmpLZ);
          if (depthResult){
            window.setPixelColour(tmpLineL, pos_y, colorInt);
          }
        }
      }
      else{
        int linePixelCount = tmpLineR - tmpLineL + 1;
        float tmpLineLZ = *(tmpLZ+j);
        float tmpLineRZ = *(tmpRZ+j);
        float* tmpLineZ = interpolation(tmpLineLZ, tmpLineRZ, linePixelCount);
        for (int i = 0; i < linePixelCount; i++){
          int pos_x = i + tmpLineL;
          if (pos_x >= 0 && pos_x < WIDTH && pos_y >= 0 && pos_y < HEIGHT){
            int depthResult = depthTesting(pos_x, pos_y, *(tmpLineZ+i));
            if (depthResult){
              window.setPixelColour(pos_x, pos_y, colorInt);
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

void fillTriangleDepth(CanvasTriangle inputTriangle, Colour brush){
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
  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;
  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;
  float tmpMiddleZ = triangle.vertices[0].depth + tmpZ2Z0 * tmpProgress;
  CanvasPoint mid = CanvasPoint(tmpMiddleX, triangle.vertices[1].y, tmpMiddleZ);

  CanvasTriangle tmpTri_a = CanvasTriangle(triangle.vertices[0], mid, triangle.vertices[1]);
  CanvasTriangle tmpTri_b = CanvasTriangle(mid, triangle.vertices[1], triangle.vertices[2]);
  fillTriangleFlatDepth(tmpTri_a, brush);
  fillTriangleFlatDepth(tmpTri_b, brush);

}

CanvasTriangle sortTriangle(CanvasTriangle triangle){
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

int depthTesting(int pos_x, int pos_y, float value){
  if (value > depthBuffer[pos_x][pos_y]){
    depthBuffer[pos_x][pos_y] = value;
    return 1;
  }
  return 0;
}

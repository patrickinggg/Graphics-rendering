#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

void draw();
void update();
void handleEvent(SDL_Event event);
void drawImageFile();

float* interpolation(float from, float to, int numOfVal);
void drawLine(CanvasPoint p1, CanvasPoint p2, vec3 colour);
void drawTriangle(CanvasTriangle triangle);
void fillTriangle(CanvasTriangle triangle, Colour brush);
void fillTexture(CanvasTriangle triangle);
void sortTriangle(CanvasTriangle triangle);
uint32_t getTextureColour(float x, float y);
CanvasTriangle* getRandomTriangle();

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
DrawingWindow window2 = DrawingWindow(480, 395, false);

Colour* tmpColor = new Colour(200,200,255);
CanvasPoint* p1 = new CanvasPoint(10,20);
CanvasPoint* p2 = new CanvasPoint(200,100);
CanvasPoint* p3 = new CanvasPoint(100,200);
CanvasTriangle* tri1 = new CanvasTriangle(*p1, *p2, *p3, *tmpColor);

int imgWidth;
int imgHeight;
char texture[480*395*3+3];

int main(int argc, char* argv[])
{
  //---testing here---
  SDL_Event event2;
  if (window2.pollForInputEvents(&event2)){};
  drawImageFile();
  window2.renderFrame();

  tri1->vertices[0].texturePoint = TexturePoint(195,5);
  tri1->vertices[1].texturePoint = TexturePoint(395,380);
  tri1->vertices[2].texturePoint = TexturePoint(65,330);
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

void drawImageFile(){
  char buffer1[100];
  int bufferCursor = 0;
  int pixelCursor = 0;
  char status = 0;
  int readSpeed = 1;
  ifstream f("texture.ppm", ios::binary);

  while(!f.eof()){
    char buffer2[3];
    f.read(buffer2, readSpeed);
    if (status == 3){  // content
      for (int rgbIndex = 0; rgbIndex < 3; rgbIndex++){
        texture[pixelCursor + rgbIndex] = buffer2[rgbIndex];
      }
      pixelCursor += 3;
    }
    else if (status < 10){  // header
      if (buffer2[0] == '#'){
        status += 10;
      }
      else if (buffer2[0] == 10){
        if (status == 0){
          // P6
        }
        else if (status == 1){  // width height
          string s = buffer1;
          int spaceAt = s.find(32);
          imgWidth = stoi(s.substr(0,spaceAt));
          imgHeight = stoi(s.substr(spaceAt));
        }
        else if (status == 2){
          // 255
        }
        status += 1;
        bufferCursor = 0;
        if (status == 3){
          readSpeed = 3;
        }
      }
      else{
        buffer1[bufferCursor] = buffer2[0];
        bufferCursor += 1;
      }
    }
    else{  // comment
      if (buffer2[0] == 10){
        status -= 10;
      }
    }

  }
  f.close();
  cout<<imgWidth<<endl;
  cout<<imgHeight<<endl;
  
  window2.clearPixels();
  for(int y=0; y<window2.height ;y++) {
    for(int x=0; x<window2.width ;x++) {
      int red = texture[3*(y*imgWidth+x)];
      if (red < 0){red += 256;}
      int green = texture[3*(y*imgWidth+x)+1];
      if (green < 0){green += 256;}
      int blue = texture[3*(y*imgWidth+x)+2];
      if (blue < 0){blue += 256;}
      uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window2.setPixelColour(x, y, colour);
    }
  }
}

void draw()
{
  window.clearPixels();
  for(int y=0; y<window.height ;y++) {
    for(int x=0; x<window.width ;x++) {
      float red = 255;
      float green = 255;
      float blue = 255;
      uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(x, y, colour);
    }
  }

  // ------------task 1-----------------
  // drawLine(*p1, *p2, *tmpColor);

  //-------------task 3--------------
  //fillTriangle(*tri1, *tmpColor);

  //-------------task 5-----------
  fillTexture(*tri1);

  //--------------task 2----------------
  drawTriangle(*tri1);
  
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
  
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;

    else if(event.key.keysym.sym == SDLK_u){
      //--------------task 2--------------
      CanvasTriangle* tri2 = tri1;
      tri1 = getRandomTriangle();
      delete tri2;
    }
    else if(event.key.keysym.sym == SDLK_f){
      //------------task 3---------------
      CanvasTriangle* tri2 = tri1;
      tri1 = getRandomTriangle();
      delete tri2;
      Colour* color2 = tmpColor;
      tmpColor = new Colour(rand()%255, rand()%255, rand()%255);
      delete color2;
    }
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
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
  int max_dist = (int)(h_dist + 0.5);
  if (v_dist > h_dist) {max_dist = (int)(v_dist + 0.5);}
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
    window.setPixelColour(x, y, colorInt);
  }

  delete []h_interp;
  delete []v_interp;
}

void drawTriangle(CanvasTriangle triangle){
  drawLine(triangle.vertices[0], triangle.vertices[1], triangle.colour);
  drawLine(triangle.vertices[1], triangle.vertices[2], triangle.colour);
  drawLine(triangle.vertices[2], triangle.vertices[0], triangle.colour);
}

void fillTriangle(CanvasTriangle triangle, Colour brush){
  // swap
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
  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * (triangle.vertices[1].y -
      triangle.vertices[0].y) / tmpY2Y0;

  float red = brush.red;
  float green = brush.green;
  float blue = brush.blue;
  uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

  int lineCount = 0;
  float* lineLeft;
  float* lineRight;
  // fill 1
  lineCount = (int)(triangle.vertices[1].y + 0.5) - (int)(triangle.vertices[0].y+0.5) + 1;
  float* lineY = interpolation(triangle.vertices[0].y, triangle.vertices[1].y, lineCount);
  if (tmpMiddleX <= triangle.vertices[1].x){
    lineLeft = interpolation(triangle.vertices[0].x, tmpMiddleX, lineCount);
    lineRight = interpolation(triangle.vertices[0].x, triangle.vertices[1].x, lineCount);
  }
  else{
    lineLeft = interpolation(triangle.vertices[0].x, triangle.vertices[1].x, lineCount);
    lineRight = interpolation(triangle.vertices[0].x, tmpMiddleX, lineCount);
  }
  for (int j = 0; j < lineCount; j++){
    for (int i = (int)(*(lineLeft+j) + 0.5); i < (int)(*(lineRight+j) + 0.5); i++){
      window.setPixelColour(i, (int)(*(lineY+j) + 0.5), colorInt);
    }
  }
  delete []lineY; delete []lineLeft; delete []lineRight;  // free
  // fill 2
  lineCount = (int)(triangle.vertices[2].y + 0.5) - (int)(triangle.vertices[1].y+0.5) + 1;
  lineY = interpolation(triangle.vertices[1].y, triangle.vertices[2].y, lineCount);
  if (tmpMiddleX <= triangle.vertices[1].x){
    lineLeft = interpolation(tmpMiddleX, triangle.vertices[2].x, lineCount);
    lineRight = interpolation(triangle.vertices[1].x, triangle.vertices[2].x, lineCount);
  }
  else{
    lineLeft = interpolation(triangle.vertices[1].x, triangle.vertices[2].x, lineCount);
    lineRight = interpolation(tmpMiddleX, triangle.vertices[2].x, lineCount);
  }
  for (int j = 0; j < lineCount; j++){
    for (int i = (int)(*(lineLeft+j) + 0.5); i <= (int)(*(lineRight+j) + 0.5); i++){
      window.setPixelColour(i, (int)(*(lineY+j) + 0.5), colorInt);
    }
  }
  delete []lineY; delete []lineLeft; delete []lineRight;  //free

}

void sortTriangle(CanvasTriangle triangle){
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
}

void fillTexture(CanvasTriangle triangle){
  // swap
  sortTriangle(triangle);

  // divide
  float tmpX2X0 = triangle.vertices[2].x - triangle.vertices[0].x;
  float tmpY2Y0 = triangle.vertices[2].y - triangle.vertices[0].y;
  float tmpProgress = (triangle.vertices[1].y - triangle.vertices[0].y) / tmpY2Y0;
  float tmpMiddleX = triangle.vertices[0].x + tmpX2X0 * tmpProgress;

  float tmpTexX2X0 = triangle.vertices[2].texturePoint.x - triangle.vertices[0].texturePoint.x;
  float tmpTexY2Y0 = triangle.vertices[2].texturePoint.y - triangle.vertices[0].texturePoint.y;
  float tmpTexMiddleX = triangle.vertices[0].texturePoint.x + tmpTexX2X0 * tmpProgress;
  float tmpTexMiddleY = triangle.vertices[0].texturePoint.y + tmpTexY2Y0 * tmpProgress;

  int lineCount = 0;
  float* lineLeft;
  float* lineRight;
  float* texLeftX;
  float* texRightX;
  float* texLeftY;
  float* texRightY;
  // fill 1
  lineCount = (int)(triangle.vertices[1].y + 0.5) - (int)(triangle.vertices[0].y+0.5) + 1;
  float* lineY = interpolation(triangle.vertices[0].y, triangle.vertices[1].y, lineCount);
  if (tmpMiddleX <= triangle.vertices[1].x){
    lineLeft = interpolation(triangle.vertices[0].x, tmpMiddleX, lineCount);
    lineRight = interpolation(triangle.vertices[0].x, triangle.vertices[1].x, lineCount);
    texLeftX = interpolation(triangle.vertices[0].texturePoint.x, tmpTexMiddleX, lineCount);
    texRightX = interpolation(triangle.vertices[0].texturePoint.x, triangle.vertices[1].texturePoint.x
        , lineCount);
    texLeftY = interpolation(triangle.vertices[0].texturePoint.y, tmpTexMiddleY, lineCount);
    texRightY = interpolation(triangle.vertices[0].texturePoint.y, triangle.vertices[1].texturePoint.y
        , lineCount);
  }
  else{
    lineLeft = interpolation(triangle.vertices[0].x, triangle.vertices[1].x, lineCount);
    lineRight = interpolation(triangle.vertices[0].x, tmpMiddleX, lineCount);
    texLeftX = interpolation(triangle.vertices[0].texturePoint.x, triangle.vertices[1].texturePoint.x
        , lineCount);
    texRightX = interpolation(triangle.vertices[0].texturePoint.x, tmpTexMiddleX, lineCount);
    texLeftY = interpolation(triangle.vertices[0].texturePoint.y, triangle.vertices[1].texturePoint.y
        , lineCount);
    texRightY = interpolation(triangle.vertices[0].texturePoint.y, tmpTexMiddleY, lineCount);
  }

  for (int j = 0; j < lineCount; j++){
    int linePixelCount = (int)(*(lineRight+j) + 0.5) - (int)(*(lineLeft+j) + 0.5) + 1;
    float* texLineX = interpolation(*(texLeftX+j), *(texRightX+j), linePixelCount);
    float* texLineY = interpolation(*(texLeftY+j), *(texRightY+j), linePixelCount);
    for (int i = 0; i < linePixelCount; i++){
      int drawX = (int)(*(lineLeft+j) + 0.5) + i;
      uint32_t colorInt = getTextureColour(*(texLineX+i), *(texLineY+i));
      window.setPixelColour(drawX, (int)(*(lineY+j) + 0.5), colorInt);
    }
  }
  delete []lineY; delete []lineLeft; delete []lineRight;  //free
  delete []texLeftX; delete [] texRightX; delete [] texLeftY; delete [] texRightY;

  // fill 2
  lineCount = (int)(triangle.vertices[2].y + 0.5) - (int)(triangle.vertices[1].y+0.5) + 1;
  lineY = interpolation(triangle.vertices[1].y, triangle.vertices[2].y, lineCount);
  if (tmpMiddleX <= triangle.vertices[1].x){
    lineLeft = interpolation(tmpMiddleX, triangle.vertices[2].x, lineCount);
    lineRight = interpolation(triangle.vertices[1].x, triangle.vertices[2].x, lineCount);
    texLeftX = interpolation(tmpTexMiddleX, triangle.vertices[2].texturePoint.x, lineCount);
    texRightX = interpolation(triangle.vertices[1].texturePoint.x, triangle.vertices[2].texturePoint.x
        , lineCount);
    texLeftY = interpolation(tmpTexMiddleY, triangle.vertices[2].texturePoint.y, lineCount);
    texRightY = interpolation(triangle.vertices[1].texturePoint.y, triangle.vertices[2].texturePoint.y
        , lineCount);
  }
  else{
    lineLeft = interpolation(triangle.vertices[1].x, triangle.vertices[2].x, lineCount);
    lineRight = interpolation(tmpMiddleX, triangle.vertices[2].x, lineCount);
    texLeftX = interpolation(triangle.vertices[1].texturePoint.x, triangle.vertices[2].texturePoint.x
        , lineCount);
    texRightX = interpolation(tmpTexMiddleX, triangle.vertices[2].texturePoint.x, lineCount);
    texLeftY = interpolation(triangle.vertices[1].texturePoint.y, triangle.vertices[2].texturePoint.y
        , lineCount);
    texRightY = interpolation(tmpTexMiddleY, triangle.vertices[2].texturePoint.y, lineCount);
  }

  for (int j = 0; j < lineCount; j++){
    int linePixelCount = (int)(*(lineRight+j) + 0.5) - (int)(*(lineLeft+j) + 0.5) + 1;
    float* texLineX = interpolation(*(texLeftX+j), *(texRightX+j), linePixelCount);
    float* texLineY = interpolation(*(texLeftY+j), *(texRightY+j), linePixelCount);
    for (int i = 0; i < linePixelCount; i++){
      int drawX = (int)(*(lineLeft+j) + 0.5) + i;
      uint32_t colorInt = getTextureColour(*(texLineX+i), *(texLineY+i));
      window.setPixelColour(drawX, (int)(*(lineY+j) + 0.5), colorInt);
    }
  }
  delete []lineY; delete []lineLeft; delete []lineRight;  //free
  delete []texLeftX; delete [] texRightX; delete [] texLeftY; delete [] texRightY;

}

CanvasTriangle* getRandomTriangle(){
  Colour* tmpTriColor = new Colour(rand()%255, rand()%255, rand()%255);
  CanvasPoint* tmpP1 = new CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
  CanvasPoint* tmpP2 = new CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
  CanvasPoint* tmpP3 = new CanvasPoint(rand()%WIDTH, rand()%HEIGHT);
  CanvasTriangle* tmpTri = new CanvasTriangle(*tmpP1, *tmpP2, *tmpP3, *tmpTriColor);
  return tmpTri;
}

uint32_t getTextureColour(float x, float y){
  int ix = (int)(x + 0.5);
  int iy = (int)(y + 0.5);
  int red = texture[3*(iy*imgWidth+ix)];
  if (red < 0){red += 256;}
  int green = texture[3*(iy*imgWidth+ix)+1];
  if (green < 0){green += 256;}
  int blue = texture[3*(iy*imgWidth+ix)+2];
  if (blue < 0){blue += 256;}
  uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
  return colour;
}

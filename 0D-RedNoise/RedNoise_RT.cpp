#define WIDTH 640
#define HEIGHT 480

#include <string>
#include <math.h>
#include <random>
#include "Colour.h"
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <map>
#include <MyObjFileManager.h>
#include <MyMatrix.h>
#include <MyCamera.h>
#include <MyPointLight.h>
#include <MyParticle.h>
#include <MyPhysics.h>
#include <MyRayTracer.h>
#include <MyRasteriser.h>


using namespace std;
using namespace glm;

void draw();
void update();
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
MyObjFileManager objReader = MyObjFileManager();
vector<MyObject*> objs;
vector<MyPointLight*> lights;
int frame = 0;

MyCamera plainCamera = MyCamera();
MyRayTracer raytracer = MyRayTracer();
MyRasteriser rasteriser = MyRasteriser();

int main(int argc, char* argv[])
{
  //---testing here---
  plainCamera.position = vec3(0,2,5);
  objs = objReader.readObject("cb.obj", 1.0);
  MyPointLight* light1 = new MyPointLight();
  light1->position = vec3(-2.5,1,-1);
  light1->energy = 12.0;
  light1->lightColour = Colour(255,130,160);
  lights.push_back(light1);
  MyPointLight* light2 = new MyPointLight();
  light2->position = vec3(1,4.5,-3);
  light2->energy = 30.0;
  light2->lightColour = Colour(255,255,255);
  lights.push_back(light2);
  plainCamera.UpdateDirection();

  raytracer.Initialize(window, objs, lights, &plainCamera);
  raytracer.enableFog = 0;
  raytracer.fogBrush = Colour(0,0,0);
  raytracer.fogDensity = 0.0;
  // raytracer.Render();

  rasteriser.Initialize(window, objs, lights, &plainCamera);


  // MyTexture saveFile = MyTexture();
  // saveFile.tex_height = HEIGHT;
  // saveFile.tex_width = WIDTH;
  // saveFile.InitializeBuffer();
  // saveFile.ReadScreen(window);
  // saveFile.SavePPM("testout.ppm");
  // saveFile.Dispose();
  //------------------

  SDL_Event event;
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);

    update();

    // draw();

    if (frame >= 322 && frame < 361){
      raytracer.Render();

      MyTexture saveFile = MyTexture();
      saveFile.tex_height = HEIGHT;
      saveFile.tex_width = WIDTH;
      saveFile.InitializeBuffer();
      saveFile.ReadScreen(window);

      // float blur_sigma = 0.66;
      // blur_sigma = 2.66 - (frame - 180.0) * 0.1;
      // int blur_size = ceil(blur_sigma * 3);
      // MyTexture blurFile = MyTexture();
      // blurFile.tex_height = HEIGHT;
      // blurFile.tex_width = WIDTH;
      // blurFile.InitializeBuffer();
      // for (int i = 0; i < WIDTH; i++){
      //   for (int j = 0; j < HEIGHT; j++){
      //     float tmpR = 0.0; float tmpG = 0.0; float tmpB = 0.0;
      //     for (int m = -blur_size; m <= blur_size; m++){
      //       for (int n = -blur_size; n <= blur_size; n++){
      //         int tmpX = i+m; int tmpY = j+n;
      //         if (tmpX >= 0 && tmpX < WIDTH && tmpY >= 0 && tmpY < HEIGHT){
      //           unsigned char* tmpColour = saveFile.GetRawPixel(tmpX, tmpY);
      //           float tmpWeight = (1.0 / (2.0 * 3.1416 * blur_sigma * blur_sigma)) 
      //                 * pow(2.71828, -((pow(m * 1.0, 2) + pow(n * 1.0, 2)) / (2 * blur_sigma * blur_sigma)));
      //           tmpR += (float)(tmpColour[0]) * tmpWeight;
      //           tmpG += (float)(tmpColour[1]) * tmpWeight;
      //           tmpB += (float)(tmpColour[2]) * tmpWeight;
      //           delete[] tmpColour;
      //         }
      //       }
      //     }
      //     blurFile.SetRawPixelColour(i, j, (int)tmpR, (int)tmpG, (int)tmpB);
      //   }
      // }
      // string filename = std::to_string(frame);
      // blurFile.SavePPM("frame" + filename + ".ppm");
      // saveFile.Dispose();
      // blurFile.Dispose();

      string filename = std::to_string(frame);
      saveFile.SavePPM("frame" + filename + ".ppm");
      saveFile.Dispose();
    }


    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

void draw()
{
  // // clear depth buffer
  // for (int j = 0; j < HEIGHT; j++){
  //   for (int i = 0; i < WIDTH; i++){
  //     depthBuffer[i][j] = 0;
  //   }
  // }

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

  // raytracer.Render();
  rasteriser.RenderWireFrame();
}

void update()
{
  frame += 1;
  // (*(objs[6])).Translate(vec3(0, 0.01*sin(frame*3.14/180.0), 0));
  (*(objs[3])).DeltaUV(0.004, -0.004, -1);
  if (frame <= 360){
    plainCamera.position.x += 0.005*sin(frame*3.14159/180.0);
    plainCamera.position.y = 2.0 + 0.3*sin(frame*3.14159/180.0);
    plainCamera.LookAt(vec3(0,2.0,0));
  }
  if (frame >= 220 && frame < 260){
    raytracer.SoftShadowDistance = 0.1 + (frame-220)*0.01;
    raytracer.SoftShadowElevation = 5 + (int)((frame-220) / 5);
  }
  if (frame >= 260 && frame < 300){
    raytracer.enableNoise = 1;
    raytracer.nd = std::normal_distribution<double>(0,0.01);
  }
  if (frame >= 300 && frame <= 330){
    raytracer.enableNoise = 0;
    float fogProg = frame-300.0;
    raytracer.enableFog = 1;
    raytracer.fogDensity = fogProg*0.1 / 30.0;
  }
  if (frame >= 330 && frame <= 360){
    float fogProg = 360.0-frame;
    raytracer.enableFog = 1;
    raytracer.fogDensity = fogProg*0.1 / 30.0;
    raytracer.fogBrush = Colour(120,120,120);
  }
  if (frame >= 361){
    exit(0);
  }
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


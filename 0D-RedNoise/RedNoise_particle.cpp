#define WIDTH 640
#define HEIGHT 480

#include <string>
#include <math.h>
#include <random>
#include <stdlib.h>
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

  rasteriser.Initialize(window, objs, lights, &plainCamera);

  MyPhysicsForce* wind1 = new MyPhysicsForce();
  wind1->forceValue = vec3(-2.0, 0, 0);
  rasteriser.physicsEngine.forceField.push_back(wind1);
  rasteriser.physicsEngine.gravityValue.y = -1.0;

  SDL_Event event;
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);

    update();

    draw();

      MyTexture saveFile = MyTexture();
      saveFile.tex_height = HEIGHT;
      saveFile.tex_width = WIDTH;
      saveFile.InitializeBuffer();
      saveFile.ReadScreen(window);
      string filename = std::to_string(frame);
      saveFile.SavePPM("wire_frame" + filename + ".ppm");
      saveFile.Dispose();

    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

void draw()
{
  rasteriser.RenderBackground();
  rasteriser.RenderWireFrame();
  rasteriser.RenderLightTag();
  rasteriser.RenderParticleTag();

}

void update()
{
  frame += 1;

  int newParCount = rand()%3;
  for (int i = 0; i < newParCount; i++){
    float tmpX = ((rand()%100)-50.0)*5.0 / 50.0;
    float tmpZ = ((rand()%100)-100.0)*10.0 / 100.0;
    vec3 tmpPos = vec3(tmpX, 10, tmpZ);
    MyParticle* tmpPar = new MyParticle();
    tmpPar->position = tmpPos;
    tmpPar->particleBrush = Colour(rand()%255,rand()%255,rand()%255);

    rasteriser.targetParticles.push_back(tmpPar);

    MyPhysicsTarget* tmpRigid = new MyPhysicsTarget(1); // 1 is particle
    tmpRigid->SetRefPar(tmpPar);
    tmpRigid->mass = (rand()%100) * 1.0;
    tmpRigid->applyGravity = 1;

    rasteriser.physicsEngine.targetObjects.push_back(tmpRigid);
  }
  rasteriser.physicsEngine.RunFrame();

  if (frame > 600){
    //exit(0);
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


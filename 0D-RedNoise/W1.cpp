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

float* interpolation(float from, float to, int numOfVal);
vec3* interpolation3D(vec3 from, vec3 to, int numOfVal);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char* argv[])
{

  // vec3* interp = interpolation3D(vec3(1,4,9.2), vec3(4,1,9.8), 4);
  // for (int i = 0; i < 4; i++){
  //   printf("%f ", (*(interp+i)).x);
  //   printf("%f ", (*(interp+i)).y);
  //   printf("%f\n", (*(interp+i)).z);
  // }


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
  // ----------------RedNoise code-------------------
  // window.clearPixels();
  // for(int y=0; y<window.height ;y++) {
  //   for(int x=0; x<window.width ;x++) {
  //     float red = rand() % 255;
  //     float green = 0.0;
  //     float blue = 0.0;
  //     uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
  //     window.setPixelColour(x, y, colour);
  //   }
  // }

  //---------task 3---------------
  // window.clearPixels();
  // float* horizontal = interpolation(255,0,320);
  // for(int y=0; y<window.height ;y++) {
  //   for(int x=0; x<window.width ;x++) {
  //     float gray = *(horizontal+x);
  //     float red = gray;
  //     float green = gray;
  //     float blue = gray;
  //     uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
  //     window.setPixelColour(x, y, colour);
  //   }
  // }

  //---------task 5---------------
  window.clearPixels();
  vec3* ver_left = interpolation3D(vec3(255,0,0), vec3(255,255,0), HEIGHT);  // red to yellow
  vec3* ver_right = interpolation3D(vec3(0,0,255), vec3(0,255,0), HEIGHT);  // blue to green
  for(int y=0; y<window.height ;y++) {
    vec3* hor = interpolation3D(*(ver_left+y), *(ver_right+y), WIDTH);

    for(int x=0; x<window.width ;x++) {
      vec3 color = *(hor+x);
      float red = color.x;
      float green = color.y;
      float blue = color.z;
      uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
      window.setPixelColour(x, y, colour);
    }
  }


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
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}

float* interpolation(float from, float to, int numOfVal){
    float* result = new float[numOfVal];
    float dist = to - from;
    float interval = dist / (numOfVal-1);
    for (int i = 0; i<numOfVal; i++){
      *(result+i) = from + i*interval;
    }
    return result;
}

vec3* interpolation3D(vec3 from, vec3 to, int numOfVal){
  vec3* result = new vec3[numOfVal];
  float dist[3] = {to.x - from.x, to.y - from.y, to.z - from.z};
  float interval[3] = {dist[0] / (numOfVal-1), dist[1] / (numOfVal-1), dist[2] / (numOfVal-1)};
  for (int i = 0; i<numOfVal; i++){
    *(result+i) = vec3(from.x + i*interval[0], from.y + i*interval[1], from.z + i*interval[2]) ;
  }
  return result;
}


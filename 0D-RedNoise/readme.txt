Contributor:

Frank Chen | Patrick Liu
======================================

>>How to use RayTracer module:

>>First, define the rendering size:

		#define WIDTH 640
		#define HEIGHT 480

>>Declare all dependencies, the suggesting order is:

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

		DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
		MyObjFileManager objReader = MyObjFileManager();
		vector<MyObject*> objs;
		vector<MyPointLight*> lights;
		MyCamera plainCamera = MyCamera();
		MyRayTracer raytracer = MyRayTracer();

>>Set the camera, read *.obj file and create the lights

		plainCamera.position = vec3(0,2,5);
		plainCamera.UpdateDirection();
		
		objs = objReader.readObject("cb.obj", 1.0);
		
		MyPointLight* light1 = new MyPointLight();
		light1->position = vec3(-2.5,1,-1);
		light1->energy = 12.0;
		light1->lightColour = Colour(255,130,160);
		lights.push_back(light1);

>>Initialise the RayTracer class

		raytracer.Initialize(window, objs, lights, &plainCamera);

>>Render

		raytracer.Render();

>>More feature including soft shadow, bump mapping, exponential fog, etc. see 'RedNoise_RT.cpp' Example file

=============================================================


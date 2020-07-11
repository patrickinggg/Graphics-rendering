#include <MyTexture.h>

using namespace std;

class MyMaterial{
  public:
    Colour* diffuse_color;
    Colour* ambient_color;
    Colour* specular_color;
    float specular_exponent;
	
	  int hasTexture = 0;
	  MyTexture* diffuse_texture;
    int hasBump = 0;
    MyTexture* bump_texture;

    float medium_reflectWeight = 0.2;
    float medium_refractWeight = 0.0;
    float medium_refract = 1.5;
    
	
	  // obsolete, use diffuse_color instead.
    Colour* mat_color;

    MyMaterial(){
        Colour* disabled = new Colour(0,0,0);
        diffuse_color = disabled;
        ambient_color = disabled;
        specular_color = disabled;
        specular_exponent = 1.0;
    }
    
};
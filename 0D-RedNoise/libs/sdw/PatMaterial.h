#include <Pat_Colour.h>
using namespace std;

class PatMaterial{
  public:
    Pat_Colour diffuse_color;
    Pat_Colour ambient_color;
    Pat_Colour specular_color;
    float specular_exponent;

    PatMaterial(){
      Pat_Colour disabled = Pat_Colour(0,0,0);
      diffuse_color = disabled;
      ambient_color = disabled;
      specular_color = disabled;
      specular_exponent = 1.0;
    }

};

using namespace std;
using namespace glm;

class MyPointLight{


  public:
      vec3 position;
      Colour lightColour;
      float energy;

      MyPointLight(){
          position = vec3(0,0,0);
          lightColour = Colour(255,255,255);
          energy = 1.0;
      }

      MyPointLight(glm::vec3 pos){
          position = pos;
          lightColour = Colour(255,255,255);
          energy = 1.0;
      }
};

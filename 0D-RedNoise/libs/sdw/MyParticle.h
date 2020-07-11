
using namespace std;
using namespace glm;

class MyParticle
{
  public:
    vec3 position;
    Colour particleBrush;

    MyParticle()
    {
        position = vec3(0, 0, 0);
        particleBrush = Colour(255, 255, 255);
    }


};
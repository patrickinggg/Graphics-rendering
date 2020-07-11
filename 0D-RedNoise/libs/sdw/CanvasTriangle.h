#include "CanvasPoint.h"
#include <iostream>
// #include "PatMaterial.h"
class CanvasTriangle
{
  public:
    CanvasPoint vertices[3];
    Colour colour;
    PatMaterial material;

    CanvasTriangle()
    {
    }

    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      colour = Colour(255,255,255);
    }

    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2, Colour c)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      colour = c;
    }
    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2, PatMaterial mat)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      material = mat;
    }
};

std::ostream& operator<<(std::ostream& os, const CanvasTriangle& triangle)
{
    os << triangle.vertices[0]  << triangle.vertices[1]  << triangle.vertices[2] << std::endl;
    return os;
}

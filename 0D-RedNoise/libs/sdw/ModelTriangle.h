#include <glm/glm.hpp>
#include "Colour.h"
#include <PatMaterial.h>
#include <string>

class ModelTriangle
{
  public:
    glm::vec3 vertices[3];
    glm::vec3 normals[3];
    glm::vec3 face_normal;
    Colour colour;
    PatMaterial material;

    ModelTriangle()
    {
    }
    ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
    }
    ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour trigColour)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      colour = trigColour;
    }

    ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, PatMaterial mat)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      material = mat;
    }
};

std::ostream& operator<<(std::ostream& os, const ModelTriangle& triangle)
{
    os << "(" << triangle.vertices[0].x << ", " << triangle.vertices[0].y << ", " << triangle.vertices[0].z << ")" << std::endl;
    os << "(" << triangle.vertices[1].x << ", " << triangle.vertices[1].y << ", " << triangle.vertices[1].z << ")" << std::endl;
    os << "(" << triangle.vertices[2].x << ", " << triangle.vertices[2].y << ", " << triangle.vertices[2].z << ")" << std::endl;
    os << std::endl;
    return os;
}

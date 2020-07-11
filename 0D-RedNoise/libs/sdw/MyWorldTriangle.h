#include <glm/glm.hpp>

class MyWorldTriangle
{
  public:
    glm::vec3 vertices[3];
    glm::vec3 normals[3];
    glm::vec2 texcoord[3];
    MyMaterial* mtl;

    MyWorldTriangle()
    {
    }

};


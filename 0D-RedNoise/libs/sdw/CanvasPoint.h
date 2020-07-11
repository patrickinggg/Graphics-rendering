#include "TexturePoint.h"
#include <iostream>

class CanvasPoint
{
  public:
    float x;
    float y;
    double depth;
    glm::vec3 pat_brightness;
    float brightness;
    float intensity;
    TexturePoint texturePoint;
    glm::vec3 original_vecPoint;
    glm::vec3 normal;

    CanvasPoint()
    {
        texturePoint = TexturePoint(-1,-1);
    }

    CanvasPoint(float xPos, float yPos)
    {
      x = xPos;
      y = yPos;
      depth = 0.0;
      brightness = 1.0f;
      texturePoint = TexturePoint(-1,-1);
    }

    CanvasPoint(float xPos, float yPos, float pointDepth)
    {
      x = xPos;
      y = yPos;
      depth = pointDepth;
      brightness = 1.0f;
      texturePoint = TexturePoint(-1,-1);
    }

    CanvasPoint(float xPos, float yPos, float pointDepth, glm::vec3 pointBrightness)
    {
      x = xPos;
      y = yPos;
      depth = pointDepth;
      pat_brightness = pointBrightness;
      texturePoint = TexturePoint(-1,-1);
    }

    CanvasPoint(float xPos, float yPos, float pointDepth, float pointBrightness)
    {
      x = xPos;
      y = yPos;
      depth = pointDepth;
      brightness = pointBrightness;
      texturePoint = TexturePoint(-1,-1);
    }

};

std::ostream& operator<<(std::ostream& os, const CanvasPoint& point)
{
    os << "(" << point.x << ", " << point.y << ", " << point.depth << ") " << point.brightness << std::endl;
    return os;
}

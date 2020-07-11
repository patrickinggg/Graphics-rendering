

#include <iostream>

class Pat_Colour
{
  public:
    std::string name;

    float red;
    float green;
    float blue;

    Pat_Colour()
    {
    }


    Pat_Colour(float r, float g, float b)
    {
      name = "";
      red = r;
      green = g;
      blue = b;

    }

};

std::ostream& operator<<(std::ostream& os, const Pat_Colour& colour)
{
    os << colour.name << " [" << colour.red << ", " << colour.green << ", " << colour.blue << "]" << std::endl;
    return os;
}

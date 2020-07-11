class PatObject
{
  public:
    std::string name;
    std::vector<ModelTriangle> triangles;


    PatObject(std::string name1, std::vector<ModelTriangle> triangles1)
    {
      name = name1;
      triangles = triangles1;

    }
};

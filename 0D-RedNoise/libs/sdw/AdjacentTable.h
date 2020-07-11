using namespace std;
using namespace glm;


class AdjacentTable{
  public:
    vec3 vertex;
    vector<ModelTriangle> adj_faces;

    AdjacentTable(vec3 ver, std::vector<ModelTriangle> faces){
        vertex = ver;
        adj_faces = faces;
    }

};

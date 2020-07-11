
using namespace std;
using namespace glm;

class MyObject
{
  public:
    string name;
    vector<MyWorldTriangle*> mesh;

    MyObject()
    {
    }

    void Translate(vec3 delta){
        for(int i = 0; i < (int)(mesh.size()); i++){
            for (int j = 0; j < 3; j++){
                (*(mesh[i])).vertices[j] = (*(mesh[i])).vertices[j] + delta;
            }
        }
    }

    void DeltaUV(float deltaU, float deltaV, int meshIdx){
        if (meshIdx == -1){
            for (int i = 0; i < (int)(mesh.size()); i++){
                for (int j = 0; j < 3; j++){
                    (*(mesh[i])).texcoord[j].x += deltaU;
                    (*(mesh[i])).texcoord[j].y += deltaV;
                }
            }
        }else{
            for (int j = 0; j < 3; j++){
                (*(mesh[meshIdx])).texcoord[j].x += deltaU;
                (*(mesh[meshIdx])).texcoord[j].y += deltaV;
            }
        }
    }


};
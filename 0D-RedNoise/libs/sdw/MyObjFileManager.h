#include <MyMaterial.h>
#include <MyWorldTriangle.h>
#include <MyObject.h>

using namespace std;
using namespace glm;

class MyObjFileManager{
  private:
    string mtlFileName;
    MyObject* currentObj;
    MyMaterial* currentMat;
    vector<vec3*> verticesRepo;
    vector<vec3*> normalsRepo;
    vector<vec2*> texCoordsRepo;
    map<string,MyMaterial*> matRepo;
    map<string,MyTexture*> texRepo;

  public:

    vector<MyObject*> readObject(string path, float scale){
        vector<MyObject*> objs;
        ifstream f(path, ios::in);
        while(!f.eof()){
            string str;
            getline(f, str);
            if (str.length() < 1){continue;}
            string* segs = split(str, 32);
            string arg = *segs;
            if (arg == "mtllib"){    // read materials
            mtlFileName = *(segs+1);
            ifstream f2(mtlFileName, ios::in);
            MyMaterial* tmpMat = new MyMaterial();
            while(!f2.eof()){
                string str2;
                getline(f2, str2);
                if (str2.length() < 1){continue;}
                string* segs2 = split(str2, 32);
                string arg2 = *segs2;
                if (arg2 == "newmtl"){
                    tmpMat = new MyMaterial();
                    matRepo[*(segs2+1)] = tmpMat;
                }
                else if (arg2 == "Kd"){
                    int tmpRed = (int)(255.0 * stof(*(segs2+1)));
                    int tmpGreen = (int)(255.0 * stof(*(segs2+2)));
                    int tmpBlue = (int)(255.0 * stof(*(segs2+3)));
                    Colour* tmpC = new Colour(tmpRed, tmpGreen, tmpBlue);
                    (*tmpMat).diffuse_color = tmpC;
                }
                else if (arg2 == "Ka"){
                    int tmpRed = (int)(255.0 * stof(*(segs2+1)));
                    int tmpGreen = (int)(255.0 * stof(*(segs2+2)));
                    int tmpBlue = (int)(255.0 * stof(*(segs2+3)));
                    Colour* tmpC = new Colour(tmpRed, tmpGreen, tmpBlue);
                    (*tmpMat).ambient_color = tmpC;
                }
                else if (arg2 == "Ks"){
                    int tmpRed = (int)(255.0 * stof(*(segs2+1)));
                    int tmpGreen = (int)(255.0 * stof(*(segs2+2)));
                    int tmpBlue = (int)(255.0 * stof(*(segs2+3)));
                    Colour* tmpC = new Colour(tmpRed, tmpGreen, tmpBlue);
                    (*tmpMat).specular_color = tmpC;
                }
                else if (arg2 == "Ns"){
                    float tmpValue = stof(*(segs2+1));
                    (*tmpMat).specular_exponent = tmpValue;
                }
                else if (arg2 == "map_Kd"){
                    string tmpTexPath = *(segs2+1);
                    (*tmpMat).hasTexture = 1;
                    if (texRepo.find(tmpTexPath) == texRepo.end()){
                        MyTexture* tmpTex = new MyTexture();
                        // read ppm file
                        tmpTex->name = tmpTexPath;
                        tmpTex->path = tmpTexPath;
                        (*tmpTex).ReadPPM();
                        
                        texRepo[tmpTexPath] = tmpTex;
                        (*tmpMat).diffuse_texture = tmpTex;
                    }else{
                        (*tmpMat).diffuse_texture = texRepo[tmpTexPath];
                    }
                }
                else if (arg2 == "bump"){
                    string tmpTexPath = *(segs2+1);
                    (*tmpMat).hasBump = 1;
                    if (texRepo.find(tmpTexPath) == texRepo.end()){
                        MyTexture* tmpTex = new MyTexture();
                        // read ppm file
                        tmpTex->name = tmpTexPath;
                        tmpTex->path = tmpTexPath;
                        (*tmpTex).ReadPPM();

                        texRepo[tmpTexPath] = tmpTex;
                        (*tmpMat).bump_texture = tmpTex;
                    }else{
                        (*tmpMat).bump_texture = texRepo[tmpTexPath];
                    }
                }
                else if (arg2 == "MyRefractMedium"){
                    float tmpValue = stof(*(segs2+1));
                    (*tmpMat).medium_refract = tmpValue;
                }
                else if (arg2 == "MyRefractWeight"){
                    float tmpValue = stof(*(segs2+1));
                    (*tmpMat).medium_refractWeight = tmpValue;
                }
                else if (arg2 == "MyReflectWeight"){
                    float tmpValue = stof(*(segs2+1));
                    (*tmpMat).medium_reflectWeight = tmpValue;
                }
            }
            f2.close();

            }
            else if(arg == "o"){
                currentObj = new MyObject();
                objs.push_back(currentObj);
                (*currentObj).name = *(segs+1);
                }
            else if(arg == "usemtl"){
                MyMaterial* tmpMat = matRepo[*(segs+1)];
                currentMat = tmpMat;
            }
            else if(arg == "v"){
                float tmpX = stof(*(segs+1));
                float tmpY = stof(*(segs+2));
                float tmpZ = stof(*(segs+3));
                vec3* tmpVec = new vec3(tmpX * scale, tmpY * scale, tmpZ * scale);
                verticesRepo.push_back(tmpVec);
            }
            else if(arg == "vn"){  // normals
                float tmpX = stof(*(segs+1));
                float tmpY = stof(*(segs+2));
                float tmpZ = stof(*(segs+3));
                vec3* tmpNor = new vec3(tmpX, tmpY, tmpZ);
                normalsRepo.push_back(tmpNor);
            }
            else if(arg == "vt"){  // texCoords
                float tmpU = stof(*(segs+1));
                float tmpV = stof(*(segs+2));
                vec2* tmpTexCoord = new vec2(tmpU, tmpV);
                texCoordsRepo.push_back(tmpTexCoord);
            }
            else if(arg == "f"){
                string* f_args1 = split(*(segs+1), '/');
                string* f_args2 = split(*(segs+2), '/');
                string* f_args3 = split(*(segs+3), '/');
                int index1 = stoi(*f_args1);
                int index2 = stoi(*f_args2);
                int index3 = stoi(*f_args3);
                int nor_idx1 = stoi(*(f_args1+2));
                int nor_idx2 = stoi(*(f_args2+2));
                int nor_idx3 = stoi(*(f_args3+2));

                MyWorldTriangle* tmpTri = new MyWorldTriangle();
                tmpTri->vertices[0] = *verticesRepo[index1 - 1];
                tmpTri->vertices[1] = *verticesRepo[index2 - 1];
                tmpTri->vertices[2] = *verticesRepo[index3 - 1];
                tmpTri->normals[0] = *normalsRepo[nor_idx1 - 1];
                tmpTri->normals[1] = *normalsRepo[nor_idx2 - 1];
                tmpTri->normals[2] = *normalsRepo[nor_idx3 - 1];
                
                string hasTexcoord = *(f_args1+1);
                if (!(hasTexcoord == "")){
                    int texc_idx1 = stoi(*(f_args1+1));
                    int texc_idx2 = stoi(*(f_args2+1));
                    int texc_idx3 = stoi(*(f_args3+1));
                    tmpTri->texcoord[0] = *texCoordsRepo[texc_idx1 - 1];
                    tmpTri->texcoord[1] = *texCoordsRepo[texc_idx2 - 1];
                    tmpTri->texcoord[2] = *texCoordsRepo[texc_idx3 - 1];
                }
                tmpTri->mtl = currentMat;
                (*currentObj).mesh.push_back(tmpTri);
            }

        }
        f.close();
        return objs;
    }


};
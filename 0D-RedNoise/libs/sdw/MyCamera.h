
using namespace std;
using namespace glm;

class MyCamera{
  public:
    // simple one with only position and f
    vec3 position;
    vec3 rotation;
    float f;

    // complex one will have fov
    float near = 1.0;
    float far = 10.0;

    mat3 dirMat;

    MyCamera(){
      position = vec3(0,0,10);
      rotation = vec3(0,0,0);
      f = -1;
    }
    

    void UpdateDirection(){
      mat3 rx = mat3(1.0, 0, 0,
                    0, cos(rotation.x), -sin(rotation.x),
                    0, sin(rotation.x), cos(rotation.x));
      mat3 ry = mat3(cos(rotation.y), 0, sin(rotation.y),
                    0, 1.0, 0,
                    -sin(rotation.y), 0, cos(rotation.y));
      mat3 rz = mat3(cos(rotation.z), -sin(rotation.z), 0,
                    sin(rotation.z), cos(rotation.z), 0,
                    0, 0, 1.0);
      dirMat = rz*ry*rx;   
    }

    void LookAt(vec3 targetPos){
      vec3 tmpDir = targetPos - position;
      tmpDir = MyMatrix::normalize(tmpDir);
      float tmpX = -asin(tmpDir.y);
      float tmpY = -atan(tmpDir.x / tmpDir.z);
      rotation = vec3(tmpX, tmpY, 0);
      UpdateDirection();
    }

    MyMatrix GetWV(){
      MyMatrix m1 = MyMatrix(4);
      m1.setTranslate(-position.x, -position.y, -position.z);
      MyMatrix m2 = MyMatrix(4);
      m2.setRotateX(rotation.x);
      MyMatrix m3 = MyMatrix(4);
      m3.setRotateY(rotation.y);
      MyMatrix m4 = MyMatrix(4);
      m4.setRotateZ(rotation.z);
      MyMatrix result = m1.dot(m4.dot(m3.dot(m2)));
      return result;
    }
};
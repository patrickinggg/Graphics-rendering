
using namespace std;
using namespace glm;

class MyMatrix{
  public:
    mat4 glm_mat = mat4();
    int matSize;

    MyMatrix(int inputMatSize){
        matSize = inputMatSize;
    }

    void setEye(float eye){
        for (int j = 0; j<4; j++){
            for (int i = 0; i<4; i++){
                glm_mat[j][i] = 0;
            }
        }
        for (int j = 0; j<4; j++){
            glm_mat[j][j] = eye;
        }
    }

    void setV(int row, int col, float value){
        glm_mat[col-1][row-1] = value;
    }

    float getV(int row, int col){
        return glm_mat[col-1][row-1];
    }

    MyMatrix dot(MyMatrix right){
        MyMatrix result = MyMatrix(4);
        result.glm_mat = glm_mat * right.glm_mat;
        return result;
    }

    vec3 dot3(vec3 right){
        vec4 input = vec4(right.x, right.y, right.z, 1.0);
        vec4 result4 = glm_mat * input;
        vec3 result3 = vec3(result4.x, result4.y, result4.z);
        return result3;
    }

    void setTranslate(float deltaX, float deltaY, float deltaZ){
        setEye(1.0);
        setV(1, 4, deltaX);
        setV(2, 4, deltaY);
        setV(3, 4, deltaZ);
    }

    void setRotateX(float theta){
        setEye(1.0);
        setV(2, 2, cos(theta));
        setV(2, 3, -sin(theta));
        setV(3, 2, sin(theta));
        setV(3, 3, cos(theta));
    }
    void setRotateY(float theta){
        setEye(1.0);
        setV(1, 1, cos(theta));
        setV(1, 3, sin(theta));
        setV(3, 1, -sin(theta));
        setV(3, 3, cos(theta));
    }
    void setRotateZ(float theta){
        setEye(1.0);
        setV(1, 1, cos(theta));
        setV(1, 2, -sin(theta));
        setV(2, 1, sin(theta));
        setV(2, 2, cos(theta));
    }

    void fromMat3(mat3 inputMat){
        //TODO
    }


    static vec3 normalize(vec3 input){
        float ratio = sqrt(pow(input.x,2)+pow(input.y,2)+pow(input.z,2));
        vec3 result = vec3(input.x/ratio, input.y/ratio, input.z/ratio);
        return result;
    }

};



using namespace std;
using namespace glm;

class MyPhysicsForce{
  public:
    vec3 forceValue = vec3(0, 0, 0);
    int forceGroup = 0;

    MyPhysicsForce(){
    }

};

class MyPhysicsTarget{
  private:
    MyObject* ref_obj;
    MyParticle* ref_par;

  public:
    int objType = 0;
    float mass = 1.0;
    vec3 velocity = vec3(0,0,0);
    int forceGroup = 0;
    vec3 bBoxCentre = vec3(0,0,0);
    vec3 bBoxSize = vec3(1,1,1);
    int applyGravity = 0;

    MyPhysicsTarget(int targetType){
        objType = targetType;
    }

    void SetRefObj(MyObject* inputObj){
        ref_obj = inputObj;
    }
    void SetRefPar(MyParticle* inputPar){
        ref_par = inputPar;
    }

    void Run(float time){
        if (objType == 0){
            (*ref_obj).Translate(velocity * time); 
        }else{
            if ((*ref_par).position.y < -10.0){return;}
            (*ref_par).position += (velocity * time);
        }
    }


};

class MyPhysics
{
  public:
    float frameRate = 0.03333;  // 30fps
    vector<MyPhysicsTarget*> targetObjects;
    vector<MyPhysicsForce*> forceField;
    int applyGravity = 1;
    vec3 gravityValue = vec3(0, -9.8, 0);

    MyPhysics()
    {
    }

    void RunFrame(){
        for (int i = 0; i < (int)(targetObjects.size()); i++){
            MyPhysicsTarget* target = targetObjects[i];
            for (int j = 0; j < (int)(forceField.size()); j++){
                MyPhysicsForce* tmpForce = forceField[j];
                if (target->forceGroup == tmpForce->forceGroup){
                    vec3 tmpForceValue = tmpForce->forceValue;
                    float tmpMass = target->mass;
                    vec3 tmpAccelerate = tmpForceValue / tmpMass;
                    (*target).velocity += (frameRate * tmpAccelerate);
                }
            }
            if (applyGravity){
                if (target->applyGravity){
                    (*target).velocity += (frameRate * gravityValue);
                }
            }
            (*target).Run(frameRate);
        }
    }



};
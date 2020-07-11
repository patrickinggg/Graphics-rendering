
using namespace std;
using namespace glm;

#ifndef WIDTH
#define WIDTH 640
#endif

#ifndef HEIGHT
#define HEIGHT 480
#endif

class MyRasteriser{
  private:
    DrawingWindow window;  // default drawing window of sdl
    MyCamera* camera;       // default camera
    vector<MyObject*> targetObjects;  // models used in ray tracing
    vector<MyPointLight*> targetLights;  // lighting used in ray tracing

    float depthBuffer[WIDTH][HEIGHT];  // depth buffer of rasteriser

    float* interpolation(float from, float to, int numOfVal){
        if (numOfVal <= 1){
            float* r = new float[1];
            *r = from;
            return r;
        }
        float* result = new float[numOfVal];
        float dist = to - from;
        float interval = dist / (numOfVal-1);
        for (int i = 0; i<numOfVal; i++){
            *(result+i) = from + i*interval;
        }
        return result;
    }

    void drawTriangle(CanvasTriangle triangle){
        drawLine(triangle.vertices[0], triangle.vertices[1], triangle.colour);
        drawLine(triangle.vertices[1], triangle.vertices[2], triangle.colour);
        drawLine(triangle.vertices[2], triangle.vertices[0], triangle.colour);
    }

    void drawLine(CanvasPoint p1, CanvasPoint p2, Colour colour){
        float h_dist = p1.x - p2.x;
        if (h_dist < 0) {h_dist = - h_dist;}
        float v_dist = p1.y - p2.y;
        if (v_dist < 0) {v_dist = - v_dist;}
        int max_dist = (int)round(h_dist);
        if (v_dist > h_dist) {max_dist = (int)round(v_dist);}
        max_dist += 1;

        float* h_interp = interpolation(p1.x, p2.x, max_dist);
        float* v_interp = interpolation(p1.y, p2.y, max_dist);
        
        float red = colour.red;
        float green = colour.green;
        float blue = colour.blue;
        uint32_t colorInt = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);

        for (int i = 0; i < max_dist; i++){
            float x = *(h_interp+i);
            float y = *(v_interp+i);
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
                window.setPixelColour(x, y, colorInt);
            }
        }

        delete []h_interp;
        delete []v_interp;
    }


  public:
    MyPhysics physicsEngine = MyPhysics();  // default physics
    vector<MyParticle*> targetParticles;

    MyRasteriser(){  // no arg constructor
      // test here
    }

    void Initialize(DrawingWindow inputWindow, vector<MyObject*> inputObjs,
                    vector<MyPointLight*> inputLights, MyCamera* inputCamera){
        window = inputWindow;
        targetObjects = inputObjs;
        targetLights = inputLights;
        camera = inputCamera;
    }

    void RenderBackground(){
        window.clearPixels();
        for(int y=0; y<window.height ;y++) {
            for(int x=0; x<window.width ;x++) {
            float red = 0;
            float green = 0;
            float blue = 0;
            uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
            window.setPixelColour(x, y, colour);
            }
        }
    }

    void RenderWireFrame(){
        //RenderBackground();
        for (int i = 0; i < (int)(targetObjects.size()); i++){
            MyObject* tmpObj = targetObjects[i];
            for (int j = 0; j < (int)((*tmpObj).mesh.size()); j++){
                MyWorldTriangle* tmpTri = (*tmpObj).mesh[j];
                CanvasTriangle screenTri = CanvasTriangle();
                for (int k = 0; k < 3; k++){
                    vec3 vtx = (*tmpTri).vertices[k];
                    
                    MyMatrix mat_view = (*camera).GetWV();
                    vec3 vec_view = mat_view.dot3(vtx);
                    vec3 vec_proj = vec3(vec_view.x * ((*camera).f / vec_view.z), 
                        vec_view.y * ((*camera).f / vec_view.z), 
                        (*camera).f);
                    
                    screenTri.vertices[k] = CanvasPoint(0.5*WIDTH + 0.5*HEIGHT*vec_proj.x,
                        0.5*HEIGHT*(1-vec_proj.y));
                }
                screenTri.colour = Colour(178,178,178);
                drawTriangle(screenTri);
            }
        }
    }

    void RenderLightTag(){
        for (int i = 0; i < (int)(targetLights.size()); i++){
            vec3 tmpLightPos = (*(targetLights[i])).position;
            Colour tmpLightBrush = (*(targetLights[i])).lightColour;

            MyMatrix mat_view = (*camera).GetWV();
            vec3 vec_view = mat_view.dot3(tmpLightPos);
            vec3 vec_proj = vec3(vec_view.x * ((*camera).f / vec_view.z), 
                        vec_view.y * ((*camera).f / vec_view.z), 
                        (*camera).f);
                    
            CanvasPoint tmpPoint = CanvasPoint(0.5*WIDTH + 0.5*HEIGHT*vec_proj.x,
                        0.5*HEIGHT*(1-vec_proj.y));
            CanvasPoint p1 = CanvasPoint(tmpPoint.x, tmpPoint.y-10);
            CanvasPoint p2 = CanvasPoint(tmpPoint.x-10, tmpPoint.y);
            CanvasPoint p3 = CanvasPoint(tmpPoint.x, tmpPoint.y+10);
            CanvasPoint p4 = CanvasPoint(tmpPoint.x+10, tmpPoint.y);
            drawLine(p1,p2,tmpLightBrush);
            drawLine(p2,p3,tmpLightBrush);
            drawLine(p3,p4,tmpLightBrush);
            drawLine(p4,p1,tmpLightBrush);
            drawLine(p1,p3,tmpLightBrush);
            drawLine(p2,p4,tmpLightBrush);
        }
    }

    void RenderParticleTag(){
        for (int i = 0; i < (int)(targetParticles.size()); i++){
            vec3 tmpPos = (*(targetParticles[i])).position;
            Colour tmpBrush = (*(targetParticles[i])).particleBrush;

            MyMatrix mat_view = (*camera).GetWV();
            vec3 vec_view = mat_view.dot3(tmpPos);
            vec3 vec_proj = vec3(vec_view.x * ((*camera).f / vec_view.z), 
                        vec_view.y * ((*camera).f / vec_view.z), 
                        (*camera).f);
                    
            CanvasPoint tmpPoint = CanvasPoint(0.5*WIDTH + 0.5*HEIGHT*vec_proj.x,
                        0.5*HEIGHT*(1-vec_proj.y));
            //culling
            if (tmpPoint.x >= 0 && tmpPoint.x <= WIDTH && tmpPoint.y >= 0 && tmpPoint.y <= HEIGHT){
                CanvasPoint p1 = CanvasPoint(tmpPoint.x-2, tmpPoint.y-2);
                CanvasPoint p2 = CanvasPoint(tmpPoint.x-2, tmpPoint.y+2);
                CanvasPoint p3 = CanvasPoint(tmpPoint.x+2, tmpPoint.y+2);
                CanvasPoint p4 = CanvasPoint(tmpPoint.x+2, tmpPoint.y-2);
                drawLine(p1,p2,tmpBrush);
                drawLine(p2,p3,tmpBrush);
                drawLine(p3,p4,tmpBrush);
                drawLine(p4,p1,tmpBrush);
            }

        }
    }

    void ClearDepthBuffer(){
        // clear depth buffer
        for (int j = 0; j < HEIGHT; j++){
            for (int i = 0; i < WIDTH; i++){
                depthBuffer[i][j] = 0;
            }
        }
    }


};
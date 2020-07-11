
using namespace std;
using namespace glm;

#ifndef WIDTH
#define WIDTH 640
#endif

#ifndef HEIGHT
#define HEIGHT 480
#endif

class MyRayIntersectResult{
  public:
    vec3 tuv;
    MyWorldTriangle triangle;

    MyRayIntersectResult(){
		tuv = vec3(-1, -1, -1);
    }

    MyRayIntersectResult(vec3 inputTuv, MyWorldTriangle inputTriangle){
		tuv = inputTuv;
		triangle = inputTriangle;
    }
	
	vec3 GetPosition(){
        vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		vec3 result = triangle.vertices[0] + tuv.y * e0 + tuv.z * e1;
		return result;
	}
	
	vec3 GetNormal(){
        vec3 e0n = triangle.normals[1] - triangle.normals[0];
        vec3 e1n = triangle.normals[2] - triangle.normals[0];
		vec3 worldNormal = triangle.normals[0] + tuv.y * e0n + tuv.z * e1n;  // phong
        if ((triangle.mtl)->hasBump){
			vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
			vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
			vec2 e0t = triangle.texcoord[1] - triangle.texcoord[0];
			vec2 e1t = triangle.texcoord[2] - triangle.texcoord[0];
            vec2 worldTex = triangle.texcoord[0] + tuv.y * e0t + tuv.z * e1t;
			float* tmpBumpValue = (*(*(triangle.mtl)).bump_texture).GetPixel_CentreFloat(worldTex.x, worldTex.y);
			float bumpT = tmpBumpValue[0];
			float bumpB = tmpBumpValue[1];
			float bumpN = tmpBumpValue[2];
			vec3 dirN = worldNormal;
			vec3 dirT = e1t.y * e0 + e0t.y * e1;
			vec3 dirB = e1t.x * e0 + e0t.x * e1;
			dirT = MyMatrix::normalize(dirT);
			dirB = MyMatrix::normalize(dirB);
			worldNormal = bumpT*dirT + bumpB*dirB + bumpN*dirN;
			worldNormal = MyMatrix::normalize(worldNormal);
		}
		return worldNormal;
	}

	float GetMediumReflectWeight(){
		return (*(triangle.mtl)).medium_reflectWeight;
	}
	float GetMediumRefractWeight(){
		return (*(triangle.mtl)).medium_refractWeight;
	}
	float GetMediumRefract(){
		return (*(triangle.mtl)).medium_refract;
	}
};

class MyRay{
  public:
    vec3 from;
	vec3 dir;
	float energy = 0;
	vec3 normal;  // source normal, optional
	float medium = 1.0;  // refraction param, optional

	int enable = 0;
	
	MyRay(){
	}
};

class MyRayTracer{
  private:
    vector<MyObject*> targetObjects;  // models used in ray tracing
    vector<MyPointLight*> targetLights;  // lighting used in ray tracing
    DrawingWindow window;             // default sdl window
	
	vector<float> pixelDiffuseBuffer; // {a,r,g,b}
	vector<float> pixelSpecularBuffer;
	vector<float> pixelAmbientBuffer; // {r,g,b}

    std::default_random_engine rand;

    MyRayIntersectResult TryRayIntersect(vec3 pos, vec3 rayDir, float minDepth){
      MyRayIntersectResult result = MyRayIntersectResult();
      float currentDepth = 9999.9;   // far plane
      for (int i = 0; i < (int)(targetObjects.size()); i++){
        MyObject* tmpObj = targetObjects[i];
        for (int j = 0; j < (int)((*tmpObj).mesh.size()); j++){
          MyWorldTriangle tmpTri = *((*tmpObj).mesh[j]);

          vec3 e0 = tmpTri.vertices[1] - tmpTri.vertices[0];
          vec3 e1 = tmpTri.vertices[2] - tmpTri.vertices[0];
          vec3 sp = pos - tmpTri.vertices[0];
          mat3 de = mat3(-rayDir, e0, e1);
          vec3 tuv = inverse(de) * sp;

          if (tuv.y>=0.0 && tuv.y <= 1.0 && tuv.z >=0.0 && tuv.z <= 1.0 &&
              tuv.y+tuv.z<=1.0){
            if (tuv.x > minDepth && tuv.x < currentDepth){  // depth testing
              currentDepth = tuv.x;
              result.tuv = tuv;
              result.triangle = tmpTri;
            }
          }

        }
      }
      return result;
    }

	MyRay* Ray_Refr_Refl(MyRay* inputRay, MyRayIntersectResult surface){
		MyRay* result = new MyRay[2];
		// reflect
		vec3 worldNormal = surface.GetNormal();
		vec3 worldPos = surface.GetPosition();
		float perpendicular = glm::dot((*inputRay).dir, worldNormal);
		vec3 lightSrcMir = (*inputRay).dir - 2*perpendicular*worldNormal;
        vec3 accurateLSM = 1.0f * lightSrcMir;
        if (enableNoise){
            double noiseX = nd(rand); double noiseY = nd(rand); double noiseZ = nd(rand);
            MyMatrix m1 = MyMatrix(4); m1.setRotateX(noiseX);
            MyMatrix m2 = MyMatrix(4); m2.setRotateY(noiseY);
            MyMatrix m3 = MyMatrix(4); m3.setRotateZ(noiseZ);
            MyMatrix m4 = m3.dot(m2.dot(m1));
            lightSrcMir = m4.dot3(lightSrcMir);
        }

		result[0] = MyRay();
		result[0].from = worldPos;
		result[0].dir = lightSrcMir;
		result[0].energy = (*inputRay).energy * surface.GetMediumReflectWeight(); 
		result[0].normal = worldNormal;
		result[0].medium = (*inputRay).medium;
		result[0].enable = 1;
		// refract
		int isInside = 0;
		if (perpendicular > 0.0){isInside = 1;}
		result[1] = MyRay();
		if (isInside == 0){
			float refractRate = surface.GetMediumRefract() / (*inputRay).medium;
			float theta1 = acos(-perpendicular);
			if (theta1 >= 0.0 && theta1 < 1.5707){
				float theta2 = asin(sin(theta1) / refractRate);
				vec3 norm2ray = (*inputRay).dir + worldNormal;
				vec3 dirRefract = worldNormal * (-1.0f)
						+ norm2ray * (sin(theta2)/(sin(theta2)+sin(theta1-theta2)));
				dirRefract = MyMatrix::normalize(dirRefract);
                if (enableNoise){
                    double noiseX = nd(rand); double noiseY = nd(rand); double noiseZ = nd(rand);
                    MyMatrix m1 = MyMatrix(4); m1.setRotateX(noiseX);
                    MyMatrix m2 = MyMatrix(4); m2.setRotateY(noiseY);
                    MyMatrix m3 = MyMatrix(4); m3.setRotateZ(noiseZ);
                    MyMatrix m4 = m3.dot(m2.dot(m1));
                    dirRefract = m4.dot3(dirRefract);
                }
				result[1].from = worldPos;
				result[1].dir = dirRefract;
				result[1].energy = (*inputRay).energy * surface.GetMediumRefractWeight(); 
				result[1].normal = worldNormal;
				result[1].medium = surface.GetMediumRefract();
				result[1].enable = 1;
			}
		}else{
			float refractRate = 1.0 / (*inputRay).medium;
			float theta1 = acos(perpendicular);
			if (theta1 >= 0.0 && theta1 < 1.5707){
				float tmpSinTheta2 = sin(theta1) / refractRate;
				if (tmpSinTheta2 <= 1.0){
					float theta2 = asin(tmpSinTheta2);
					float theta1_half = 0.5*theta1;
					vec3 norm2ray = (*inputRay).dir - worldNormal;
					vec3 dirRefract = worldNormal 
							+  norm2ray*((sin(theta1_half) + cos(theta1_half)*tan(theta2-theta1_half))
							/(2.0f*sin(theta1_half)));
					dirRefract = MyMatrix::normalize(dirRefract);
                    if (enableNoise){
                        double noiseX = nd(rand); double noiseY = nd(rand); double noiseZ = nd(rand);
                        MyMatrix m1 = MyMatrix(4); m1.setRotateX(noiseX);
                        MyMatrix m2 = MyMatrix(4); m2.setRotateY(noiseY);
                        MyMatrix m3 = MyMatrix(4); m3.setRotateZ(noiseZ);
                        MyMatrix m4 = m3.dot(m2.dot(m1));
                        dirRefract = m4.dot3(dirRefract);
                    }
					result[1].from = worldPos;
					result[1].dir = dirRefract;
					result[1].energy = (*inputRay).energy * surface.GetMediumRefractWeight(); 
					result[1].normal = -1.0f * worldNormal;
					result[1].medium = surface.GetMediumRefract();
					result[1].enable = 1;
				}else{
					// total reflect
                    result[0].dir = accurateLSM;
					result[0].energy += (*inputRay).energy * surface.GetMediumRefractWeight(); 
				}

			}
		}
		if (result[0].energy < 0.001){result[0].enable = 0;}
		if (result[1].energy < 0.001){result[1].enable = 0;}
		return result;
	}
	
	void RayIteration(MyRay* inputRay, int iterationDepth){
		if (iterationDepth >= 8){return;}    // max iteration
		if (inputRay->enable == 0){return;}

		vec3 rayDir = (*inputRay).dir;
		MyRayIntersectResult rayResult = TryRayIntersect((*inputRay).from, rayDir, AcneThreshold);
		if (rayResult.tuv.x > 0){    // render
			MyRay* raySplit = Ray_Refr_Refl(inputRay, rayResult);
			MyWorldTriangle tmpTri = rayResult.triangle;
			vec3 tuv = rayResult.tuv;
			vec3 e0 = tmpTri.vertices[1] - tmpTri.vertices[0];
			vec3 e1 = tmpTri.vertices[2] - tmpTri.vertices[0];
			vec3 e0n = tmpTri.normals[1] - tmpTri.normals[0];
			vec3 e1n = tmpTri.normals[2] - tmpTri.normals[0];
			vec2 e0t = tmpTri.texcoord[1] - tmpTri.texcoord[0];
			vec2 e1t = tmpTri.texcoord[2] - tmpTri.texcoord[0];
			
			vec3 worldPos = tmpTri.vertices[0] + tuv.y * e0 + tuv.z * e1;
			vec3 worldNormal = tmpTri.normals[0] + tuv.y * e0n + tuv.z * e1n;  // phong
			worldNormal = MyMatrix::normalize(worldNormal);
			vec2 worldTex = tmpTri.texcoord[0] + tuv.y * e0t + tuv.z * e1t;

			// base colour of object
			// base texture
			Colour brush = *((tmpTri.mtl)->diffuse_color);
			if ((tmpTri.mtl)->hasTexture){
				unsigned char* tmpTexColour = (*(*(tmpTri.mtl)).diffuse_texture).GetPixel(worldTex.x, worldTex.y);
				float texRed = tmpTexColour[0] * brush.red / 255.0;
				float texGreen = tmpTexColour[1] * brush.green / 255.0;
				float texBlue = tmpTexColour[2] * brush.blue / 255.0;
				brush = Colour((int)texRed, (int)texGreen, (int)texBlue);
				delete[] tmpTexColour;
			}
			Colour brushSpec = *((tmpTri.mtl)->specular_color);

			if ((tmpTri.mtl)->hasBump){
				float* tmpBumpValue = (*(*(tmpTri.mtl)).bump_texture).GetPixel_CentreFloat(worldTex.x, worldTex.y);

				float bumpT = tmpBumpValue[0];
				float bumpB = tmpBumpValue[1];
				float bumpN = tmpBumpValue[2];
				vec3 dirN = worldNormal;
				vec3 dirT = e1t.y * e0 + e0t.y * e1;
				vec3 dirB = e1t.x * e0 + e0t.x * e1;
				dirT = MyMatrix::normalize(dirT);
				dirB = MyMatrix::normalize(dirB);
				worldNormal = bumpT*dirT + bumpB*dirB + bumpN*dirN;
				worldNormal = MyMatrix::normalize(worldNormal);
			}
			
			float red = brush.red; float green = brush.green; float blue = brush.blue;
			float redS = brushSpec.red; float greenS = brushSpec.green; float blueS = brushSpec.blue;
			// light colour
			float redLight = 0.0; float greenLight = 0.0; float blueLight = 0.0;  // diffuse(ambient) factor
			float redSpec = 0.0; float greenSpec = 0.0; float blueSpec = 0.0;    // spec factor
			
			for (int k = 0; k < (int)(targetLights.size()); k++){
				vec3 lightPos = targetLights[k]->position;
				float lightDistance = sqrt(pow(worldPos.x-lightPos.x,2)+pow(worldPos.y-lightPos.y,2)
										+pow(worldPos.z-lightPos.z,2));
				vec3 lightDir = MyMatrix::normalize(lightPos - worldPos);
				// soft shadow - plane elevation
				float shadowValue = 0.0;
				for (int ss_idx = -SoftShadowElevation; ss_idx <= SoftShadowElevation; ss_idx++){
					float ss_moveup = (SoftShadowDistance*ss_idx/SoftShadowElevation);
					vec3 ss_worldPos = worldPos + ss_moveup * worldNormal;
					float shadowRayResult = ShadowRayTracing(ss_worldPos, lightPos);
					shadowValue += (1.0*shadowRayResult);
				}
				shadowValue /= (2.0*SoftShadowElevation + 1.0);
				if (shadowValue < 0.01){continue;}
				
				// diffuse light part1
				float brightness = targetLights[k]->energy/(4*3.1416*lightDistance);
				// diffuse light part2
				float perpendicular = glm::dot(lightDir, worldNormal);
				if (perpendicular > 1.0) {perpendicular = 1.0;}
				else if (perpendicular < -1.0) {perpendicular = -1.0;}
				if (perpendicular > 0){
					brightness *= perpendicular;
				} else {brightness = 0.0;}
				brightness *= shadowValue;  // soft shadow factor
				// diffuse light reflect
				if (iterationDepth != 0){
					float reflectDist = rayResult.tuv.x;
					float reflectEmit = targetLights[k]->energy * 0.5 * perpendicular * 2;
					float brightness2 = reflectEmit/(4*3.1416*(lightDistance+reflectDist));
					float perpendicular2 = glm::dot(rayDir, (*inputRay).normal);
					if (perpendicular2 > 1.0) {perpendicular2 = 1.0;}
					else if (perpendicular2 < -1.0) {perpendicular2 = -1.0;}
					if (perpendicular2 < 0) {perpendicular2 = -perpendicular2;}
					if (perpendicular2 > 0 && perpendicular > 0){
						brightness2 *= (perpendicular2 * perpendicular);
					} else {brightness2 = 0.0;}
					brightness = brightness2;
				}
				// diffuse blend
				if (brightness > 1.0){brightness = 1.0;}
				redLight += (brightness*targetLights[k]->lightColour.red / 255.0);
				greenLight += (brightness*targetLights[k]->lightColour.green / 255.0);
				blueLight += (brightness*targetLights[k]->lightColour.blue / 255.0);
		
				// specular light
				vec3 lightSrcMir = lightDir - (2.0f*perpendicular)*worldNormal;
				float spec_value = glm::dot(rayDir,lightSrcMir);
				if (spec_value < 0.0){spec_value = 0.0;}
				float spec_brightness = pow(spec_value, (tmpTri.mtl)->specular_exponent);
				spec_brightness *= shadowValue;  // soft shadow factor
				// specular blend
				redSpec += (spec_brightness*targetLights[k]->lightColour.red/255.0);
				greenSpec += (spec_brightness*targetLights[k]->lightColour.green/255.0);
				blueSpec += (spec_brightness*targetLights[k]->lightColour.blue/255.0);
			}
			if (redLight > 1.0){redLight = 1.0;}if (greenLight > 1.0){greenLight = 1.0;}if (blueLight > 1.0){blueLight = 1.0;}
			//red*=redLight; green*=greenLight; blue*=blueLight;
			pixelDiffuseBuffer.push_back((*inputRay).energy);
			pixelDiffuseBuffer.push_back(red);
			pixelDiffuseBuffer.push_back(green);
			pixelDiffuseBuffer.push_back(blue);
			pixelDiffuseBuffer.push_back(redLight);
			pixelDiffuseBuffer.push_back(greenLight);
			pixelDiffuseBuffer.push_back(blueLight);
			
			if (redSpec > 1.0){redSpec = 1.0;}if (greenSpec > 1.0){greenSpec = 1.0;}if (blueSpec > 1.0){blueSpec = 1.0;}
			//redS*=redSpec; greenS*=greenSpec; blueS*=blueSpec;
			pixelSpecularBuffer.push_back((*inputRay).energy);
			pixelSpecularBuffer.push_back(redS);
			pixelSpecularBuffer.push_back(greenS);
			pixelSpecularBuffer.push_back(blueS);
			pixelSpecularBuffer.push_back(redSpec);
			pixelSpecularBuffer.push_back(greenSpec);
			pixelSpecularBuffer.push_back(blueSpec);

			if (iterationDepth == 0){
				pixelAmbientBuffer.push_back(((tmpTri.mtl)->ambient_color)->red);
				pixelAmbientBuffer.push_back(((tmpTri.mtl)->ambient_color)->green);
				pixelAmbientBuffer.push_back(((tmpTri.mtl)->ambient_color)->blue);
			}
			
			// split
			RayIteration(&raySplit[0], iterationDepth+1);
			RayIteration(&raySplit[1], iterationDepth+1);
			delete[] raySplit;
		}
	}
	
  public:
    MyCamera* camera;
    float AcneThreshold = 0.01;

	int SoftShadowElevation = 5;
    float SoftShadowDistance = 0.1;

    int enableFog = 0;
    Colour fogBrush = Colour(0,0,0);
    float fogDensity = 0.0;  // exponential fog

    int enableNoise = 0;
    std::normal_distribution<double> nd;

    MyRayTracer(){  // no arg constructor
      // test here
    }

    // load resources
    void Initialize(DrawingWindow inputWindow, vector<MyObject*> inputObjs,
                    vector<MyPointLight*> inputLights, MyCamera* inputCamera){
      window = inputWindow;
      targetObjects = inputObjs;
      targetLights = inputLights;
      camera = inputCamera;
    }

    void Render(){
		float halfWidth = WIDTH*0.5;
		float halfHeight = HEIGHT*0.5;
		// super sampling - Quincunx
		Colour ssamp_scanBuffer[WIDTH+2][2];
		int ssamp_scanBufferOrder = 0;
		for (int j = 0; j <= HEIGHT; j++){
			for (int i = 0; i <= WIDTH; i++){
				//Colour tmpPixel = GetRenderPixel((i-halfWidth)/halfWidth, (halfHeight-j)/halfHeight);
				Colour tmpPixel = GetRenderPixel((i-halfWidth)/halfHeight, (halfHeight-j)/halfHeight);
				ssamp_scanBuffer[i][ssamp_scanBufferOrder] = tmpPixel;
			}
			ssamp_scanBufferOrder = 1 - ssamp_scanBufferOrder;
			if (j == 0){continue;}
			float centrePixY = j - 0.5;
			for (int i = 0; i < WIDTH; i++){
				float centrePixX = i + 0.5;
                //Colour tmpPixel = GetRenderPixel((centrePixX-halfWidth)/halfWidth, (halfHeight-centrePixY)/halfHeight);
                Colour tmpPixel = GetRenderPixel((centrePixX-halfWidth)/halfHeight, (halfHeight-centrePixY)/halfHeight);
				Colour finalPixel = SuperSampling_QuincunxBlend(ssamp_scanBuffer[i][0], ssamp_scanBuffer[i+1][0],
																ssamp_scanBuffer[i][1], ssamp_scanBuffer[i+1][1],
																tmpPixel);

				uint32_t colorInt = (255<<24) + ((int)(finalPixel.red)<<16) + ((int)(finalPixel.green)<<8) 
									+ (int)(finalPixel.blue);
				window.setPixelColour(i, j-1, colorInt);
			}
		}

    }

    Colour GetRenderPixel(float x, float y){
      	vec3 defaultDir = vec3(x, y, (*camera).f);
      	vec3 rayDir = (*camera).dirMat * defaultDir;
      	rayDir = MyMatrix::normalize(rayDir);
	  
		vector<float>().swap(pixelDiffuseBuffer);
		vector<float>().swap(pixelSpecularBuffer);
		vector<float>().swap(pixelAmbientBuffer);

		MyRay* cameraRay = new MyRay();
		cameraRay->from = (*camera).position;
		cameraRay->dir = rayDir;
		cameraRay->energy = 1.0;
		cameraRay->enable = 1;

		RayIteration(cameraRay, 0);
		
		float red = 0.0; float green = 0.0; float blue = 0.0;
		float redS = 0.0; float greenS = 0.0; float blueS = 0.0;
		// int diffuseBundleCount = (int)(pixelDiffuseBuffer.size() / 7);
		// for (int i = (diffuseBundleCount-1); i >= 0; i--){
		// 	int index = i*7;
		// 	float tmpRedLight = pixelDiffuseBuffer[index+4] + red;
		// 	float tmpGreenLight = pixelDiffuseBuffer[index+5] + green;
		// 	float tmpBlueLight = pixelDiffuseBuffer[index+6] + blue;
		// 	if (tmpRedLight > 1.0){tmpRedLight = 1.0;}
		// 	if (tmpGreenLight > 1.0){tmpGreenLight = 1.0;}
		// 	if (tmpBlueLight > 1.0){tmpBlueLight = 1.0;}
		// 	float tmpAlpha = pixelDiffuseBuffer[index];
		// 	red = pixelDiffuseBuffer[index+1] * tmpRedLight * tmpAlpha / 255.0;
		// 	green = pixelDiffuseBuffer[index+2] * tmpGreenLight * tmpAlpha / 255.0;
		// 	blue = pixelDiffuseBuffer[index+3] * tmpBlueLight * tmpAlpha / 255.0;
		// }
		// red *= 255.0; green *= 255.0; blue *= 255.0;
		for (int i = 0; i < (int)(pixelDiffuseBuffer.size()); i+=7){
			float tmpAlpha = pixelDiffuseBuffer[i];
			float tmpRed = pixelDiffuseBuffer[i+1] * pixelDiffuseBuffer[i+4];
			float tmpGreen = pixelDiffuseBuffer[i+2] * pixelDiffuseBuffer[i+5];
			float tmpBlue = pixelDiffuseBuffer[i+3] * pixelDiffuseBuffer[i+6];
			red += (tmpAlpha * tmpRed);
			green += (tmpAlpha * tmpGreen);
			blue += (tmpAlpha * tmpBlue);
		}
		for (int i = 0; i < (int)(pixelSpecularBuffer.size()); i+=7){
			float tmpAlpha = pixelSpecularBuffer[i];
			float tmpRed = pixelSpecularBuffer[i+1] * pixelSpecularBuffer[i+4];
			float tmpGreen = pixelSpecularBuffer[i+2] * pixelSpecularBuffer[i+5];
			float tmpBlue = pixelSpecularBuffer[i+3] * pixelSpecularBuffer[i+6];
			redS += (tmpAlpha * tmpRed);
			greenS += (tmpAlpha * tmpGreen);
			blueS += (tmpAlpha * tmpBlue);
		}
		// ambient light
		if (pixelAmbientBuffer.size() == 3){
			red += pixelAmbientBuffer[0];
			green += pixelAmbientBuffer[1];
			blue += pixelAmbientBuffer[2];
		}
		red += redS; green += greenS; blue += blueS;
        // fog
        if (enableFog){
            float tmpFogFactor = 0.0;
            MyRayIntersectResult firstRayCollide = TryRayIntersect(
                            cameraRay->from, cameraRay->dir, AcneThreshold);
            if (firstRayCollide.tuv.x > 0){
                tmpFogFactor = 1.0 / pow(2.71828, (firstRayCollide.tuv.x * fogDensity));
                if (tmpFogFactor > 1.0){tmpFogFactor = 1.0;}
            }
            red = tmpFogFactor * red + (1.0-tmpFogFactor) * fogBrush.red;
            green = tmpFogFactor * green + (1.0-tmpFogFactor) * fogBrush.green;
            blue = tmpFogFactor * blue + (1.0-tmpFogFactor) * fogBrush.blue;
        }
		if (red > 255){red = 255;}if (green > 255){green = 255;}if (blue > 255){blue = 255;}

		delete cameraRay; 
		return Colour(int(red), int(green), int(blue));
    }

	float ShadowRayTracing(vec3 from, vec3 light){
		vec3 lightDir = MyMatrix::normalize(light - from);
		float lightDistance = sqrt(pow(from.x-light.x,2)+pow(from.y-light.y,2)
								+pow(from.z-light.z,2));
        MyRayIntersectResult shadowRayResult = TryRayIntersect(from, lightDir, AcneThreshold);
        if (shadowRayResult.tuv.x > AcneThreshold && shadowRayResult.tuv.x < lightDistance){
			if (shadowRayResult.GetMediumRefractWeight() > 0.5){return 0.5;}  // transparent
			// obstruct
            return 0;  // dark
        }
		return 1;  // bright
	}
	
	Colour SuperSampling_QuincunxBlend(Colour corner1, Colour corner2, Colour corner3, Colour corner4, Colour centre){
		float r = (float)(corner1.red)*0.125 + (float)(corner2.red)*0.125 + (float)(corner3.red)*0.125
					+ (float)(corner4.red)*0.125 + (float)(centre.red)*0.5;
		float g = (float)(corner1.green)*0.125 + (float)(corner2.green)*0.125 + (float)(corner3.green)*0.125
					+ (float)(corner4.green)*0.125 + (float)(centre.green)*0.5;
		float b = (float)(corner1.blue)*0.125 + (float)(corner2.blue)*0.125 + (float)(corner3.blue)*0.125
					+ (float)(corner4.blue)*0.125 + (float)(centre.blue)*0.5;
		return Colour((int)(r), (int)(g), (int)(b));
	}

};

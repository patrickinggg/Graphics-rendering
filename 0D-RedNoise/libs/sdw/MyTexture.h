
using namespace std;

class MyTexture{
  private:
    float* PixelInterpolation(unsigned char* a, unsigned char* b, float progress){
		float* value = new float[3];
		float r1 = float(a[0]); float g1 = float(a[1]); float b1 = float(a[2]);
		float r2 = float(b[0]); float g2 = float(b[1]); float b2 = float(b[2]);
		float rv = r1 + (r2-r1)*progress;
		float gv = g1 + (g2-g1)*progress;
		float bv = b1 + (b2-b1)*progress;
		value[0] = rv;
		value[1] = gv;
		value[2] = bv;
		return value;
	}

	float* PixelInterpolation(float* a, float* b, float progress){
		float* value = new float[3];
		float r1 = a[0]; float g1 = a[1]; float b1 = a[2];
		float r2 = b[0]; float g2 = b[1]; float b2 = b[2];
		float rv = r1 + (r2-r1)*progress;
		float gv = g1 + (g2-g1)*progress;
		float bv = b1 + (b2-b1)*progress;
		value[0] = rv;
		value[1] = gv;
		value[2] = bv;
		return value;
	}

  public:
    string name;
	string path;
	int tex_height = 0;
	int tex_width = 0;
	char* tex_buffer;
	
    MyTexture(){
    }
	
	void Dispose(){
		delete[] tex_buffer;
	}
	
	void InitializeBuffer(){
		tex_buffer = new char[tex_height*tex_width*3];
	}
	
	void ReadPPM(){
		char buffer1[100];
		int bufferCursor = 0;
		int pixelCursor = 0;
		char status = 0;
		int readSpeed = 1;
		ifstream f(path, ios::binary);
		
		while(!f.eof()){
			char buffer2[3];
			f.read(buffer2, readSpeed);
			if (status == 3){  // content
				for (int rgbIndex = 0; rgbIndex < 3; rgbIndex++){
					tex_buffer[pixelCursor + rgbIndex] = buffer2[rgbIndex];
				}
				pixelCursor += 3;
				if (pixelCursor >= tex_height*tex_width*3){break;}
			}
			else if (status < 10){  // header
				if (buffer2[0] == '#'){
					status += 10;
				}
				else if (buffer2[0] == 10){
					if (status == 0){
						// P6
					}
					else if (status == 1){  // width height
						string s = buffer1;
						int spaceAt = s.find(32);
						tex_width = stoi(s.substr(0,spaceAt));
						tex_height = stoi(s.substr(spaceAt));
						InitializeBuffer();
					}
					else if (status == 2){
						// 255
					}
					status += 1;
					bufferCursor = 0;
					if (status == 3){
						readSpeed = 3;
					}
				}
				else{
					buffer1[bufferCursor] = buffer2[0];
					bufferCursor += 1;
				}
			}
			else{  // comment
				if (buffer2[0] == 10){
					status -= 10;
				}
			}
		}
		f.close();
	}
	
	unsigned char* GetRawPixel(int x, int y){
		unsigned char* colour = new unsigned char[3];
		int pixPos = (x + y*tex_width) * 3;
		for (int i = 0; i < 3; i++){
			int tmpColour = tex_buffer[pixPos + i];
			if (tmpColour < 0){
				tmpColour += 256;
			}
			colour[i] = (unsigned char)tmpColour;
		}
		return colour;
	}
	
	unsigned char* GetPixel(float u, float v){
		float trueX = u * tex_width;
		float trueY = v * tex_height;
		int lowerX = (int)(floor(trueX));
		int lowerY = (int)(floor(trueY));
		float progX = trueX - lowerX;
		float progY = trueY - lowerY;
		while (lowerX < 0){lowerX += tex_width;}
		if (lowerX >= tex_width){lowerX = lowerX % tex_width;}
		while (lowerY < 0){lowerY += tex_height;}
		if (lowerY >= tex_height){lowerY = lowerY % tex_height;}
		int upperX = lowerX + 1;
		int upperY = lowerY + 1;
		while (upperX < 0){upperX += tex_width;}
		if (upperX >= tex_width){upperX = upperX % tex_width;}
		while (upperY < 0){upperY += tex_height;}
		if (upperY >= tex_height){upperY = upperY % tex_height;}
		unsigned char* PixTL = GetRawPixel(lowerX, lowerY);
		unsigned char* PixBL = GetRawPixel(lowerX, upperY);
		unsigned char* PixTR = GetRawPixel(upperX, lowerY);
		unsigned char* PixBR = GetRawPixel(upperX, upperY);
		float* PixL = PixelInterpolation(PixTL, PixBL, progY);
		float* PixR = PixelInterpolation(PixTR, PixBR, progY);
		float* value = PixelInterpolation(PixL, PixR, progX);

		unsigned char* result = new unsigned char[3];
		result[0] = (unsigned char)((int)(value[0]));
		result[1] = (unsigned char)((int)(value[1]));
		result[2] = (unsigned char)((int)(value[2]));
		// dispose
		delete[] PixTL;	delete[] PixBL;	delete[] PixTR;	delete[] PixBR;
		delete[] PixL; delete[] PixR; delete[] value;
		return result;
	}

	float* GetPixel_CentreFloat(float u, float v){
		float trueX = u * tex_width;
		float trueY = v * tex_height;
		int lowerX = (int)(floor(trueX));
		int lowerY = (int)(floor(trueY));
		float progX = trueX - lowerX;
		float progY = trueY - lowerY;
		while (lowerX < 0){lowerX += tex_width;}
		if (lowerX >= tex_width){lowerX = lowerX % tex_width;}
		while (lowerY < 0){lowerY += tex_height;}
		if (lowerY >= tex_height){lowerY = lowerY % tex_height;}
		int upperX = lowerX + 1;
		int upperY = lowerY + 1;
		while (upperX < 0){upperX += tex_width;}
		if (upperX >= tex_width){upperX = upperX % tex_width;}
		while (upperY < 0){upperY += tex_height;}
		if (upperY >= tex_height){upperY = upperY % tex_height;}
		unsigned char* PixTL = GetRawPixel(lowerX, lowerY);
		unsigned char* PixBL = GetRawPixel(lowerX, upperY);
		unsigned char* PixTR = GetRawPixel(upperX, lowerY);
		unsigned char* PixBR = GetRawPixel(upperX, upperY);
		float* PixL = PixelInterpolation(PixTL, PixBL, progY);
		float* PixR = PixelInterpolation(PixTR, PixBR, progY);
		float* value = PixelInterpolation(PixL, PixR, progX);
		value[0] -= 128.0; value[1] -= 128.0; value[2] -= 128.0;
		value[0] /= 128.0; value[1] /= 128.0; value[2] /= 128.0;
		// dispose
		delete[] PixTL;	delete[] PixBL;	delete[] PixTR;	delete[] PixBR;
		delete[] PixL; delete[] PixR; 
		return value;
	}
	
	void SetRawPixel(int x, int y, uint32_t value){
		int red = (value>>16) & 0xFF;
		int green = (value>>8) & 0xFF;
		int blue = value & 0xFF;
		int pixPos = (x + y*tex_width) * 3;
		tex_buffer[pixPos] = (char)(red);
		tex_buffer[pixPos+1] = (char)(green);
		tex_buffer[pixPos+2] = (char)(blue);
	}

	void SetRawPixelColour(int x, int y, int inputR, int inputG, int inputB){
		if (inputR > 255){inputR = 255;}
		if (inputG > 255){inputG = 255;}
		if (inputB > 255){inputB = 255;}
		uint32_t tmpC = (255<<24) + (inputR<<16) + (inputG<<8) + inputB;
        SetRawPixel(x, y, tmpC);
	}
	
	void ReadScreen(DrawingWindow window){
		// need initBuffer first
		for (int j = 0; j < tex_height; j++){
			for (int i = 0; i < tex_width; i++){
				uint32_t tmpPixel = window.getPixelColour(i, j);
				SetRawPixel(i, j, tmpPixel);
			}
		}
	}
	
	void SavePPM(string outPath){
		ofstream f(outPath, ios::trunc|ios::binary);
		f<<"P6\n";
		string line2s = std::to_string(tex_width) + " " + std::to_string(tex_height) + "\n";
		f<<line2s;
		f<<"255\n";
		for (int j = 0; j < tex_height; j++){
			for (int i = 0; i < tex_width; i++){
				int pixPos = (i + j*tex_width) * 3;
				for (int k = 0; k < 3; k++){
					f.put(tex_buffer[pixPos + k]);
				}
			}
		}
		f.close();
	}
    
};

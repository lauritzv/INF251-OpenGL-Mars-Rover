#pragma once
#include <GL/glew.h>

class MaterialObject
{
public:
	GLuint TObjectDiffuse0 = -1, TObjectNormal0 = -1, TObjectSpecular0 = -1;
	MaterialObject(const char* pathfilenameDiff, const char* pathfilenameNorm, const char* pathfilenameSpec);
	MaterialObject(const char* pathfilenameDiff);
	bool successfullyImported = true;
private:
	unsigned int TWidth = 0, THeight = 0;
	unsigned char *TextureDataDiffuse = nullptr, *TextureDataNorm = nullptr, *TextureDataSpec = nullptr;
	bool initTexture(const char * pathfilename, GLuint & TObject, unsigned char *& TextureData);
};
